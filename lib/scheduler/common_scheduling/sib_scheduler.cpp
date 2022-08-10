/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sib_scheduler.h"
#include "../support/config_helpers.h"
#include "../support/dmrs_helpers.h"
#include "../support/pdcch/pdcch_type0_css_occasions.h"
#include "../support/prbs_calculator.h"
#include "srsgnb/ran/pdcch/pdcch_type0_css_coreset_config.h"
#include "srsgnb/ran/resource_allocation/resource_allocation_frequency.h"
#include "srsgnb/ran/sib_configuration.h"

using namespace srsgnb;

//  ------   Helper functions   ------ .

// Helper function that returns whether slot n0 (where UE should monitor Type0-PDCCH CSS) is in an even/odd frame:
// - 0 if slot n0 is located in an even frame.
// - 1 if slot n0 (where UE should monitor Type0-PDCCH CSS) is located in an odd frame.
static unsigned sib1_is_even_frame(unsigned sib1_offset, double sib1_M, uint8_t numerology_mu, unsigned ssb_index)
{
  // This is only used to retrieve the nof_slots_per_frame.
  slot_point sl_point{numerology_mu, 0};

  // Compute floor( ( O * 2^mu + floor(i*M) ) / nof_slots_per_frame  ) mod 2, as per TS 38.213, Section 13.
  unsigned is_even = static_cast<unsigned>(floor(static_cast<double>(sib1_offset << numerology_mu) +
                                                 floor(static_cast<double>(ssb_index) * sib1_M)) /
                                           sl_point.nof_slots_per_frame()) %
                     2;
  return is_even;
}

// Helper function that returns slot n0 (where UE should monitor Type0-PDCCH CSS) for a given SSB (beam) index.
static slot_point get_sib1_n0(unsigned sib1_offset, double sib1_M, subcarrier_spacing scs_common, unsigned ssb_index)
{
  // Initialize n0 to a slot_point = 0.
  uint8_t    numerology_mu = to_numerology_value(scs_common);
  slot_point sib_1_n0{numerology_mu, 0};

  // Compute n0 = ( O * 2^mu + floor(i*M)  )  % nof_slots_per_frame, as per TS 38.213, Section 13.
  sib_1_n0 += static_cast<unsigned>(static_cast<double>(sib1_offset << numerology_mu) +
                                    floor(static_cast<double>(ssb_index) * sib1_M)) %
              sib_1_n0.nof_slots_per_frame();

  // We want to express n0 as a value from 0 to max_nof_slots. Since the mod operation above cap n0 to
  // (nof_slots_per_frame - 1), we need to add nof_slots_per_frame to n0 if this falls into an odd frame.
  sib_1_n0 += sib1_is_even_frame(sib1_offset, sib1_M, numerology_mu, ssb_index) * sib_1_n0.nof_slots_per_frame();

  return sib_1_n0;
}

//  ------   Public methods   ------ .

sib1_scheduler::sib1_scheduler(const cell_configuration&                       cfg_,
                               pdcch_scheduler&                                pdcch_sch,
                               const sched_cell_configuration_request_message& msg) :
  cell_cfg{cfg_},
  pdcch_sched{pdcch_sch},
  coreset0{msg.coreset0},
  searchspace0{msg.searchspace0},
  sib1_mcs{msg.sib1_mcs},
  sib1_rv{msg.sib1_rv},
  sib1_dci_aggr_lev{msg.sib1_dci_aggr_lev},
  sib1_payload_size{msg.sib1_payload_size}
{
  // Compute derived SIB1 parameters.
  sib1_period =
      std::max(ssb_periodicity_to_value(cfg_.ssb_cfg.ssb_period), sib1_rtx_periodicity_to_value(msg.sib1_retx_period));

  precompute_sib1_n0(msg.scs_common);

  // Define a BWP configuration limited by CORESET#0 RBs.
  coreset0_bwp_cfg      = cell_cfg.dl_cfg_common.init_dl_bwp.generic_params;
  coreset0_bwp_cfg.crbs = get_coreset0_crbs(cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common);
};

void sib1_scheduler::schedule_sib1(cell_slot_resource_allocator& res_grid, slot_point sl_point)
{
  // NOTE:
  // - [Implementation defined] The UE monitors the SearchSpaceSet 0 for SIB1 in 2 consecutive slots, starting from n0.
  //   In this function, it is assumed that the GNB only allocates the PDCCH/DCI_1_0 for SIB1 in the first slot, i.e.,
  //   in n0.
  // - [Implementation defined] We assume the SIB1 is (re)transmitted every 20ms if the SSB periodicity <= 20ms.
  //   Else, we set the (re)transmission periodicity as the SSB's.

  // The sib1_period_slots is expressed in unit of slots.
  // NOTE: As sib1_period_slots is expressed in milliseconds or subframes, we need to this these into slots.
  unsigned sib1_period_slots = sib1_period * static_cast<unsigned>(sl_point.nof_slots_per_subframe());

  // Helper function that determines from SSB bitmap whether n-th beam is used.
  auto is_nth_ssb_beam_active = [](uint64_t ssb_bitmap, unsigned ssb_index) {
    // In the current implementation state, only SSB indices within the interval [0, 7] are allowed.
    srsgnb_sanity_check(
        ssb_index >= 0 and ssb_index < MAX_NUM_BEAMS, "SSB index must be within the interval [0, '{}')", MAX_NUM_BEAMS);
    return (ssb_bitmap & (static_cast<uint64_t>(0b1U) << static_cast<uint64_t>(63U - ssb_index))) > 0;
  };

  // For each beam, check if the SIB1 needs to be allocated in this slot.
  for (unsigned ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ssb_idx++) {
    // Do not schedule the SIB1 for the SSB indices that are not used.
    if (not is_nth_ssb_beam_active(cell_cfg.ssb_cfg.ssb_bitmap, ssb_idx)) {
      continue;
    }

    if (sl_point.to_uint() % sib1_period_slots == sib1_n0_slots[ssb_idx].to_uint()) {
      // Ensure slot for SIB1 has DL enabled.
      if (not cell_cfg.is_dl_enabled(sl_point)) {
        logger.error("SCHED: Could not allocated SIB1 for beam idx {} as slot is not DL enabled.", ssb_idx);
        return;
      }

      allocate_sib1(res_grid, ssb_idx);
    }
  }
}

//  ------   Private methods   ------ .

void sib1_scheduler::precompute_sib1_n0(subcarrier_spacing scs_common)
{
  // TODO: get minimum channel BW from current BW configuration.
  srsgnb_assert(cell_cfg.dl_carrier.carrier_bw_mhz < 40,
                "Minimum channel BW '{}'MHz not supported for SIB1 scheduling; scheduler only supports 5MHz or "
                "10MHz min. channel BW.",
                cell_cfg.dl_carrier.carrier_bw_mhz);
  // Get Coreset0 num of symbols from Coreset0 config.
  static const unsigned min_channel_bw    = 5;
  unsigned              nof_symb_coreset0 = pdcch_type0_css_coreset_get(min_channel_bw,
                                                           cell_cfg.ssb_cfg.scs,
                                                           scs_common,
                                                           coreset0,
                                                           static_cast<uint8_t>(cell_cfg.ssb_cfg.k_ssb.to_uint()))
                                   .nof_rb_coreset;

  // TODO: Extend function to all multiplexing patterns.
  // TODO: Embed is_fr2 in the scheduler configuration.
  pdcch_type0_css_occasion_pattern1_description ss0_config_occasion_param =
      pdcch_type0_css_occasions_get_pattern1(pdcch_type0_css_occasion_pattern1_configuration{
          .is_fr2 = false, .ss_zero_index = searchspace0, .nof_symb_coreset = nof_symb_coreset0});

  for (size_t i_ssb = 0; i_ssb < MAX_NUM_BEAMS; i_ssb++) {
    sib1_n0_slots.emplace_back(get_sib1_n0(
        static_cast<unsigned>(ss0_config_occasion_param.offset), ss0_config_occasion_param.M, scs_common, i_ssb));
  }
}

bool sib1_scheduler::allocate_sib1(cell_slot_resource_allocator& res_grid, unsigned beam_idx)
{
  // This is the list of parameters that are hard-coded and will need to be derived from some general config.
  static const ofdm_symbol_range sib1_ofdm_symbols{2, 14};
  static const unsigned          nof_symb_sh      = sib1_ofdm_symbols.length();
  static const unsigned          mod_order        = get_bits_per_symbol(modulation_scheme::QPSK);
  static const float             target_code_rate = 379.0f;
  static const unsigned          nof_layers       = 1;
  // Time resource will be passed to the next function to fill the DCI.
  static const unsigned time_resource = 0;
  // As per Section 5.1.3.2, TS 38.214, nof_oh_prb = 0 if PDSCH is scheduled by PDCCH with a CRC scrambled by
  // SI-RNTI.
  static const unsigned nof_oh_prb = 0;

  // Generate dmrs information to be passed to (i) the fnc that computes number of RE used for DMRS per RB and (ii) to
  // the fnc that fills the DCI.
  dmrs_information dmrs_info = make_dmrs_info_common(
      cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common, time_resource, cell_cfg.pci, cell_cfg.dmrs_typeA_pos);

  pdsch_prbs_tbs sib1_prbs_tbs = get_nof_prbs(prbs_calculator_pdsch_config{sib1_payload_size,
                                                                           nof_symb_sh,
                                                                           calculate_nof_dmrs_per_rb(dmrs_info),
                                                                           nof_oh_prb,
                                                                           mod_order,
                                                                           static_cast<float>(target_code_rate / 1024),
                                                                           nof_layers});

  // 1. Find available RBs in PDSCH for SIB1 grant.
  crb_interval sib1_crbs;
  {
    unsigned          nof_sib1_rbs = sib1_prbs_tbs.nof_prbs;
    const prb_bitmap& used_crbs    = res_grid.dl_res_grid.used_crbs(
        coreset0_bwp_cfg, cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[time_resource].symbols);
    sib1_crbs = find_empty_interval_of_length(used_crbs, nof_sib1_rbs, 0);
    if (sib1_crbs.length() < nof_sib1_rbs) {
      // early exit
      logger.error("SCHED: Not enough PDSCH space for SIB1 in beam idx: {}", beam_idx);
      return false;
    }
  }

  // 2. Allocate DCI_1_0 for SIB1 on PDCCH.
  pdcch_dl_information* pdcch =
      pdcch_sched.alloc_pdcch_common(res_grid,
                                     rnti_t::SI_RNTI,
                                     cell_cfg.dl_cfg_common.init_dl_bwp.pdcch_common.sib1_search_space_id,
                                     sib1_dci_aggr_lev);
  if (pdcch == nullptr) {
    logger.warning("SCHED: Could not allocated SIB1's DCI in PDCCH for beam idx: {}", beam_idx);
    return false;
  }

  // 3. Now that we are sure there is space in both PDCCH and PDSCH, set SIB1 CRBs as used.
  // NOTE:
  // - ofdm_symbol_range{2, 14} is a temporary hack. The OFDM symbols should be derived from the SIB1 size and
  //   frequency allocation.
  res_grid.dl_res_grid.fill(grant_info{
      grant_info::channel::sch, cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.scs, sib1_ofdm_symbols, sib1_crbs});

  // 4. Delegate filling SIB1 grants to helper function.
  fill_sib1_grant(res_grid, sib1_crbs, time_resource, dmrs_info, sib1_prbs_tbs.tbs_bytes);
  logger.info("SCHED: SIB1, cell={}, SSB beam idx: {}, crbs={}", res_grid.cfg.cell_index, beam_idx, sib1_crbs);
  return true;
}

void sib1_scheduler::fill_sib1_grant(cell_slot_resource_allocator& res_grid,
                                     crb_interval                  sib1_crbs_grant,
                                     unsigned                      time_resource,
                                     const dmrs_information&       dmrs_info,
                                     unsigned                      tbs)
{
  // System information indicator for SIB1, in DCI 1_0. Refer to Section 7.3.1.2.1 and Table 7.3.1.2.1-2, TS 38.212.
  static const unsigned sib1_si_indicator = 0;

  // Add DCI to list to dl_pdcch.
  srsgnb_assert(res_grid.result.dl.dl_pdcchs.size() > 0, "No DL PDCCH grant found in the DL sched results.");
  auto&        sib1_pdcch = res_grid.result.dl.dl_pdcchs.back();
  prb_interval sib1_prbs  = crb_to_prb(coreset0_bwp_cfg, sib1_crbs_grant);

  // Fill SIB1 DCI.
  sib1_pdcch.dci.type                = dci_dl_rnti_config_type::si_f1_0;
  sib1_pdcch.dci.si_f1_0             = {};
  dci_1_0_si_rnti_configuration& dci = sib1_pdcch.dci.si_f1_0;
  dci.N_rb_dl_bwp                    = coreset0_bwp_cfg.crbs.length();
  dci.frequency_resource             = ra_frequency_type1_get_riv(
      ra_frequency_type1_configuration{dci.N_rb_dl_bwp, sib1_prbs.start(), sib1_prbs.length()});
  // TODO: compute time_domain_assigment from OFDM symbols (WIP).
  dci.time_resource                = time_resource;
  dci.vrb_to_prb_mapping           = 0; // TODO.
  dci.modulation_coding_scheme     = sib1_mcs;
  dci.redundancy_version           = sib1_rv;
  dci.system_information_indicator = sib1_si_indicator;

  // Add SIB1 to list of SIB1 information to pass to lower layers.
  res_grid.result.dl.bc.sibs.emplace_back();
  sib_information& sib1 = res_grid.result.dl.bc.sibs.back();
  sib1.pdcch_cfg        = &res_grid.result.dl.dl_pdcchs.back();
  sib1.si_indicator     = sib_information::si_indicator_type::sib1;

  // Fill PDSCH configuration.
  pdsch_information& pdsch = sib1.pdsch_cfg;
  pdsch.rnti               = sib1_pdcch.ctx.rnti;
  pdsch.bwp_cfg            = sib1_pdcch.ctx.bwp_cfg;
  pdsch.symbols = cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common.pdsch_td_alloc_list[dci.time_resource].symbols;
  pdsch.prbs    = sib1_prbs;
  // As per TS 38.211, Section 7.3.1.1, n_ID is set to Physical Cell ID for SIB1.
  pdsch.n_id = cell_cfg.pci;
  pdsch.codewords.emplace_back();
  pdsch_codeword& cw             = pdsch.codewords.back();
  cw.rv_index                    = dci.redundancy_version;
  cw.mcs_index                   = dci.modulation_coding_scheme;
  cw.mcs_table                   = pdsch_mcs_table::qam64;
  sch_mcs_description mcs_config = pdsch_mcs_get_config(cw.mcs_table, cw.mcs_index);
  cw.qam_mod                     = mcs_config.modulation;
  cw.target_code_rate            = mcs_config.target_code_rate;
  // This is hard-coded, and derived as per Section 5.1.3.2, TS38.214, with nof PRBs= 5, MCS=5, N_RB_sc = 12,
  // N_sh_symb = 12, N_PRBs_DMRS = 36, N_PRBs_oh = 0.
  cw.tb_size_bytes = static_cast<uint32_t>(tbs);
  pdsch.dmrs       = dmrs_info;
}
