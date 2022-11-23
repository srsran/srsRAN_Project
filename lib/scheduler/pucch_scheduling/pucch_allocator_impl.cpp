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

/////////////    RESOURCE MANAGER     /////////////

void pucch_resource_manager::slot_indication(slot_point slot_tx)
{
  // Update Slot.
  last_sl_ind = slot_tx;

  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(last_sl_ind - 1);

  res_counter.sr_resource_available   = true;
  res_counter.next_pucch_harq_res_idx = 0;
  res_counter.rnti_records.clear();
}

pucch_harq_resource_alloc_record
pucch_resource_manager::get_next_harq_res_available(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg)
{
  srsgnb_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PDCCH being allocated to far into the future");

  const auto&                    pucch_res_list = pucch_cfg.pucch_res_list;
  rnti_pucch_res_id_slot_record& res_counter    = get_slot_resource_counter(slot_harq);
  if (res_counter.next_pucch_harq_res_idx < pucch_res_list.size()) {
    res_counter.rnti_records.emplace_back(crnti);
    return pucch_harq_resource_alloc_record{.pucch_res           = &pucch_res_list[res_counter.next_pucch_harq_res_idx],
                                            .pucch_res_indicator = res_counter.next_pucch_harq_res_idx++};
  } else {
    return pucch_harq_resource_alloc_record{.pucch_res = nullptr};
  }
};

const pucch_resource* pucch_resource_manager::get_next_sr_res_available(slot_point          slot_sr,
                                                                        const pucch_config& pucch_cfg)
{
  srsgnb_sanity_check(slot_sr < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PDCCH being allocated to far into the future");
  srsgnb_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");

  if (get_slot_resource_counter(slot_sr).sr_resource_available) {
    const auto& pucch_res_list = pucch_cfg.pucch_res_list;

    // Check if the list of PUCCH resources (corresponding to \c resourceToAddModList, as part of \c PUCCH-Config, as
    // per TS 38.331) contains the resource indexed to be used for SR.
    const auto sr_pucch_resource_cfg =
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

    get_slot_resource_counter(slot_sr).sr_resource_available = false;
    return &(*sr_pucch_resource_cfg);
  }
  return nullptr;
};

int pucch_resource_manager::get_pucch_res_indicator(slot_point slot_tx, rnti_t crnti)
{
  auto& ue_recs = get_slot_resource_counter(slot_tx).rnti_records;

  for (size_t res_idx = 0; res_idx != ue_recs.size(); res_idx++) {
    if (crnti == ue_recs[res_idx]) {
      return static_cast<int>(res_idx);
    }
  }
  // -1 indicates that the there is no UE record for given RNTI.
  return -1;
}

pucch_resource_manager::rnti_pucch_res_id_slot_record& pucch_resource_manager::get_slot_resource_counter(slot_point sl)
{
  srsgnb_sanity_check(sl < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH resource ring-buffer accessed too far into the future");
  return resource_slots[sl.to_uint() % RES_MANAGER_RING_BUFFER_SIZE];
}

/////////////     PUCCH ALLOCATOR     /////////////

pucch_allocator_impl::pucch_allocator_impl(const cell_configuration& cell_cfg_) :
  cell_cfg(cell_cfg_), logger(srslog::fetch_basic_logger("MAC"))
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
      pucch_alloc.ul_res_grid.fill(first_hop_grant);
      pucch_alloc.ul_res_grid.fill(second_hop_grant);
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
    return pucch_harq_ack_output;
  }

  // Fill Slot grid.
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.first_hop_res);
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.second_hop_res);

  // Fill scheduler output.
  pucch_slot_alloc.result.ul.pucchs.emplace_back();
  pucch_info& pucch_info = pucch_slot_alloc.result.ul.pucchs.back();
  fill_pucch_harq_grant(pucch_info, tcrnti, pucch_res);
  pucch_harq_ack_output.pucch_pdu = &pucch_info;

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
                                                          const pucch_resource&         pucch_res)
{
  // NOTE: We do not check for collision in the grid, as it is assumed the PUCCH gets allocated in its reserved
  // resources.
  const bwp_configuration& bwp_config = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

  // Differentiate intra-slot frequency hopping cases.
  if (not pucch_res.intraslot_freq_hopping) {
    // No intra-slot frequency hopping.
    ofdm_symbol_range symbols{pucch_res.format_1.starting_sym_idx,
                              pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols};
    unsigned          starting_crb = prb_to_crb(bwp_config, pucch_res.starting_prb);
    pucch_slot_alloc.ul_res_grid.fill(
        grant_info{bwp_config.scs, symbols, crb_interval{starting_crb, starting_crb + PUCCH_FORMAT_1_NOF_PRBS}});
  }
  // Intra-slot frequency hopping.
  else {
    ofdm_symbol_range first_hop_symbols{pucch_res.format_1.starting_sym_idx,
                                        pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols / 2};
    unsigned          crb_first_hop = prb_to_crb(bwp_config, pucch_res.starting_prb);
    pucch_slot_alloc.ul_res_grid.fill(grant_info{
        bwp_config.scs, first_hop_symbols, crb_interval{crb_first_hop, crb_first_hop + PUCCH_FORMAT_1_NOF_PRBS}});

    ofdm_symbol_range second_hop_symbols{pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols / 2,
                                         pucch_res.format_1.starting_sym_idx + pucch_res.format_1.nof_symbols};
    unsigned          crb_second_hop = prb_to_crb(bwp_config, pucch_res.second_hop_prb);
    pucch_slot_alloc.ul_res_grid.fill(grant_info{
        bwp_config.scs, second_hop_symbols, crb_interval{crb_second_hop, crb_second_hop + PUCCH_FORMAT_1_NOF_PRBS}});
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
      pucch_slot_alloc.slot, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_sr_res == nullptr) {
    logger.warning("SCHED: SR allocation skipped for RNTI {:#x} due to PUCCH ded. resource not available.", crnti);
    return;
  }

  // Check if there is any existing PUCCH grant for HARQ-ACK already allocated for this UE.
  auto*             pucch_harq_it = get_harq_ack_granted_allocated(crnti, pucch_slot_alloc.result.ul.pucchs);
  const pucch_info* existing_pucch_harq_grant =
      pucch_harq_it != pucch_slot_alloc.result.ul.pucchs.end() ? pucch_harq_it : nullptr;

  if (existing_pucch_harq_grant != nullptr and existing_pucch_harq_grant->format_1.harq_ack_nof_bits > 2) {
    logger.warning("SCHED: SR occasion allocation for RNTI {:#x} skipped. CAUSE: max HARQ-ACK bits reached.", crnti);
    return;
  }

  // Allocate PUCCH SR grant only.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning("SCHED: SR occasion allocation for RNTI {:#x} skipped. CAUSE: no more PUCCH grants available.",
                   crnti);
    return;
  }

  const unsigned HARQ_BITS_WITH_NO_HARQ_REPORTING = 0;
  unsigned       nof_harq_ack_bits                = existing_pucch_harq_grant != nullptr
                                                        ? existing_pucch_harq_grant->format_1.harq_ack_nof_bits
                                                        : HARQ_BITS_WITH_NO_HARQ_REPORTING;

  // NOTE: We do not check for collision in the grid, as it is assumed the PUCCH gets allocated in its reserved
  // resources.
  allocate_pucch_ded_res_on_grid(pucch_slot_alloc, *pucch_sr_res);

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_ded_res_grant(
      pucch_slot_alloc.result.ul.pucchs.back(), crnti, *pucch_sr_res, nof_harq_ack_bits, sr_nof_bits::one);
  logger.debug("SCHED: SR occasion for RNTI {:#x} scheduling completed.", crnti);
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
    logger.warning("SCHED: PUCCH HARQ-ACK grant for RNTI {:#x} not allocated. CAUSE: no more PUCCH grants available.",
                   crnti);
    return pucch_harq_ack_output;
  }

  pucch_harq_resource_alloc_record pucch_harq_res_info = resource_manager.get_next_harq_res_available(
      pucch_slot_alloc.slot, crnti, ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value());
  if (pucch_harq_res_info.pucch_res == nullptr) {
    logger.warning(
        "SCHED: PUCCH HARQ-ACK grant allocation skipped for RNTI {:#x} due to PUCCH ded. resources not available.",
        crnti);
    return pucch_harq_ack_output;
  }

  if (existing_sr_grant != nullptr) {
    srsgnb_sanity_check(existing_sr_grant->format == pucch_format::FORMAT_1, "Only PUCCH format 1 expected for SR.");
    existing_sr_grant->format_1.harq_ack_nof_bits++;
  }

  // NOTE: We do not check for collision in the grid, as it is assumed the PUCCH gets allocated in its reserved
  // resources.
  allocate_pucch_ded_res_on_grid(pucch_slot_alloc, *pucch_harq_res_info.pucch_res);

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_slot_alloc.result.ul.pucchs.emplace_back();
  pucch_info&    pucch_pdu                    = pucch_slot_alloc.result.ul.pucchs.back();
  const unsigned HARQ_BITS_IN_NEW_PUCCH_GRANT = 1;
  fill_pucch_ded_res_grant(
      pucch_pdu, crnti, *pucch_harq_res_info.pucch_res, HARQ_BITS_IN_NEW_PUCCH_GRANT, sr_nof_bits::no_sr);
  logger.debug("SCHED: PUCCH HARQ-ACK grant allocation for RNTI {:#x} completed.", crnti);
  pucch_harq_ack_output.pucch_pdu           = &pucch_pdu;
  pucch_harq_ack_output.pucch_res_indicator = static_cast<unsigned>(pucch_harq_res_info.pucch_res_indicator);

  return pucch_harq_ack_output;
}

pucch_harq_ack_grant pucch_allocator_impl::update_existing_pucch_harq_grant(pucch_info& existing_harq_grant,
                                                                            pucch_info* existing_sr_grant,
                                                                            rnti_t      rnti,
                                                                            slot_point  sl_tx)
{
  pucch_harq_ack_grant output;
  if (existing_harq_grant.format_1.harq_ack_nof_bits > 1) {
    return output;
  }
  int pucch_res_idx = resource_manager.get_pucch_res_indicator(sl_tx, rnti);
  if (pucch_res_idx < 0) {
    srsgnb_assert(false, "This should not happen");
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
  else if (existing_grants.sr_grant != nullptr and existing_grants.harq_grant == nullptr and
           existing_grants.sr_grant->format_1.harq_ack_nof_bits < 2) {
    return allocate_new_pucch_harq_grant(pucch_slot_alloc, crnti, ue_cell_cfg, existing_grants.sr_grant);
  }
  // An HARQ is already scheduled. Update the existing HARQ grant and the SR grant, if present.
  else if (existing_grants.harq_grant != nullptr and existing_grants.harq_grant->format_1.harq_ack_nof_bits < 2) {
    if (existing_grants.sr_grant != nullptr) {
      srsgnb_sanity_check(existing_grants.harq_grant->format_1.harq_ack_nof_bits ==
                              existing_grants.sr_grant->format_1.harq_ack_nof_bits,
                          "Mismatch HARQ bits mismatch between SR and HARQ grants");
    }
    return update_existing_pucch_harq_grant(
        *existing_grants.harq_grant, existing_grants.sr_grant, crnti, pucch_slot_alloc.slot);
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

pucch_uci_bits pucch_allocator_impl::remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc, rnti_t crnti)
{
  pucch_uci_bits removed_uci_info;

  auto& pucchs = slot_alloc.result.ul.pucchs;

  // Remove HARQ-ACK grant first.
  auto it = std::find_if(pucchs.begin(), pucchs.end(), [crnti](pucch_info& pucch) {
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
