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
#include "cell/resource_grid.h"

using namespace srsgnb;

// TODO: Check if this can be moved to a different file.
// Refer to "SS/PBCH block and CORESET multiplexing pattern", TS 38.213, Section 13.
enum class ssb_coreset0_mplex_pattern { mplx_pattern1 = 0, mplx_pattern2, mplx_pattern3, mplex_invalid };

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
static slot_point get_sib1_n0(unsigned sib1_offset, unsigned sib1_M, uint8_t numerology_mu, unsigned ssb_index)
{
  // Initialize n0 to a slot_point = 0.
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

// Dummy function that returns O and M values given pdcch_config_sib1 from Table 13-11, TS 38.213.
// TODO: replace this with proper function from PHY team.
static void
get_sib1_offset_M(unsigned& offset, double& sib1_M, ssb_coreset0_mplex_pattern mplex_pattern, uint8_t pdcch_config_sib1)
{
  srsran_sanity_check(mplex_pattern == ssb_coreset0_mplex_pattern::mplx_pattern1,
                      "Only SSB/Coreset0 multiplexing pattern 1 is currently supported");
  switch (mplex_pattern) {
    case ssb_coreset0_mplex_pattern::mplx_pattern1:
      offset = 5;
      sib1_M = 2;
      break;
    default:
      // Only ssb_coreset0_mplex_pattern::mplx_pattern1 is currently supported.
      return;
  }
}

//  ------   Public methods   ------ .

sib1_scheduler::sib1_scheduler(pdcch_scheduler& pdcch_sch, uint8_t pdcch_config_sib1, unsigned numerology) :
  pdcch_sched{pdcch_sch}
{
  precompute_sib1_n0(pdcch_config_sib1, numerology);
};

void sib1_scheduler::schedule_sib1(cell_slot_resource_allocator& res_grid,
                                   const slot_point              sl_point,
                                   uint8_t                       ssb_periodicity,
                                   uint64_t                      ssb_in_burst_bitmap)
{
  // NOTE:
  // - [Implementation defined] The UE monitors the SearchSpaceSet 0 for SIB1 in 2 consecutive slots, starting from n0.
  //   In this function, it is assumed that the GNB only allocates the PDCCH/DCI_1_0 for SIB1 in the first slot, i.e.,
  //   in n0.
  // - [Implementation defined] We assume the SIB1 is (re)transmitted every 20ms if the SSB periodicity <= 20ms.
  //   Else, we set the (re)transmission periodicity as the SSB's.

  // The sib1_period is expressed in slots.
  // NOTE: As ssb_periodicity and SIB1_RETX_PERIODICITY_FR1 are expressed in nof_slots_per_subframe or
  // nof_slots_per_millisecond, we need to convert these into slots.
  unsigned sib1_period =
      std::max(static_cast<unsigned>(ssb_periodicity), SIB1_RETX_PERIODICITY_FR1) * sl_point.nof_slots_per_subframe();

  // Helper function that determines from SSB bitmap whether n-th beam is used.
  auto is_nth_ssb_beam_active = [](uint64_t ssb_bitmap, unsigned ssb_index) {
    // In the current implementation state, only SSB indices within the interval [0, 7] are allowed.
    srsran_sanity_check(
        ssb_index >= 0 and ssb_index < MAX_NUM_BEAMS, "SSB index must be within the interval [0, '{}')", MAX_NUM_BEAMS);
    return (ssb_bitmap & (static_cast<uint64_t>(0b1U) << static_cast<uint64_t>(63U - ssb_index))) > 0;
  };

  // For each beam, check if the SIB1 needs to be allocated in this slot .
  for (unsigned ssb_idx = 0; ssb_idx < MAX_NUM_BEAMS; ssb_idx++) {
    // Do not schedule the SIB1 for the SSB indices that are not used.
    if (not is_nth_ssb_beam_active(ssb_in_burst_bitmap, ssb_idx)) {
      continue;
    }

    if (sl_point.to_uint() % sib1_period == sib1_n0_slots[ssb_idx].to_uint()) {
      allocate_sib1(res_grid, ssb_idx);
    }
  }
}


void sib1_scheduler::precompute_sib1_n0(uint8_t pdcch_config_sib1, unsigned numerology)
{
  // This corresponds to parameter O in TS 38.213, Section 13.
  unsigned sib1_offset;
  // This corresponds to parameter M in TS 38.213, Section 13.
  double sib1_M;

  // TODO: Extend function to all multiplexing patterns
  get_sib1_offset_M(sib1_offset, sib1_M, ssb_coreset0_mplex_pattern::mplx_pattern1, pdcch_config_sib1);

  for (size_t i_ssb = 0; i_ssb < MAX_NUM_BEAMS; i_ssb++) {
    sib1_n0_slots[i_ssb] = get_sib1_n0(sib1_offset, sib1_M, numerology, pdcch_config_sib1);
  }
}

//  ------   Private methods   ------ .

bool sib1_scheduler::allocate_sib1(cell_slot_resource_allocator& res_grid, unsigned beam_idx)
{
  // TODO: PBRs for SIB1 should be derived from the SIB1 size.
  const unsigned nof_prbs_per_sib1 = 4;

  const cell_configuration& cfg = res_grid.cfg;

  // 1. Find available RBs in PDSCH for SIB1 grant.
  crb_interval sib1_crbs;
  {
    // TODO: find a better way to derive the SIB1 PRBs
    unsigned          nof_sib1_rbs = nof_prbs_per_sib1;
    const prb_bitmap& used_crbs    = res_grid.dl_res_grid.sch_crbs(cfg.dl_cfg_common.init_dl_bwp.generic_params);
    sib1_crbs                      = find_empty_interval_of_length(used_crbs, nof_sib1_rbs, 0);
    if (sib1_crbs.length() < nof_sib1_rbs) {
      // early exit
      logger.error("SCHED: Not enough PDSCH space for SIB1 in beam idx: {}", beam_idx);
      return false;
    }
  }

  // 2. Allocate DCI_1_0 for SIB1 on PDCCH.
  const static aggregation_level aggr_lvl = aggregation_level::n8;
  search_space_id                ss_id    = cfg.dl_cfg_common.init_dl_bwp.pdcch_common.sib1_search_space_id;
  pdcch_dl_information*          pdcch    = pdcch_sched.alloc_pdcch_common(res_grid, rnti_t::SI_RNTI, ss_id, aggr_lvl);
  if (pdcch == nullptr) {
    logger.error("SCHED: Could not allocated SIB1's DCI in PDCCH for beam idx: {}", beam_idx);
    return false;
  }

  // 3. Now that we are sure there is space in both PDCCH and PDSCH, set SIB1 CRBs as used.
  // NOTEs:
  // - ofdm_symbol_range{2, 14} is a temporary hack. The OFDM symbols should be derived from the SIB1 size and
  //   frequency allocation.
  res_grid.dl_res_grid.fill(grant_info{
      grant_info::channel::sch, cfg.dl_cfg_common.init_dl_bwp.generic_params.scs, ofdm_symbol_range{2, 14}, sib1_crbs});

  // 4. Delegate filling SIB1 grands to helper function.
  fill_sib1_grant(res_grid, beam_idx, sib1_crbs);
  return true;
}

void sib1_scheduler::fill_sib1_grant(cell_slot_resource_allocator& res_grid,
                                     unsigned                      beam_idx,
                                     crb_interval                  sib1_crbs_grant)
{
  const cell_configuration& cfg = res_grid.cfg;

  // Add DCI to list to dl_pdcch.
  res_grid.result.dl.dl_pdcchs.emplace_back();
  auto& sib1_pdcch = res_grid.result.dl.dl_pdcchs.back();

  // Fill PDCCH context info.
  sib1_pdcch.ctx.bwp_cfg = &cfg.dl_cfg_common.init_dl_bwp.generic_params;
  // TODO: verify CORESET CONFIG is actually filled in.
  sib1_pdcch.ctx.coreset_cfg = &cfg.dl_cfg_common.init_dl_bwp.pdcch_common.coreset0;
  sib1_pdcch.ctx.rnti        = SI_RNTI;
  // TODO: import CCEs values from somewhere
  sib1_pdcch.ctx.cces.ncce     = 4;
  sib1_pdcch.ctx.cces.aggr_lvl = aggregation_level::n8;

  // Fill RAR DCI.
  // TODO: compute DCI all these values for actual allocation.
  sib1_pdcch.dci.format_type                = dci_dl_format::f1_0;
  sib1_pdcch.dci.f1_0.freq_domain_assigment = 0;
  sib1_pdcch.dci.f1_0.time_domain_assigment = 0;
  sib1_pdcch.dci.f1_0.mcs                   = 1;
  sib1_pdcch.dci.f1_0.rv                    = 0;

  // Add SIB1 to list of SIB1 information to pass to lower layers.
  res_grid.result.dl.bc.sibs.emplace_back();
  sib_information& sib1 = res_grid.result.dl.bc.sibs.back();
  sib1.pdcch_cfg        = &res_grid.result.dl.dl_pdcchs.back();
  sib1.si_indicator     = sib_information::si_indicator_type::sib1;

  // TODO: add pdsch_configuration.
}
