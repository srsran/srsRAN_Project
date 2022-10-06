/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_scheduler_impl.h"
#include "../support/pucch/pucch_default_resource.h"

using namespace srsgnb;

pucch_resource_manager::pucch_resource_manager(const uplink_config& ul_config)
{
  srsgnb_assert(ul_config.init_ul_bwp.pucch_cfg.value().sr_res_list.size() == 1,
                "SR resouce list should only contain 1 value");

  sr_resource_idx       = ul_config.init_ul_bwp.pucch_cfg.value().sr_res_list[0].pucch_res_id;
  sr_resource_available = true;

  for (const auto& resource : ul_config.init_ul_bwp.pucch_cfg.value().pucch_res_list) {
    // Do not include PUCCH resouce used for SR.
    if (resource.res_id != static_cast<unsigned>(sr_resource_idx)) {
      pucch_harq_resources.emplace_back();
      pucch_harq_resources.back() = resource.res_id;
    }
  }
}

int pucch_resource_manager::get_next_sr_res_available()
{
  if (sr_resource_available) {
    sr_resource_available = false;
    return sr_resource_idx;
  }
  return -1;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pucch_scheduler_impl::pucch_scheduler_impl(const cell_configuration& cell_cfg_,
                                           const uplink_config&      ul_config,
                                           ue_list&                  ues_) :
  cell_cfg(cell_cfg_), resource_manager(ul_config), ues(ues_), logger(srslog::fetch_basic_logger("MAC"))
{
}

pucch_scheduler_impl::~pucch_scheduler_impl() = default;

pucch_scheduler_impl::pucch_res_alloc_cfg
pucch_scheduler_impl::alloc_pucch_common_res_harq(unsigned&                         pucch_res_indicator,
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

void pucch_scheduler_impl::fill_pucch_res_output(pucch_info& pucch_info, rnti_t rnti, pucch_res_alloc_cfg pucch_res)
{
  pucch_info.crnti                         = rnti;
  pucch_info.format                        = pucch_res.format;
  pucch_info.bwp_cfg                       = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_info.resources.intra_slot_freq_hop = true;
  pucch_info.resources.prbs                = crb_to_prb(*pucch_info.bwp_cfg, pucch_res.first_hop_res.crbs);
  srsgnb_assert(pucch_res.second_hop_res.has_value(), "Missing configuration for PUCCH resource configuration");

  pucch_info.resources.second_hop_prbs = crb_to_prb(*pucch_info.bwp_cfg, pucch_res.second_hop_res.value().crbs);
  pucch_info.resources.symbols =
      ofdm_symbol_range{pucch_res.first_hop_res.symbols.start(), pucch_res.second_hop_res.value().symbols.stop()};

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

pucch_harq_ack_grant pucch_scheduler_impl::alloc_common_pucch_harq_ack_ue(cell_resource_allocator& slot_alloc,
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

  // Get the PUCCH resources, either from default tables or from dedicated
  pucch_res_alloc_cfg pucch_res;
  pucch_res = alloc_pucch_common_res_harq(pucch_harq_ack_output.pucch_res_indicator, pucch_slot_alloc, dci_info.ctx);

  // No resources available for PUCCH.
  if (pucch_res.has_config) {
    return pucch_harq_ack_output;
  }

  // Fill Slot grid.
  pucch_slot_alloc.ul_res_grid.fill(pucch_res.first_hop_res);
  if (pucch_res.second_hop_res.has_value()) {
    pucch_slot_alloc.ul_res_grid.fill(pucch_res.second_hop_res.value());
  }

  // Fill scheduler output.
  pucch_slot_alloc.result.ul.pucchs.emplace_back();
  pucch_info& pucch_info = pucch_slot_alloc.result.ul.pucchs.back();
  fill_pucch_res_output(pucch_info, tcrnti, pucch_res);
  pucch_harq_ack_output.pucch_pdu = &pucch_info;

  return pucch_harq_ack_output;
}

static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>::iterator
get_harq_ack_granted_allocated(const ue& user, static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs)
{
  // Return the PUCCH grant allocated for HARQ-ACk, if any.
  return std::find_if(pucchs.begin(), pucchs.end(), [&user](pucch_info& pucch) {
    return pucch.crnti == user.crnti and
           ((pucch.format == pucch_format::FORMAT_0 and pucch.format_0.harq_ack_nof_bits > 0) or
            (pucch.format == pucch_format::FORMAT_1 and pucch.format_1.harq_ack_nof_bits > 0));
  });
}

static bool has_pusch_grant_allocated(unsigned crnti, const span<ul_sched_info> puschs)
{
  auto it = std::find_if(
      puschs.begin(), puschs.end(), [crnti](ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });
  return it == puschs.end() ? false : true;
}

void pucch_scheduler_impl::fill_pucch_sr_grant(pucch_info&           pucch_sr_grant,
                                               rnti_t                crnti,
                                               const pucch_resource& pucch_sr_res,
                                               const pucch_config&   pucch_cfg,
                                               unsigned              harq_ack_bits)
{
  pucch_sr_grant.crnti   = crnti;
  pucch_sr_grant.bwp_cfg = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pucch_sr_grant.format  = pucch_sr_res.format;

  switch (pucch_sr_res.format) {
    case pucch_format::FORMAT_1: {
      // Set PRBs and symbols, first.
      // The number of PRBs is not explicitly stated in the TS, but it can be inferred it's 1.
      pucch_sr_grant.resources.prbs.set(pucch_sr_res.starting_prb, pucch_sr_res.starting_prb + PUCCH_FORMAT_1_NOF_PRBS);
      pucch_sr_grant.resources.symbols.set(pucch_sr_res.format_1.starting_sym_idx,
                                           pucch_sr_res.format_1.starting_sym_idx + pucch_sr_res.format_1.nof_symbols);
      pucch_sr_grant.resources.intra_slot_freq_hop = pucch_sr_res.intraslot_freq_hopping;
      if (pucch_sr_res.intraslot_freq_hopping) {
        pucch_sr_grant.resources.second_hop_prbs.set(pucch_sr_res.second_hop_prb,
                                                     pucch_sr_res.second_hop_prb + PUCCH_FORMAT_1_NOF_PRBS);
      }
      // \c pucch-GroupHopping and \c hoppingId are set as per TS 38.211, Section 6.3.2.2.1.
      pucch_sr_grant.format_1.group_hopping = cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->group_hopping;
      pucch_sr_grant.format_1.n_id_hopping =
          cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.has_value()
              ? cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->hopping_id.value()
              : cell_cfg.pci;
      pucch_sr_grant.format_1.initial_cyclic_shift = pucch_sr_res.format_1.initial_cyclic_shift;
      pucch_sr_grant.format_1.time_domain_occ      = pucch_sr_res.format_1.time_domain_occ;
      // For PUCCH Format 1, only 1 SR bit.
      pucch_sr_grant.format_1.sr_bits           = sr_nof_bits::one;
      pucch_sr_grant.format_1.harq_ack_nof_bits = harq_ack_bits;
      // [Implementation-defined] We do not implement PUCCH over several slots.
      pucch_sr_grant.format_1.slot_repetition = pucch_repetition_tx_slot::no_multi_slot;
    }
    default:
      return;
  }
}

void pucch_scheduler_impl::allocate_pucch_on_grid(cell_slot_resource_allocator& pucch_slot_alloc,
                                                  const pucch_resource&         pucch_sr_res)
{
  // NOTE: We do not check for collision in the grid, as it is assumed the PUCCH gets allocated in its reserved
  // resources.
  const bwp_configuration& bwp_config = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;

  // Differentiate intra-slot frequency hopping cases.
  if (not pucch_sr_res.intraslot_freq_hopping) {
    // No intra-slot frequency hopping.
    ofdm_symbol_range symbols{pucch_sr_res.format_1.starting_sym_idx,
                              pucch_sr_res.format_1.starting_sym_idx + pucch_sr_res.format_1.nof_symbols};
    unsigned          starting_crb = prb_to_crb(bwp_config, pucch_sr_res.starting_prb);
    pucch_slot_alloc.ul_res_grid.fill(
        grant_info{bwp_config.scs, symbols, crb_interval{starting_crb, starting_crb + PUCCH_FORMAT_1_NOF_PRBS}});
  }
  // Intra-slot frequency hopping.
  else {
    ofdm_symbol_range first_hop_symbols{pucch_sr_res.format_1.starting_sym_idx,
                                        pucch_sr_res.format_1.starting_sym_idx + pucch_sr_res.format_1.nof_symbols / 2};
    unsigned          crb_first_hop = prb_to_crb(bwp_config, pucch_sr_res.starting_prb);
    pucch_slot_alloc.ul_res_grid.fill(grant_info{
        bwp_config.scs, first_hop_symbols, crb_interval{crb_first_hop, crb_first_hop + PUCCH_FORMAT_1_NOF_PRBS}});

    ofdm_symbol_range second_hop_symbols{pucch_sr_res.format_1.starting_sym_idx + pucch_sr_res.format_1.nof_symbols / 2,
                                         pucch_sr_res.format_1.starting_sym_idx + pucch_sr_res.format_1.nof_symbols};
    unsigned          crb_second_hop = prb_to_crb(bwp_config, pucch_sr_res.second_hop_prb);
    pucch_slot_alloc.ul_res_grid.fill(grant_info{
        bwp_config.scs, second_hop_symbols, crb_interval{crb_second_hop, crb_second_hop + PUCCH_FORMAT_1_NOF_PRBS}});
  }
}

void pucch_scheduler_impl::pucch_allocate_sr(cell_slot_resource_allocator& pucch_slot_alloc,
                                             rnti_t                        crnti,
                                             const pucch_info*             existing_pucch_harq_grant,
                                             const ue_cell_configuration&  ue_cell_cfg,
                                             const pucch_resource&         pucch_sr_res)
{
  if (existing_pucch_harq_grant != nullptr) {
    srsgnb_assert(existing_pucch_harq_grant->format == pucch_format::FORMAT_1 and
                      pucch_sr_res.format == pucch_format::FORMAT_1,
                  "Only PUCCH Format 1 currently supported");
  }
  srsgnb_assert(ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg->format_1_common_param.has_value(),
                "Parameters for PUCCH Format 1 resource not configured");

  // Allocate PUCCH SR grant only.
  if (pucch_slot_alloc.result.ul.pucchs.full()) {
    logger.warning("SCHED: SR occasion allocation for RNTI {:#x} skipped. CAUSE: no more PUCCH grants available.",
                   crnti);
    return;
  }

  const unsigned HARQ_BITS_WITH_NO_HARQ_REPORTING = 0;
  // [Implementation-defined] We assume only 1 HARQ-ACK process needs to be reported.
  // TODO: extend this to the more general case of >1 HARQ bits.
  const unsigned HARQ_BITS_WITH_HARQ_REPORTING = 1;

  unsigned nof_harq_ack_bits =
      existing_pucch_harq_grant == nullptr ? HARQ_BITS_WITH_NO_HARQ_REPORTING : HARQ_BITS_WITH_HARQ_REPORTING;

  // NOTE: We do not check for collision in the grid, as it is assumed the PUCCH gets allocated in its reserved
  // resources.
  allocate_pucch_on_grid(pucch_slot_alloc, pucch_sr_res);

  // Allocate PUCCH SR grant only, as HARQ-ACK grant has been allocated earlier.
  pucch_slot_alloc.result.ul.pucchs.emplace_back();
  fill_pucch_sr_grant(pucch_slot_alloc.result.ul.pucchs.back(),
                      crnti,
                      pucch_sr_res,
                      ue_cell_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value(),
                      nof_harq_ack_bits);
  logger.info("SCHED: SR occasion for RNTI {:#x} scheduling completed.", crnti);
}

void pucch_scheduler_impl::sched_ue_sr_opportunity(cell_resource_allocator& cell_alloc, ue& user, slot_point sl_tx)
{
  // Check if the slot is UL enabled.
  if (not cell_cfg.is_ul_enabled(sl_tx)) {
    logger.warning("SCHED: SR PUCCH occasion not scheduled as current slot is not UL enabled.");
    return;
  }

  // Check if UE has dedicated resources.
  if (user.nof_cells() == 0) {
    logger.warning("SCHED: pCell not configured for RNTI {:#x}- SR allocation will be skipped.", user.crnti);
    return;
  }
  auto& ue_cell = user.get_pcell();
  if (not ue_cell.cfg().cfg_dedicated().ul_config.has_value() or
      not ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.has_value() or
      ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().pucch_res_list.empty() or
      ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list.empty()) {
    logger.warning("SCHED: PUCCH ded resouces not configured for RNTI {:#x}- SR allocation will be skipped.",
                   user.crnti);
    return;
  }

  const auto& sr_resource_cfg_list =
      ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list;

  const auto& pucch_res_list =
      ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().pucch_res_list;

  for (const auto& sr_res : sr_resource_cfg_list) {
    srsgnb_assert(sr_res.period >= sr_periodicity::sl_1, "Minimum supported SR perdiodicity is 1 slot.");

    // Check if this slot corresponds to an SR opportunity for the UE.
    if ((static_cast<unsigned>(sl_tx.to_uint()) - sr_res.offset) % sr_periodicity_to_slot(sr_res.period) == 0) {
      auto& slot_alloc = cell_alloc[0];
      // If there is a PUSCH allocated for this UE, do not allocate any PUCCH SR grants.
      if (has_pusch_grant_allocated(user.crnti, slot_alloc.result.ul.puschs)) {
        logger.debug("SCHED: SR allocation skipped for RNTI {:#x} due to PUSCH grant allocated.", user.crnti);
        return;
      }

      // Get the index of the PUCCH resource to be used for SR.
      // NOTEs: (i) This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
      //        (ii) get_next_sr_res_available() should be a function of sr_res; however, to simplify the
      //        implementation, as assume sr_resource_cfg_list only has 1 element.
      // TODO: extend sr_resource_cfg_list to multiple resource and get_next_sr_res_available() so that it becomes a
      //       func of sr_res.
      int sr_res_idx = resource_manager.get_next_sr_res_available();
      if (sr_res_idx < 0) {
        logger.warning("SCHED: SR allocation skipped for RNTI {:#x} due to PUCCH resource already in use.", user.crnti);
        continue;
      }

      // Check if the list of PUCCH resources (corresponding to \c resourceToAddModList, as part of \c PUCCH-Config, as
      // per TS 38.331) contains the resource indexed to be used for SR.
      const auto sr_pucch_resource_cfg = std::find_if(
          pucch_res_list.begin(), pucch_res_list.end(), [sr_res_idx](const pucch_resource& pucch_sr_res_cfg) {
            return static_cast<unsigned>(sr_res_idx) == pucch_sr_res_cfg.res_id;
          });

      // If there is no such PUCCH resource, skip to the next SR resource.
      if (sr_pucch_resource_cfg == pucch_res_list.end()) {
        // TODO: Add information about the LC which this SR is for.
        logger.warning("SCHED: SR allocation skipped for RNTI {:#x} due to PUCCH ded. resource not configured.",
                       user.crnti);
        continue;
      }

      // Retrieve any existing PUCCH resource already allocated for HARQ-ACK report.
      auto              pucch_harq_it = get_harq_ack_granted_allocated(user, slot_alloc.result.ul.pucchs);
      const pucch_info* existing_pucch_harq_grant =
          pucch_harq_it == slot_alloc.result.ul.pucchs.end() ? nullptr : pucch_harq_it;
      pucch_allocate_sr(slot_alloc, user.crnti, existing_pucch_harq_grant, ue_cell.cfg(), *sr_pucch_resource_cfg);
    }
  }
}

void pucch_scheduler_impl::pucch_run_sr_allocator(cell_resource_allocator& cell_alloc, slot_point sl_tx)
{
  // Return if there are no users to serve.
  if (ues.empty()) {
    return;
  }

  // Set all PUCCH resources as unused at the beginning of the slot.
  resource_manager.reset();

  // Iterate over the users to check for SR opportunities.
  for (auto& ue : ues) {
    sched_ue_sr_opportunity(cell_alloc, ue, sl_tx);
  }
}
