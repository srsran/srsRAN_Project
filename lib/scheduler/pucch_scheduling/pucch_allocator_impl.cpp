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
#include "srsgnb/ran/pucch/pucch_info.h"

using namespace srsgnb;

/////////////    RESOURCE MANAGER     /////////////

pucch_resource_manager::pucch_resource_manager()
{
  auto reset_slot_record = [](rnti_pucch_res_id_slot_record& res_counter) {
    res_counter.ue_using_sr_resource = INVALID_RNTI;
    for (auto& ue_rec : res_counter.ues_using_format1_res) {
      ue_rec = INVALID_RNTI;
    }
    for (auto& ue_rec : res_counter.ues_using_format2_res) {
      ue_rec = INVALID_RNTI;
    }
  };

  std::for_each(resource_slots.begin(), resource_slots.end(), reset_slot_record);
}

void pucch_resource_manager::slot_indication(slot_point slot_tx)
{
  // Update Slot.
  last_sl_ind = slot_tx;

  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(last_sl_ind - 1);

  res_counter.ue_using_sr_resource = INVALID_RNTI;
  for (auto& ue_rec : res_counter.ues_using_format1_res) {
    ue_rec = INVALID_RNTI;
  }
  for (auto& ue_rec : res_counter.ues_using_format2_res) {
    ue_rec = INVALID_RNTI;
  }
}

pucch_harq_resource_alloc_record
pucch_resource_manager::get_next_harq_res_available(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg)
{
  srsgnb_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PDCCH being allocated to far into the future");

  // Get resource list of wanted slot.
  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(slot_harq);

  auto available_resource = std::find_if(res_counter.ues_using_format1_res.begin(),
                                         res_counter.ues_using_format1_res.end(),
                                         [](const rnti_t rnti) { return rnti == INVALID_RNTI; });

  const auto& pucch_res_list = pucch_cfg.pucch_res_list;

  const unsigned pucch_resource_set_format1_idx = 0;

  if (available_resource != res_counter.ues_using_format1_res.end() and
      static_cast<unsigned>(available_resource - res_counter.ues_using_format1_res.begin()) <
          pucch_cfg.pucch_res_set[pucch_resource_set_format1_idx].pucch_res_id_list.size()) {
    unsigned pucch_res_indicator =
        static_cast<unsigned>(available_resource - res_counter.ues_using_format1_res.begin());
    *available_resource = crnti;
    unsigned pucch_res_idx_from_list =
        pucch_cfg.pucch_res_set[pucch_resource_set_format1_idx].pucch_res_id_list[pucch_res_indicator];
    return pucch_harq_resource_alloc_record{.pucch_res           = &pucch_res_list[pucch_res_idx_from_list],
                                            .pucch_res_indicator = pucch_res_indicator};
  }
  return pucch_harq_resource_alloc_record{.pucch_res = nullptr};
};

pucch_harq_resource_alloc_record pucch_resource_manager::get_next_format2_res_available(slot_point          slot_harq,
                                                                                        rnti_t              crnti,
                                                                                        const pucch_config& pucch_cfg)
{
  srsgnb_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PDCCH being allocated to far into the future");

  // Get resource list of wanted slot.
  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(slot_harq);

  auto available_resource = std::find_if(res_counter.ues_using_format2_res.begin(),
                                         res_counter.ues_using_format2_res.end(),
                                         [](const rnti_t rnti) { return rnti == INVALID_RNTI; });

  const auto& pucch_res_list = pucch_cfg.pucch_res_list;

  const unsigned pucch_resource_set_format2_idx = 1;

  if (available_resource != res_counter.ues_using_format2_res.end() and
      static_cast<unsigned>(available_resource - res_counter.ues_using_format2_res.begin()) <
          pucch_cfg.pucch_res_set[pucch_resource_set_format2_idx].pucch_res_id_list.size()) {
    unsigned pucch_res_indicator =
        static_cast<unsigned>(available_resource - res_counter.ues_using_format2_res.begin());
    *available_resource = crnti;
    unsigned pucch_res_idx_from_list =
        pucch_cfg.pucch_res_set[pucch_resource_set_format2_idx].pucch_res_id_list[pucch_res_indicator];

    return pucch_harq_resource_alloc_record{.pucch_res           = &pucch_res_list[pucch_res_idx_from_list],
                                            .pucch_res_indicator = pucch_res_indicator};
  }
  return pucch_harq_resource_alloc_record{.pucch_res = nullptr};
};

const pucch_resource*
pucch_resource_manager::get_next_sr_res_available(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg)
{
  srsgnb_sanity_check(slot_sr < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PDCCH being allocated to far into the future");
  srsgnb_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");

  if (get_slot_resource_counter(slot_sr).ue_using_sr_resource == INVALID_RNTI) {
    const auto& pucch_res_list = pucch_cfg.pucch_res_list;

    // Check if the list of PUCCH resources (corresponding to \c resourceToAddModList, as part of \c PUCCH-Config, as
    // per TS 38.331) contains the resource indexed to be used for SR.
    const auto* sr_pucch_resource_cfg =
        std::find_if(pucch_res_list.begin(),
                     pucch_res_list.end(),
                     [sr_res_idx = pucch_cfg.sr_res_list[0].pucch_res_id](const pucch_resource& pucch_sr_res_cfg) {
                       return static_cast<unsigned>(sr_res_idx) == pucch_sr_res_cfg.res_id;
                     });

    // If there is no such PUCCH resource, return \c nullptr.
    if (sr_pucch_resource_cfg == pucch_res_list.end()) {
      // TODO: Add information about the LC which this SR is for.
      return nullptr;
    }

    get_slot_resource_counter(slot_sr).ue_using_sr_resource = crnti;
    return &(*sr_pucch_resource_cfg);
  }
  return nullptr;
};

bool pucch_resource_manager::release_harq_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg)
{
  auto& allocated_ues = get_slot_resource_counter(slot_harq).ues_using_format1_res;
  auto  target_res = std::find_if(allocated_ues.begin(), allocated_ues.end(), [target_rnti = crnti](const rnti_t rnti) {
    return rnti == target_rnti;
  });

  // If the resources was found, then release it (i.e., remove the C-RNTI of the user allocated to it).
  if (target_res != allocated_ues.end()) {
    *target_res = INVALID_RNTI;
    return true;
  }

  return false;
}

bool pucch_resource_manager::release_format2_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg)
{
  auto& allocated_ues = get_slot_resource_counter(slot_harq).ues_using_format2_res;
  auto  target_res = std::find_if(allocated_ues.begin(), allocated_ues.end(), [target_rnti = crnti](const rnti_t rnti) {
    return rnti == target_rnti;
  });

  // If the resources was found, then release it (i.e., remove the C-RNTI of the user allocated to it).
  if (target_res != allocated_ues.end()) {
    *target_res = INVALID_RNTI;
    return true;
  }

  return false;
}

bool pucch_resource_manager::release_sr_resource(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg)
{
  auto& allocated_ue = get_slot_resource_counter(slot_sr).ue_using_sr_resource;

  // If the UE allocated to the SR PUCCH resource matches the given CRNTI, release the resource.
  if (allocated_ue == crnti) {
    allocated_ue = INVALID_RNTI;
    return true;
  }

  return false;
}

int pucch_resource_manager::get_f1_pucch_res_indic(slot_point slot_tx, rnti_t crnti)
{
  const auto& ue_recs = get_slot_resource_counter(slot_tx).ues_using_format1_res;

  auto ue_resource = std::find_if(
      ue_recs.begin(), ue_recs.end(), [target_rnti = crnti](const rnti_t rnti) { return rnti == target_rnti; });

  // -1 indicates that the there is no UE record for given RNTI.
  return ue_resource != ue_recs.end() ? static_cast<int>(ue_resource - ue_recs.begin()) : -1;
}

int pucch_resource_manager::get_f2_pucch_res_indic(slot_point slot_tx, rnti_t crnti)
{
  const auto& ue_recs = get_slot_resource_counter(slot_tx).ues_using_format2_res;

  auto ue_resource = std::find_if(
      ue_recs.begin(), ue_recs.end(), [target_rnti = crnti](const rnti_t rnti) { return rnti == target_rnti; });

  // -1 indicates that the there is no UE record for given RNTI.
  return ue_resource != ue_recs.end() ? static_cast<int>(ue_resource - ue_recs.begin()) : -1;
}

pucch_resource_manager::rnti_pucch_res_id_slot_record& pucch_resource_manager::get_slot_resource_counter(slot_point sl)
{
  srsgnb_sanity_check(sl < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH resource ring-buffer accessed too far into the future");
  return resource_slots[sl.to_uint() % RES_MANAGER_RING_BUFFER_SIZE];
}

/////////////     PUCCH ALLOCATOR     /////////////

pucch_allocator_impl::pucch_allocator_impl(const cell_configuration& cell_cfg_) :
  cell_cfg(cell_cfg_), logger(srslog::fetch_basic_logger("SCHED"))
{
}

pucch_allocator_impl::~pucch_allocator_impl() = default;

pucch_allocator_impl::pucch_res_alloc_cfg
pucch_allocator_impl::alloc_pucch_common_res_harq(unsigned&                      pucch_res_indicator,
                                                  cell_slot_resource_allocator&  pucch_alloc,
                                                  const dci_context_information& dci_info)
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
      pucch_res_alloc_cfg ret_pucch_resource{
          .first_hop_res = first_hop_grant, .cs = cyclic_shift, .format = pucch_res.format};
      ret_pucch_resource.second_hop_res = second_hop_grant;
      ret_pucch_resource.has_config     = true;
      pucch_res_indicator               = d_pri;
      return ret_pucch_resource;
    }
  }

  return pucch_res_alloc_cfg{};
}

void pucch_allocator_impl::fill_pucch_harq_grant(pucch_info&                pucch_info,
                                                 rnti_t                     rnti,
                                                 const pucch_res_alloc_cfg& pucch_res)
{
  pucch_info.crnti                     = rnti;
  pucch_info.format                    = pucch_res.format;
  pucch_info.bwp_cfg                   = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_info.resources.prbs            = crb_to_prb(*pucch_info.bwp_cfg, pucch_res.first_hop_res.crbs);
  pucch_info.resources.second_hop_prbs = crb_to_prb(*pucch_info.bwp_cfg, pucch_res.second_hop_res.crbs);
  pucch_info.resources.symbols =
      ofdm_symbol_range{pucch_res.first_hop_res.symbols.start(), pucch_res.second_hop_res.symbols.stop()};

  switch (pucch_res.format) {
    case pucch_format::FORMAT_0: {
      pucch_info.format_0.group_hopping = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      pucch_info.format_0.n_id_hopping  = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
                                              ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
                                              : static_cast<unsigned>(cell_cfg.pci);
      // \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
      pucch_info.format_0.initial_cyclic_shift = pucch_res.cs;
      // SR cannot be reported using common PUCCH resources.
      pucch_info.format_0.sr_bits = sr_nof_bits::no_sr;
      // [Implementation-defined] For the default PUCCH resources, we assume only 1 HARQ-ACK process needs to be
      // reported.
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
      pucch_info.format_1.sr_bits = sr_nof_bits::no_sr;
      // [Implementation-defined] For the default PUCCH resources, we assume only 1 HARQ-ACK process needs to be
      // reported.
      pucch_info.format_1.harq_ack_nof_bits = 1;
      // This option can be configured with Dedicated PUCCH resources.
      pucch_info.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
      // As per TS 38.213, Section 9.2.1, OCC with index 0 is used for PUCCH resources in Table 9.2.1-1.
      pucch_info.format_1.time_domain_occ = 0;
      break;
    }
    default:
      srsgnb_assert(false, "PUCCH Format must from 0 to 4, but only 0 and 1 are currently supported.");
  }
}

pucch_harq_ack_grant pucch_allocator_impl::alloc_common_pucch_harq_ack_ue(cell_resource_allocator& slot_alloc,
                                                                          rnti_t                   tcrnti,
                                                                          unsigned pdsch_time_domain_resource,
                                                                          unsigned k1,
                                                                          const pdcch_dl_information& dci_info)
{
  // PUCCH output.
  pucch_harq_ack_grant pucch_harq_ack_output{};

  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = slot_alloc[pdsch_time_domain_resource + k1];

  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    return pucch_harq_ack_output;
  }

  // Get the PUCCH resources, either from default tables.
  pucch_res_alloc_cfg pucch_res;
  pucch_res = alloc_pucch_common_res_harq(pucch_harq_ack_output.pucch_res_indicator, pucch_slot_alloc, dci_info.ctx);

  // No resources available for PUCCH.
  if (not pucch_res.has_config) {
    logger.debug("PUCCH for TC-RNTI={:#x} not allocated due to resources not available", tcrnti);
    return pucch_harq_ack_output;
  }

  // Fill Slot grid.
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.first_hop_res);
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.second_hop_res);

  // Fill scheduler output.
  pucch_info& pucch_info = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_harq_grant(pucch_info, tcrnti, pucch_res);
  pucch_harq_ack_output.pucch_pdu = &pucch_info;

  logger.debug("PUCCH for TC-RNTI={:#x} allocated for slot={}.", tcrnti, pucch_slot_alloc.slot.to_uint());

  return pucch_harq_ack_output;
}

static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>::iterator
get_harq_ack_granted_allocated(rnti_t crnti, static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs)
{
  // Return the PUCCH grant allocated for HARQ-ACk, if any.
  return std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
    return pucch.crnti == crnti and
           ((pucch.format == pucch_format::FORMAT_0 and pucch.format_0.harq_ack_nof_bits > 0) or
            (pucch.format == pucch_format::FORMAT_1 and pucch.format_1.harq_ack_nof_bits > 0));
  });
}

void pucch_allocator_impl::fill_pucch_ded_res_grant(pucch_info&           pucch_sr_grant,
                                                    rnti_t                crnti,
                                                    const pucch_resource& pucch_ded_res_cfg,
                                                    unsigned              harq_ack_bits,
                                                    sr_nof_bits           sr_bits)
{
  pucch_sr_grant.crnti   = crnti;
  pucch_sr_grant.bwp_cfg = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_sr_grant.format  = pucch_ded_res_cfg.format;

  switch (pucch_ded_res_cfg.format) {
    case pucch_format::FORMAT_1: {
      // Set PRBs and symbols, first.º
      // The number of PRBs is not explicitly stated in the TS, but it can be inferred it's 1.
      pucch_sr_grant.resources.prbs.set(pucch_ded_res_cfg.starting_prb,
                                        pucch_ded_res_cfg.starting_prb + PUCCH_FORMAT_1_NOF_PRBS);
      pucch_sr_grant.resources.symbols.set(pucch_ded_res_cfg.format_1.starting_sym_idx,
                                           pucch_ded_res_cfg.format_1.starting_sym_idx +
                                               pucch_ded_res_cfg.format_1.nof_symbols);
      if (pucch_ded_res_cfg.intraslot_freq_hopping) {
        pucch_sr_grant.resources.second_hop_prbs.set(pucch_ded_res_cfg.second_hop_prb,
                                                     pucch_ded_res_cfg.second_hop_prb + PUCCH_FORMAT_1_NOF_PRBS);
      }
      // \c pucch-GroupHopping and \c hoppingId are set as per TS 38.211, Section 6.3.2.2.1.
      pucch_sr_grant.format_1.group_hopping = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      pucch_sr_grant.format_1.n_id_hopping =
          cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
              ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
              : cell_cfg.pci;
      pucch_sr_grant.format_1.initial_cyclic_shift = pucch_ded_res_cfg.format_1.initial_cyclic_shift;
      pucch_sr_grant.format_1.time_domain_occ      = pucch_ded_res_cfg.format_1.time_domain_occ;
      // For PUCCH Format 1, only 1 SR bit.
      pucch_sr_grant.format_1.sr_bits           = sr_bits;
      pucch_sr_grant.format_1.harq_ack_nof_bits = harq_ack_bits;
      // [Implementation-defined] We do not implement PUCCH over several slots.
      pucch_sr_grant.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
    }
    default:
      return;
  }
}

void pucch_allocator_impl::allocate_pucch_ded_res_on_grid(cell_slot_resource_allocator& pucch_slot_alloc,
                                                          const pucch_resource&         pucch_res,
                                                          unsigned                      nof_prbs)
{
  srsgnb_assert(pucch_res.format == pucch_format::FORMAT_1 or pucch_res.format == pucch_format::FORMAT_2,
                "Only PUCCH Format 1 and 2 are currently supported");

  // NOTE: We do not check for collision in the grid, as it is assumed the PUCCH gets allocated in its reserved
  // resources.
  const bwp_configuration& bwp_config = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

  // Differentiate intra-slot frequency hopping cases.
  if (not pucch_res.intraslot_freq_hopping) {
    unsigned starting_sym = pucch_res.format == pucch_format::FORMAT_1 ? pucch_res.format_1.starting_sym_idx
                                                                       : pucch_res.format_2.starting_sym_idx;
    unsigned stop_sym     = pucch_res.format == pucch_format::FORMAT_1 ? starting_sym + pucch_res.format_1.nof_symbols
                                                                       : starting_sym + pucch_res.format_2.nof_symbols;

    // No intra-slot frequency hopping.
    ofdm_symbol_range symbols{starting_sym, stop_sym};
    unsigned          starting_crb = prb_to_crb(bwp_config, pucch_res.starting_prb);
    pucch_slot_alloc.ul_res_grid.fill(
        grant_info{bwp_config.scs, symbols, crb_interval{starting_crb, starting_crb + nof_prbs}});
  }
  // Intra-slot frequency hopping.
  else {
    // First hop.
    unsigned starting_sym = pucch_res.format == pucch_format::FORMAT_1 ? pucch_res.format_1.starting_sym_idx
                                                                       : pucch_res.format_2.starting_sym_idx;
    unsigned stop_sym = pucch_res.format == pucch_format::FORMAT_1 ? starting_sym + pucch_res.format_1.nof_symbols / 2
                                                                   : starting_sym + pucch_res.format_2.nof_symbols / 2;
    ofdm_symbol_range first_hop_symbols{starting_sym, stop_sym};
    unsigned          crb_first_hop = prb_to_crb(bwp_config, pucch_res.starting_prb);
    pucch_slot_alloc.ul_res_grid.fill(
        grant_info{bwp_config.scs, first_hop_symbols, crb_interval{crb_first_hop, crb_first_hop + nof_prbs}});

    // Second hop.
    unsigned second_starting_sym = stop_sym;
    unsigned second_stop_sym     = pucch_res.format == pucch_format::FORMAT_1
                                       ? starting_sym + pucch_res.format_1.nof_symbols
                                       : starting_sym + pucch_res.format_2.nof_symbols;

    ofdm_symbol_range second_hop_symbols{second_starting_sym, second_stop_sym};
    unsigned          crb_second_hop = prb_to_crb(bwp_config, pucch_res.second_hop_prb);
    pucch_slot_alloc.ul_res_grid.fill(
        grant_info{bwp_config.scs, second_hop_symbols, crb_interval{crb_second_hop, crb_second_hop + nof_prbs}});
  }
}

void pucch_allocator_impl::pucch_allocate_sr_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                                         rnti_t                        crnti,
                                                         const ue_cell_configuration&  ue_cell_cfg)
{
  // Get the index of the PUCCH resource to be used for SR.
  // NOTEs: (i) This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  //        (ii) get_next_sr_res_available() should be a function of sr_res; however, to simplify the
  //        implementation, as assume sr_resource_cfg_list only has 1 element.
  // TODO: extend sr_resource_cfg_list to multiple resource and get_next_sr_res_available() so that it becomes a
  //       func of sr_res.
  const pucch_resource* pucch_sr_res = resource_manager.get_next_sr_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_sr_res == nullptr) {
    logger.warning("SR allocation skipped for RNTI {:#x} for slot={} due to PUCCH ded. resource not available.",
                   crnti,
                   pucch_slot_alloc.slot.to_uint());
    return;
  }

  // Check if there is any existing PUCCH grant for HARQ-ACK already allocated for this UE.
  auto*             pucch_harq_it = get_harq_ack_granted_allocated(crnti, pucch_slot_alloc.result.ul.pucchs);
  const pucch_info* existing_pucch_harq_grant =
      pucch_harq_it != pucch_slot_alloc.result.ul.pucchs.end() ? pucch_harq_it : nullptr;

  if (existing_pucch_harq_grant != nullptr and existing_pucch_harq_grant->format_1.harq_ack_nof_bits > 2) {
    logger.warning("SR occasion allocation for RNTI {:#x} for slot={} skipped. CAUSE: max HARQ-ACK bits reached.",
                   crnti,
                   pucch_slot_alloc.slot.to_uint());
    return;
  }

  // Allocate PUCCH SR grant only.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning("SR occasion allocation for RNTI {:#x} for slot={} skipped. CAUSE: no more PUCCH grants available.",
                   crnti,
                   pucch_slot_alloc.slot.to_uint());
    return;
  }

  const unsigned HARQ_BITS_WITH_NO_HARQ_REPORTING = 0;
  unsigned       nof_harq_ack_bits                = existing_pucch_harq_grant != nullptr
                                                        ? existing_pucch_harq_grant->format_1.harq_ack_nof_bits
                                                        : HARQ_BITS_WITH_NO_HARQ_REPORTING;

  // NOTE: We do not check for collision in the grid, as it is assumed the PUCCH gets allocated in its reserved
  // resources.
  allocate_pucch_ded_res_on_grid(pucch_slot_alloc, *pucch_sr_res, PUCCH_FORMAT_1_NOF_PRBS);

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  fill_pucch_ded_res_grant(
      pucch_slot_alloc.result.ul.pucchs.emplace_back(), crnti, *pucch_sr_res, nof_harq_ack_bits, sr_nof_bits::one);
  logger.debug("SR occasion for RNTI {:#x} for slot={} scheduling completed.", crnti, pucch_slot_alloc.slot.to_uint());
}

struct existing_pucch_grants {
  pucch_info* sr_grant{nullptr};
  pucch_info* harq_grant{nullptr};
};

// Retrieve the existing PUCCH grants for the current RNTI.
existing_pucch_grants get_existing_pucch_grants(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs, rnti_t rnti)
{
  existing_pucch_grants grants;
  for (auto& pucch : pucchs) {
    if (pucch.crnti == rnti) {
      if (pucch.format_1.sr_bits == sr_nof_bits::one) {
        grants.sr_grant = &pucch;
      } else if (pucch.format_1.harq_ack_nof_bits > 0 and pucch.format_1.sr_bits == sr_nof_bits::no_sr) {
        grants.harq_grant = &pucch;
      }
    }
  }

  return grants;
}

pucch_harq_ack_grant pucch_allocator_impl::allocate_new_pucch_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                                         rnti_t                        crnti,
                                                                         const ue_cell_configuration&  ue_cell_cfg,
                                                                         pucch_info* existing_sr_grant)
{
  pucch_harq_ack_grant pucch_harq_ack_output;
  // Check if there is space for one more additional PUCCH HARQ grant.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning(
        "PUCCH HARQ-ACK grant for RNTI {:#x} for slot={} not allocated. CAUSE: no more PUCCH grants available.",
        crnti,
        pucch_slot_alloc.slot.to_uint());
    return pucch_harq_ack_output;
  }

  pucch_harq_resource_alloc_record pucch_harq_res_info = resource_manager.get_next_harq_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_harq_res_info.pucch_res == nullptr) {
    logger.warning("PUCCH HARQ-ACK allocation for RNTI {:#x} for slot={} skipped due to PUCCH ded. resources "
                   "not available.",
                   crnti,
                   pucch_slot_alloc.slot.to_uint());
    return pucch_harq_ack_output;
  }

  if (existing_sr_grant != nullptr) {
    srsgnb_sanity_check(existing_sr_grant->format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for SR.");
    existing_sr_grant->format_1.harq_ack_nof_bits++;
  }

  // NOTE: We do not check for collision in the grid, as it is assumed the PUCCH gets allocated in its reserved
  // resources.
  allocate_pucch_ded_res_on_grid(pucch_slot_alloc, *pucch_harq_res_info.pucch_res, PUCCH_FORMAT_1_NOF_PRBS);

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_info&    pucch_pdu                    = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  const unsigned HARQ_BITS_IN_NEW_PUCCH_GRANT = 1;
  fill_pucch_ded_res_grant(
      pucch_pdu, crnti, *pucch_harq_res_info.pucch_res, HARQ_BITS_IN_NEW_PUCCH_GRANT, sr_nof_bits::no_sr);
  logger.debug(
      "PUCCH HARQ-ACK allocation for RNTI {:#x} for slot={} completed.", crnti, pucch_slot_alloc.slot.to_uint());
  pucch_harq_ack_output.pucch_pdu           = &pucch_pdu;
  pucch_harq_ack_output.pucch_res_indicator = static_cast<unsigned>(pucch_harq_res_info.pucch_res_indicator);

  return pucch_harq_ack_output;
}

pucch_harq_ack_grant pucch_allocator_impl::convert_to_format2(cell_slot_resource_allocator& pucch_slot_alloc,
                                                              pucch_info&                   existing_harq_grant,
                                                              pucch_info*                   existing_sr_grant,
                                                              rnti_t                        rnti,
                                                              slot_point                    sl_tx,
                                                              const ue_cell_configuration&  ue_cell_cfg)
{
  pucch_harq_ack_grant output;

  // Try to obtain a PUCCH format 2 resource, if any available.
  pucch_harq_resource_alloc_record format2_res = resource_manager.get_next_format2_res_available(
      sl_tx, rnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());

  // If no PUCCH format 2 resource available, exit without changing the existing allocations.
  if (format2_res.pucch_res == nullptr) {
    return output;
  }

  // Check if the number of PRBs is sufficient for the number of bits to be acked.
  unsigned sr_bits             = existing_sr_grant != nullptr ? 1 : 0;
  unsigned harq_ack_bits       = existing_harq_grant.format_1.harq_ack_nof_bits + 1;
  unsigned uci_bits            = harq_ack_bits + sr_bits;
  unsigned nof_symbols         = format2_res.pucch_res->format_2.nof_symbols;
  float    max_pucch_code_rate = to_max_code_rate_float(ue_cell_cfg.cfg_dedicated()
                                                         .ul_config.value()
                                                         .init_ul_bwp.pucch_cfg.value()
                                                         .format_2_common_param.value()
                                                         .max_c_rate);
  unsigned nof_prbs =
      get_pucch_format2_nof_prbs(uci_bits, format2_res.pucch_res->format_2.nof_prbs, nof_symbols, max_pucch_code_rate);

  // The number of REs available are not sufficient to accommodate harq_bits_to_ack with exceeding the max code rate;
  // exit without allocating anything.
  if (pucch_format2_code_rate(nof_prbs, nof_symbols, uci_bits) > max_pucch_code_rate) {
    return output;
  }

  // Remove the previously allocated PUCCH format 1 resources.
  remove_pucch_format1_from_grants(
      pucch_slot_alloc, rnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());

  // Allocate PUCCH SR grant only.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.error("List still full after removing PUCCH f1 grant for RNTI {:#x} for slot={}.",
                 rnti,
                 pucch_slot_alloc.slot.to_uint());
    return output;
  }

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_info& pucch_pdu = pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_ded_res_grant(pucch_pdu, rnti, *format2_res.pucch_res, harq_ack_bits, static_cast<sr_nof_bits>(sr_bits));
  logger.debug("PUCCH Format 2 grant (HARQ-ACK) allocation for RNTI {:#x} for slot={} completed.",
               rnti,
               pucch_slot_alloc.slot.to_uint());
  output.pucch_pdu           = &pucch_pdu;
  output.pucch_res_indicator = static_cast<unsigned>(format2_res.pucch_res_indicator);

  return output;
}

pucch_harq_ack_grant
pucch_allocator_impl::update_existing_pucch_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                       pucch_info&                   existing_harq_grant,
                                                       pucch_info*                   existing_sr_grant,
                                                       rnti_t                        rnti,
                                                       slot_point                    sl_tx,
                                                       const ue_cell_configuration&  ue_cell_cfg)
{
  // If the existing grant is for at lest 2 HARQ-ACK bits, then the PUCCH needs to be converted to format 2.
  if (existing_harq_grant.format_1.harq_ack_nof_bits > 1) {
    return convert_to_format2(pucch_slot_alloc, existing_harq_grant, existing_sr_grant, rnti, sl_tx, ue_cell_cfg);
  }

  pucch_harq_ack_grant output;
  int                  pucch_res_idx = resource_manager.get_f1_pucch_res_indic(sl_tx, rnti);
  if (pucch_res_idx < 0) {
    srsgnb_assert(pucch_res_idx >= 0, "PUCCH resource index should not be negative.");
    return output;
  }
  srsgnb_sanity_check(existing_harq_grant.format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for HARQ.");
  // Update the SR, if present.
  if (existing_sr_grant != nullptr) {
    srsgnb_sanity_check(existing_sr_grant->format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for SR.");
    existing_sr_grant->format_1.harq_ack_nof_bits++;
  }
  // Update the HARQ, if present.
  existing_harq_grant.format_1.harq_ack_nof_bits++;
  output.pucch_pdu           = &existing_harq_grant;
  output.pucch_res_indicator = pucch_res_idx;

  logger.debug("UE={:#x}'s HARQ-ACK mltplxd on existing PUCCH for slot={}", rnti, sl_tx.to_uint());
  return output;
}

pucch_harq_ack_grant pucch_allocator_impl::alloc_ded_pucch_harq_ack_ue(cell_resource_allocator&     res_alloc,
                                                                       rnti_t                       crnti,
                                                                       const ue_cell_configuration& ue_cell_cfg,
                                                                       unsigned pdsch_time_domain_resource,
                                                                       unsigned k1)
{
  // TS 38.213, Section 9.2.3, explains the UE's procedure to multiplex HARQ-ACK reporting of multiple slot and for
  // different cells.
  // "The PUCCH resource determination is based on a PUCCH resource indicator field [5, TS 38.212] in a last DCI format
  // 1_0 or DCI format 1_1, among the DCI formats 1_0 or DCI formats 1_1 that have a value of a PDSCH-to-HARQ_feedback
  // timing indicator field indicating a same slot for the PUCCH transmission, that the UE detects and for which the UE
  // transmits corresponding HARQ-ACK information in the PUCCH where, for PUCCH resource determination, detected DCI
  // formats are first indexed in an ascending order across serving cells indexes for a same PDCCH monitoring occasion
  // and are then indexed in an ascending order across PDCCH monitoring occasion indexes".
  // As a result of this, and depending on whether there is any scheduled SRs, the PUCCH allocator can either allocate
  // a new PUCCH grant or update an existing one by changing the number of HARQ-ACK bits to be reported.

  // NOTE: This function does not check whether there are PUSCH grants allocated for the same UE. The check needs to
  // be performed by the caller.

  // PUCCH output.
  pucch_harq_ack_grant pucch_harq_ack_output{};

  // Get the slot allocation grid considering the PDSCH delay (k0) and the PUCCH delay wrt PDSCH (k1).
  cell_slot_resource_allocator& pucch_slot_alloc = res_alloc[pdsch_time_domain_resource + k1];

  auto& pucchs = pucch_slot_alloc.result.ul.pucchs;

  // Retrieve the existing PUCCH grants.
  existing_pucch_grants existing_grants = get_existing_pucch_grants(pucchs, crnti);

  // No existing PUCCH grants, allocate a new one.
  if (existing_grants.sr_grant == nullptr and existing_grants.harq_grant == nullptr) {
    return allocate_new_pucch_harq_grant(pucch_slot_alloc, crnti, ue_cell_cfg, nullptr);
  }
  // An SR is already scheduled but no HARQ grants allocated. Update the SR grants and allocate a new HARQ grant.
  if (existing_grants.sr_grant != nullptr and existing_grants.harq_grant == nullptr and
      existing_grants.sr_grant->format_1.harq_ack_nof_bits < 2) {
    return allocate_new_pucch_harq_grant(pucch_slot_alloc, crnti, ue_cell_cfg, existing_grants.sr_grant);
  }
  // An HARQ is already scheduled. Update the existing HARQ grant and the SR grant, if present.
  if (existing_grants.harq_grant != nullptr and existing_grants.harq_grant->format_1.harq_ack_nof_bits < 2) {
    if (existing_grants.sr_grant != nullptr) {
      srsgnb_sanity_check(existing_grants.harq_grant->format_1.harq_ack_nof_bits ==
                              existing_grants.sr_grant->format_1.harq_ack_nof_bits,
                          "Mismatch HARQ bits mismatch between SR and HARQ grants");
    }
    return update_existing_pucch_harq_grant(pucch_slot_alloc,
                                            *existing_grants.harq_grant,
                                            existing_grants.sr_grant,
                                            crnti,
                                            pucch_slot_alloc.slot,
                                            ue_cell_cfg);
  }

  return pucch_harq_ack_output;
}

static unsigned get_harq_ack_bits(const pucch_info& pucch)
{
  switch (pucch.format) {
    case srsgnb::pucch_format::FORMAT_1:
      return pucch.format_1.harq_ack_nof_bits;
      // TODO: add remaining Formats.
    default:
      srsgnb_assert(pucch.format == srsgnb::pucch_format::FORMAT_1, "Only PUCCH Format 1 supported.");
      return 0;
  }
}

void pucch_allocator_impl::remove_pucch_format1_from_grants(cell_slot_resource_allocator& slot_alloc,
                                                            rnti_t                        crnti,
                                                            const pucch_config&           pucch_cfg)
{
  pucch_uci_bits removed_uci_info;

  auto& pucchs = slot_alloc.result.ul.pucchs;

  // Remove HARQ-ACK grant first.
  auto* it = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
    return pucch.crnti == crnti and pucch.format_1.sr_bits == sr_nof_bits::no_sr and
           pucch.format_1.harq_ack_nof_bits > 0;
  });

  if (it != pucchs.end()) {
    pucchs.erase(it);
    resource_manager.release_harq_resource(slot_alloc.slot, crnti, pucch_cfg);
  }

  // Remove SR grant, if any.
  it = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
    return pucch.crnti == crnti and pucch.format_1.sr_bits == sr_nof_bits::one;
  });

  if (it != pucchs.end()) {
    pucchs.erase(it);
    resource_manager.release_sr_resource(slot_alloc.slot, crnti, pucch_cfg);
  }
}

pucch_uci_bits pucch_allocator_impl::remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc, rnti_t crnti)
{
  pucch_uci_bits removed_uci_info;

  auto& pucchs = slot_alloc.result.ul.pucchs;

  // Remove HARQ-ACK grant first.
  auto* it = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
    return pucch.crnti == crnti and pucch.format_1.sr_bits == sr_nof_bits::no_sr and
           pucch.format_1.harq_ack_nof_bits > 0;
  });

  if (it != pucchs.end()) {
    removed_uci_info.harq_ack_nof_bits = get_harq_ack_bits(*it);
    pucchs.erase(it);
    // TODO: set the PUCCH resource as not used; this requires a redesign of the PUCCH resource manager.
  }

  // Remove SR grant, if any.
  it = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
    return pucch.crnti == crnti and pucch.format_1.sr_bits == sr_nof_bits::one;
  });

  if (it != pucchs.end()) {
    pucchs.erase(it);
    // TODO: set the PUCCH resource as not used; this requires a redesign of the PUCCH resource manager.
  }

  return removed_uci_info;
}

void pucch_allocator_impl::slot_indication(slot_point sl_tx)
{
  // If last_sl_ind is not valid (not initialized), then the check sl_tx == last_sl_ind + 1 does not matter.
  srsgnb_sanity_check(not last_sl_ind.valid() or sl_tx == last_sl_ind + 1, "Detected a skipped slot");

  // Update Slot.
  last_sl_ind = sl_tx;

  resource_manager.slot_indication(sl_tx);
}
