/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_allocator_impl.h"
#include "../support/pucch/pucch_default_resource.h"

using namespace srsgnb;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pucch_allocator_impl::pucch_allocator_impl(const cell_configuration& cell_cfg_) : cell_cfg(cell_cfg_) {}

pucch_allocator_impl::~pucch_allocator_impl() = default;

const unsigned pucch_allocator_impl::get_pdsch_k0() const
{
  static const unsigned      pdsch_time_res_index = 0;
  const pdsch_config_common& pdsch_cfg            = cell_cfg.dl_cfg_common.init_dl_bwp.pdsch_common;

  return pdsch_cfg.pdsch_td_alloc_list[pdsch_time_res_index].k0;
}

pucch_allocator_impl::pucch_res_alloc_cfg
pucch_allocator_impl::alloc_pucch_common_res_harq(unsigned&                         pucch_res_indicator,
                                                  cell_slot_resource_allocator&     pucch_alloc,
                                                  const dci_dl_context_information& dci_info)
{
  // This is the max value of \f$\Delta_{PRI}\f$, which is a 3-bit unsigned.
  const unsigned max_d_pri = 7;

  // Get the parameter N_bwp_size, which is the Initial UL BWP size in PRBs, as per TS 38.213, Section 9.2.1.
  unsigned size_ul_bwp = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();

  // Get PUCCH common resource config from Table 9.2.1-1, TS 38.213.
  pucch_default_resource pucch_res = get_pucch_default_resource(
      cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common, size_ul_bwp);

  // Get N_CCE (nof_coreset_cces) and n_{CCE,0} (start_cce_idx), as per TS 38.213, Section 9.2.1.
  unsigned nof_coreset_cces = dci_info.coreset_cfg->get_nof_cces();
  unsigned start_cce_idx    = dci_info.cces.ncce;

  // As per TS 38.211, Section 6.3.2.1, the first floor(N_symb_PUCCH/2) are for the first hop, the remaining ones for
  // the second hop.
  ofdm_symbol_range first_hop_symbols{pucch_res.first_symbol_index,
                                      pucch_res.first_symbol_index + pucch_res.nof_symbols / 2};
  ofdm_symbol_range second_hop_symbols{pucch_res.first_symbol_index + pucch_res.nof_symbols / 2,
                                       pucch_res.first_symbol_index + pucch_res.nof_symbols};

  const bwp_configuration& init_ul_bwp_param = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

  // Find a value of \Delta_PRI such that the PUCCH resources are not used.
  for (unsigned d_pri = 0; d_pri != max_d_pri; ++d_pri) {
    // r_PUCCH, as per Section 9.2.1, TS 38.213.
    unsigned r_pucch = get_pucch_default_resource_index(start_cce_idx, nof_coreset_cces, d_pri);

    // Compute PRB_first_hop and PRB_second_hop as per Section 9.2.1, TS 38.213.
    auto prbs = get_pucch_default_prb_index(r_pucch, pucch_res.rb_bwp_offset, pucch_res.cs_indexes.size(), size_ul_bwp);

    // With the default PUCCH resource configs, Format is either 0 or 1, which only occupy 1 RB.
    unsigned   crb_first_hop = prb_to_crb(init_ul_bwp_param, prbs.first);
    grant_info first_hop_grant{
        init_ul_bwp_param.scs, first_hop_symbols, crb_interval{crb_first_hop, crb_first_hop + 1}};
    unsigned   crb_second_hop = prb_to_crb(init_ul_bwp_param, prbs.second);
    grant_info second_hop_grant{
        init_ul_bwp_param.scs, second_hop_symbols, crb_interval{crb_second_hop, crb_second_hop + 1}};

    // Compute CS index as per Section 9.2.1, TS 38.213.
    size_t cs_idx = r_pucch < 8 ? static_cast<size_t>(r_pucch) % pucch_res.cs_indexes.size()
                                : static_cast<size_t>(r_pucch - 8) % pucch_res.cs_indexes.size();
    srsgnb_assert(cs_idx < pucch_res.cs_indexes.size(), "CS index exceeds static vector size");
    uint8_t cyclic_shift = pucch_res.cs_indexes[cs_idx];

    // If both 1st and 2nd hop grants do not collide with any UL grants, then allocate PUCCH in the grid.
    if (not pucch_alloc.ul_res_grid.collides(first_hop_grant) &&
        not pucch_alloc.ul_res_grid.collides(second_hop_grant)) {
      // Set outputs before exiting the function.
      pucch_alloc.ul_res_grid.fill(first_hop_grant);
      pucch_alloc.ul_res_grid.fill(second_hop_grant);
      pucch_res_alloc_cfg ret_pucch_resource{
          .first_hop_res = first_hop_grant, .cs = cyclic_shift, .format = pucch_res.format};
      ret_pucch_resource.second_hop_res.emplace();
      ret_pucch_resource.second_hop_res.value() = second_hop_grant;
      pucch_res_indicator                       = d_pri;
      return ret_pucch_resource;
    }
  }

  return pucch_res_alloc_cfg{.has_config = false};
}

static bool has_ded_pucch_resource_cfg(const ue_cell_configuration& ue_cell_cfg)
{
  // Check if there is any PUCCH res config in the UE configuration.
  if (ue_cell_cfg.ul_config.has_value() and ue_cell_cfg.ul_config.value().init_ul_bwp.pucch_cnf.has_value()) {
    const auto& pucch_cfg = ue_cell_cfg.ul_config.value().init_ul_bwp.pucch_cnf.value();
    // NOTE: We assume that the PUCCH resources for HARQ-ACK are in PUCCH Resource Set 0, only.
    // TODO: change this so as to extend the check to other PUCCH Resource Sets.
    if (not pucch_cfg.pucch_res_set_0.pucch_res_id_list.empty()) {
      return true;
    }
  }
  return false;
}

void pucch_allocator_impl::fill_pucch_res_output(pucch_info&                  pucch_info,
                                                 rnti_t                       rnti,
                                                 pucch_res_alloc_cfg          pucch_res,
                                                 const ue_cell_configuration& ue_cell_cfg)
{
  pucch_info.crnti  = rnti;
  pucch_info.format = pucch_res.format;
  if (has_ded_pucch_resource_cfg(ue_cell_cfg)) {
    srsgnb_assert(false, "PUCCH scheduler for dedicated resources not yet implemented");
  } else {
    pucch_info.bwp_cfg                       = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
    pucch_info.resources.intra_slot_freq_hop = true;
    pucch_info.resources.prbs                = crb_to_prb(*pucch_info.bwp_cfg, pucch_res.first_hop_res.crbs);
    srsgnb_assert(pucch_res.second_hop_res.has_value(), "Missing configuration for PUCCH resource configuration");

    pucch_info.resources.second_hop_prbs = crb_to_prb(*pucch_info.bwp_cfg, pucch_res.second_hop_res.value().crbs);
    pucch_info.resources.symbols =
        ofdm_symbol_range{pucch_res.first_hop_res.symbols.start(), pucch_res.second_hop_res.value().symbols.stop()};
  }

  switch (pucch_res.format) {
    case pucch_format::FORMAT_0: {
      pucch_info.format_0.group_hopping = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      pucch_info.format_0.n_id_hopping  = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                              ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                              : static_cast<unsigned>(cell_cfg.pci);
      /// \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
      pucch_info.format_0.initial_cyclic_shift = pucch_res.cs;
      // SR cannot be reported using common PUCCH resources.
      pucch_info.format_0.sr_bits = sr_nof_bits::no_sr;
      // For the default PUCCH resources, we assume only 1 HARQ-ACK process needs to be reported.
      pucch_info.format_0.harq_ack_nof_bits = 1;
      break;
    }
    case pucch_format::FORMAT_1: {
      pucch_info.format_1.group_hopping = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      pucch_info.format_1.n_id_hopping  = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                              ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                              : static_cast<unsigned>(cell_cfg.pci);
      pucch_info.format_1.initial_cyclic_shift = pucch_res.cs;
      // SR cannot be reported using common PUCCH resources.
      pucch_info.format_1.sr_bits           = sr_nof_bits::no_sr;
      pucch_info.format_1.harq_ack_nof_bits = 1;
      // This option can be configured with Dedicated PUCCH resources.
      pucch_info.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
      /// As per TS 38.213, Section 9.2.1, OCC with index 0 is used for PUCCH resources in Table 9.2.1-1.
      pucch_info.format_1.time_domain_occ = 0;
      break;
    }
    default:
      srsgnb_assert(false, "PUCCH Format must from 0 to 4, but only 0 and 1 are currently supported.");
  }
}

pucch_info* pucch_allocator_impl::alloc_pucch_harq_ack_ue(unsigned&                    pucch_res_indicator,
                                                          unsigned&                    harq_feedback_timing_indicator,
                                                          cell_resource_allocator&     slot_alloc,
                                                          const pdcch_dl_information&  dci_info,
                                                          rnti_t                       rnti,
                                                          const ue&                    ue,
                                                          const ue_cell_configuration& ue_cell_cfg)
{
  // Get resource allocator for SLOT k0+k1.
  // TODO: extend scheduler for k1 different from 4 slots.
  const unsigned k1 = 4;
  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = slot_alloc[get_pdsch_k0() + k1];

  // Get the PUCCH resources, either from default tables or from dedicated
  pucch_res_alloc_cfg pucch_res = {.has_config = false};
  if (has_ded_pucch_resource_cfg(ue_cell_cfg)) {
    srsgnb_assert(false, "PUCCH scheduler for dedicated resources not yet implemented");
  } else {
    pucch_res = alloc_pucch_common_res_harq(pucch_res_indicator, pucch_slot_alloc, dci_info.ctx);
  }

  // No resources available for PUCCH.
  if (pucch_res.has_config) {
    return NULL;
  }

  // Fill Slot grid.
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.first_hop_res);
  if (pucch_res.second_hop_res.has_value()) {
    pucch_slot_alloc.ul_res_grid.fill(pucch_res.second_hop_res.value());
  }

  // Fill scheduler output.
  pucch_slot_alloc.result.ul.pucchs.emplace_back();
  pucch_info& pucch_info = pucch_slot_alloc.result.ul.pucchs.back();
  fill_pucch_res_output(pucch_info, rnti, pucch_res, ue_cell_cfg);

  // NOTE: HARQ feedback is encoded for DCI 1_0. As per TS 38.213, Section 9.2.3, the harq_feedback_timing_indicator
  // maps to {1, 2, 3, 4, 5, 6, 7, 8}.
  // TODO: extend the HARQ feedback indicator to DCI format 1_1.
  harq_feedback_timing_indicator = k1 - 1;

  return &pucch_info;
}