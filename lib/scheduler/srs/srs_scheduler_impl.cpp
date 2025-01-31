/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srs_scheduler_impl.h"
#include "../cell/resource_grid.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

srs_scheduler_impl::srs_scheduler_impl(const cell_configuration& cell_cfg_, ue_repository& ues_) :
  cell_cfg(cell_cfg_), ues(ues_), logger(srslog::fetch_basic_logger("SCHED"))
{
  // Max size of the SRS resource slot wheel, dimensioned based on the maximum SRS periods.
  periodic_srs_slot_wheel.resize(static_cast<unsigned>(srs_periodicity::sl2560));

  // Pre-reserve space for the UEs that will be added.
  updated_ues.reserve(MAX_NOF_DU_UES);
}

srs_scheduler_impl::~srs_scheduler_impl() = default;

/////////////////////          Public functions        ////////////////////////////

void srs_scheduler_impl::run_slot(cell_resource_allocator& cell_alloc)
{
  // Initial allocation: we allocate opportunities all over the grid.
  schedule_updated_ues_srs(cell_alloc);

  // Only allocate in the farthest slot in the grid. The allocation in the first slots of the grid has been completed by
  // the previous function.
  schedule_slot_srs(cell_alloc[cell_alloc.max_ul_slot_alloc_delay]);
}

void srs_scheduler_impl::add_ue(const ue_cell_configuration& ue_cfg)
{
  add_ue_to_grid(ue_cfg, false);
}

void srs_scheduler_impl::add_ue_to_grid(const ue_cell_configuration& ue_cfg, bool is_reconf)
{
  if (not ue_cfg.cfg_dedicated().ul_config.has_value() or
      not ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.srs_cfg.has_value()) {
    return;
  }
  const srs_config& srs_cfg = ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.srs_cfg.value();

  auto get_srs_res_with_id = [&srs_cfg](unsigned srs_res_id) {
    return std::find_if(
        srs_cfg.srs_res_list.begin(),
        srs_cfg.srs_res_list.end(),
        [srs_res_id](const srs_config::srs_resource& srs_res) { return srs_res.id.ue_res_id == srs_res_id; });
  };

  for (const auto& srs_res_set :
       ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.srs_cfg.value().srs_res_set_list) {
    // This scheduler is only for periodic SRS resources.
    if (not std::holds_alternative<srs_config::srs_resource_set::periodic_resource_type>(srs_res_set.res_type)) {
      continue;
    }

    for (const auto& srs_res_id : srs_res_set.srs_res_id_list) {
      const auto* srs_res = get_srs_res_with_id(srs_res_id);

      if (srs_res == srs_cfg.srs_res_list.end()) {
        logger.error("rnti={} SRS resource set id={} has an invalid SRS resource ID {}",
                     ue_cfg.crnti,
                     fmt::underlying(srs_res_set.id),
                     fmt::underlying(srs_res_id));
        continue;
      }
      // We assume that a periodic SRS resource set only contains periodic SRS resources. This has been checked in the
      // scheduler configuration validator.
      srsran_sanity_check(srs_res->periodicity_and_offset.has_value(),
                          "rnti={}: Periodicity and offset not set for SRS resource ID={}",
                          ue_cfg.crnti,
                          fmt::underlying(srs_res->id.ue_res_id));
      add_resource(ue_cfg.crnti,
                   srs_res->periodicity_and_offset.value().period,
                   srs_res->periodicity_and_offset.value().offset,
                   srs_res->id.ue_res_id);
    }
  }

  // Register the UE in the list of recently configured UEs.
  // Note: We skip this step during RRC Reconfiguration because it would involve cancelling already scheduled SRSs
  // in the grid. While we don't fully support this feature, we leave the old SRSs in the grid. The worst that
  // can happen is some misdetected SRSs for a short period of time.
  if (not is_reconf) {
    updated_ues.push_back(ue_cfg.crnti);
  }
}

void srs_scheduler_impl::rem_ue(const ue_cell_configuration& ue_cfg)
{
  if (not ue_cfg.cfg_dedicated().ul_config.has_value() or
      not ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.srs_cfg.has_value()) {
    return;
  }
  const srs_config& srs_cfg = ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.srs_cfg.value();

  auto get_srs_res_with_id = [&srs_cfg](unsigned srs_res_id) {
    return std::find_if(
        srs_cfg.srs_res_list.begin(),
        srs_cfg.srs_res_list.end(),
        [srs_res_id](const srs_config::srs_resource& srs_res) { return srs_res.id.ue_res_id == srs_res_id; });
  };

  for (const auto& srs_res_set : srs_cfg.srs_res_set_list) {
    if (not std::holds_alternative<srs_config::srs_resource_set::periodic_resource_type>(srs_res_set.res_type)) {
      continue;
    }

    for (const auto& srs_res_id : srs_res_set.srs_res_id_list) {
      const auto* srs_res = get_srs_res_with_id(srs_res_id);

      if (srs_res == srs_cfg.srs_res_list.end()) {
        logger.error("rnti={} SRS resource set id={} has an invalid SRS resource ID {}",
                     ue_cfg.crnti,
                     fmt::underlying(srs_res_set.id),
                     fmt::underlying(srs_res_id));
        continue;
      }
      // We assume that a periodic SRS resource set only contains periodic SRS resources. This has been checked in the
      // scheduler configuration validator.
      srsran_sanity_check(srs_res->periodicity_and_offset.has_value(),
                          "rnti={}: Periodicity and offset not set for SRS resource ID={}",
                          ue_cfg.crnti,
                          fmt::underlying(srs_res->id.ue_res_id));
      rem_resource(ue_cfg.crnti,
                   srs_res->periodicity_and_offset.value().period,
                   srs_res->periodicity_and_offset.value().offset,
                   srs_res->id.ue_res_id);
    }
  }
}

void srs_scheduler_impl::reconf_ue(const ue_cell_configuration& new_ue_cfg, const ue_cell_configuration& old_ue_cfg)
{
  if (new_ue_cfg.cfg_dedicated().ul_config.has_value() and old_ue_cfg.cfg_dedicated().ul_config.has_value() and
      new_ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.srs_cfg.has_value() and
      old_ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.srs_cfg.has_value()) {
    // Both old and new UE config have SRS config.
    const auto& new_srs_cfg = new_ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.srs_cfg.value();
    const auto& old_srs_cfg = old_ue_cfg.cfg_dedicated().ul_config.value().init_ul_bwp.srs_cfg.value();

    if (new_srs_cfg.srs_res_set_list == old_srs_cfg.srs_res_set_list and
        new_srs_cfg.srs_res_list == old_srs_cfg.srs_res_list) {
      // Nothing changed.
      return;
    }
  }

  rem_ue(old_ue_cfg);
  add_ue_to_grid(new_ue_cfg, true);
}

void srs_scheduler_impl::handle_positioning_measurement_request(const positioning_measurement_request& req)
{
  // Ensure uniqueness of RNTI in the \c pending_pos_requests.
  auto it = std::find_if(
      pending_pos_requests.begin(),
      pending_pos_requests.end(),
      [&req](const positioning_measurement_request& prev_req) { return prev_req.pos_rnti == req.pos_rnti; });
  if (it != pending_pos_requests.end()) {
    // Avoid more than one positioning request per C-RNTI.
    logger.info("rnti={}: Positioning measurement request discarded. Cause: A previous request with the same RNTI is "
                "currently active",
                req.pos_rnti);
    return;
  }

  if (req.ue_index.has_value()) {
    // It is a positioning request for a connected UE.
    if (not ues.contains(req.ue_index.value())) {
      logger.warning("ue={}: Positioning measurement request discarded. Cause: Non-existent UE",
                     fmt::underlying(req.ue_index.value()));
      return;
    }
    auto& u = ues[req.ue_index.value()];
    if (u.crnti != req.pos_rnti) {
      logger.warning("ue={}: Positioning measurement request discarded. Cause: Incorrect C-RNTI",
                     fmt::underlying(req.ue_index.value()));
      return;
    }
    const serving_cell_config& ue_cfg = u.get_pcell().cfg().cfg_dedicated();

    if (not ue_cfg.ul_config.has_value() or not ue_cfg.ul_config.value().init_ul_bwp.srs_cfg.has_value()) {
      logger.warning("ue={}: Positioning measurement request discarded. Cause: Could not find reusable SRS",
                     fmt::underlying(req.ue_index.value()));
      return;
    }

    // Register positioning request.
    pending_pos_requests.push_back(req);
  } else {
    // It is a positioning measurement for a UE of another cell.

    // Register positioning request.
    pending_pos_requests.push_back(req);

    // Add SRS config to slot wheel.
    for (const auto& srs_res : req.srs_to_measure.srs_res_list) {
      add_resource(req.pos_rnti,
                   srs_res.periodicity_and_offset.value().period,
                   srs_res.periodicity_and_offset.value().offset,
                   srs_res.id.ue_res_id);
    }
  }
}

void srs_scheduler_impl::handle_positioning_measurement_stop(du_cell_index_t cell_index, rnti_t pos_rnti)
{
  auto it = std::find_if(
      pending_pos_requests.begin(),
      pending_pos_requests.end(),
      [pos_rnti](const positioning_measurement_request& prev_req) { return prev_req.pos_rnti == pos_rnti; });
  if (it == pending_pos_requests.end()) {
    logger.warning("rnti={}: Positioning measurement could not be stopped. Cause: No matching positioning procedure "
                   "for the same RNTI",
                   pos_rnti);
    return;
  }

  if (not it->ue_index.has_value()) {
    // Case of positioning for a neighbor cell UE.
    for (const auto& srs_res : it->srs_to_measure.srs_res_list) {
      rem_resource(it->pos_rnti,
                   srs_res.periodicity_and_offset.value().period,
                   srs_res.periodicity_and_offset.value().offset,
                   srs_res.id.ue_res_id);
    }
  }

  // Remote positioning request from pending list.
  pending_pos_requests.erase(it);
}

/////////////////////          Private functions        ////////////////////////////

void srs_scheduler_impl::schedule_slot_srs(srsran::cell_slot_resource_allocator& slot_alloc)
{
  // For the provided slot, check if there are any pending SRS resources to allocate, and allocate them.
  auto& slot_srss = periodic_srs_slot_wheel[slot_alloc.slot.to_uint() % periodic_srs_slot_wheel.size()];
  for (auto srs_info_it : slot_srss) {
    allocate_srs_opportunity(slot_alloc, srs_info_it);
  }
}

void srs_scheduler_impl::schedule_updated_ues_srs(cell_resource_allocator& cell_alloc)
{
  if (not updated_ues.empty()) {
    // Schedule SRS up to the farthest slot.
    for (unsigned n = 0; n != cell_alloc.max_ul_slot_alloc_delay; ++n) {
      auto& slot_srss = periodic_srs_slot_wheel[(cell_alloc.slot_tx() + n).to_uint() % periodic_srs_slot_wheel.size()];

      // For all the periodic SRS info element at this slot, allocate only those that belong to the UE updated_ues.
      for (const periodic_srs_info& srs : slot_srss) {
        const bool rnti_in_updated_ues =
            std::find(updated_ues.begin(), updated_ues.end(), srs.rnti) != updated_ues.end();
        if (rnti_in_updated_ues) {
          allocate_srs_opportunity(cell_alloc[n], srs);
        }
      }
    }

    // Clear the list of updated UEs.
    updated_ues.clear();
  }
}

bool srs_scheduler_impl::allocate_srs_opportunity(cell_slot_resource_allocator& slot_alloc,
                                                  const periodic_srs_info&      srs_opportunity)
{
  slot_point sl_srs = slot_alloc.slot;

  const ue_cell_configuration* ue_cfg = get_ue_cfg(srs_opportunity.rnti);
  if (ue_cfg == nullptr) {
    logger.error("cell={} c-rnti={}: UE for which SRS is being scheduled was not found",
                 fmt::underlying(cell_cfg.cell_index),
                 srs_opportunity.rnti);
    return false;
  }

  if (not ue_cfg->cell_cfg_common.is_ul_enabled(sl_srs)) {
    logger.warning("cell={} c-rnti={}: slot={} for SRS resource id={} is being scheduled is not UL enabled",
                   fmt::underlying(cell_cfg.cell_index),
                   srs_opportunity.rnti,
                   sl_srs,
                   fmt::underlying(srs_opportunity.srs_res_id));
    return false;
  }

  if (slot_alloc.result.ul.srss.full()) {
    logger.warning("cell={} c-rnti={}: SRS resource id={} cannot be allocated for slot={}. Cause: SRS list is full",
                   fmt::underlying(cell_cfg.cell_index),
                   srs_opportunity.rnti,
                   fmt::underlying(srs_opportunity.srs_res_id),
                   sl_srs);
    return false;
  }

  const auto srs_res_list = ue_cfg->cfg_dedicated().ul_config.value().init_ul_bwp.srs_cfg.value().srs_res_list;

  // Retrieve the SRS resource ID from the UE dedicated config.
  const auto* res_it = std::find_if(
      srs_res_list.begin(), srs_res_list.end(), [srs_opportunity](const srs_config::srs_resource& srs_res) {
        return srs_res.id.ue_res_id == srs_opportunity.srs_res_id;
      });

  if (res_it == srs_res_list.end()) {
    logger.warning("cell={} c-rnti={}: SRS resource id={} cannot be allocated for slot={}. Cause: SRS resource not "
                   "found in UE ded. config",
                   fmt::underlying(cell_cfg.cell_index),
                   srs_opportunity.rnti,
                   fmt::underlying(srs_opportunity.srs_res_id),
                   sl_srs);
    return false;
  }

  const unsigned nof_symbs_per_slot = get_nsymb_per_slot(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp);
  const unsigned starting_symb      = nof_symbs_per_slot - res_it->res_mapping.start_pos - 1;
  slot_alloc.ul_res_grid.fill(
      grant_info(cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.scs,
                 ofdm_symbol_range{starting_symb, starting_symb + static_cast<unsigned>(res_it->res_mapping.nof_symb)},
                 cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs));
  fill_srs_pdu(slot_alloc.result.ul.srss.emplace_back(), *res_it, *ue_cfg);

  return true;
}

void srs_scheduler_impl::fill_srs_pdu(srs_info&                       pdu,
                                      const srs_config::srs_resource& srs_res_cfg,
                                      const ue_cell_configuration&    ue_cfg)
{
  pdu.crnti             = ue_cfg.crnti;
  pdu.bwp_cfg           = &cell_cfg.ul_cfg_common.init_ul_bwp.generic_params;
  pdu.nof_antenna_ports = static_cast<uint8_t>(srs_res_cfg.nof_ports);

  const unsigned nof_symbs_per_slot = get_nsymb_per_slot(cell_cfg.dl_cfg_common.init_dl_bwp.generic_params.cp);
  const unsigned starting_symb      = nof_symbs_per_slot - srs_res_cfg.res_mapping.start_pos - 1;
  pdu.symbols.set(starting_symb, starting_symb + static_cast<unsigned>(srs_res_cfg.res_mapping.nof_symb));
  pdu.nof_repetitions = srs_res_cfg.res_mapping.rept_factor;

  pdu.config_index         = srs_res_cfg.freq_hop.c_srs;
  pdu.sequence_id          = static_cast<uint8_t>(srs_res_cfg.sequence_id);
  pdu.bw_index             = srs_res_cfg.freq_hop.b_srs;
  pdu.tx_comb              = srs_res_cfg.tx_comb.size;
  pdu.comb_offset          = srs_res_cfg.tx_comb.tx_comb_offset;
  pdu.cyclic_shift         = srs_res_cfg.tx_comb.tx_comb_cyclic_shift;
  pdu.freq_position        = srs_res_cfg.freq_domain_pos;
  pdu.freq_shift           = srs_res_cfg.freq_domain_shift;
  pdu.freq_hopping         = srs_res_cfg.freq_hop.b_hop;
  pdu.group_or_seq_hopping = srs_res_cfg.grp_or_seq_hop;
  pdu.resource_type        = srs_res_cfg.res_type;

  pdu.t_srs_period = srs_res_cfg.periodicity_and_offset.value().period;
  pdu.t_offset     = srs_res_cfg.periodicity_and_offset.value().offset;

  pdu.normalized_channel_iq_matrix_requested = true;
  pdu.positioning_report_requested           = false;

  // Check if there is a pending positioning request.
  for (const auto& pos_req : pending_pos_requests) {
    if (pos_req.pos_rnti == pdu.crnti) {
      pdu.positioning_report_requested = true;
      break;
    }
  }
}

void srs_scheduler_impl::add_resource(rnti_t                 crnti,
                                      srs_periodicity        res_period,
                                      unsigned               res_offset,
                                      srs_config::srs_res_id res_id)
{
  // Add UE-SRS resource element for each offset in the periodic SRS slot wheel.
  auto srs_period = static_cast<unsigned>(res_period);
  for (unsigned wheel_offset = res_offset, wheel_size = periodic_srs_slot_wheel.size(); wheel_offset < wheel_size;
       wheel_offset += srs_period) {
    auto& slot_wheel = periodic_srs_slot_wheel[wheel_offset];

    // Check if the UE is already in the slot wheel.
    auto* it = std::find_if(slot_wheel.begin(), slot_wheel.end(), [crnti, res_id](const auto& r) {
      return r.rnti == crnti and r.srs_res_id == res_id;
    });

    if (it == slot_wheel.end()) {
      // New UE-SRS resource: create a new element in the list of SRS opportunities.
      slot_wheel.push_back(periodic_srs_info{crnti, res_id});
    } else {
      logger.error("rnti={}: SRS resource id={} with period={} and offset={} already exists in the SRS slot wheel",
                   crnti,
                   fmt::underlying(res_id),
                   fmt::underlying(res_period));
    }
  }
}

void srs_scheduler_impl::rem_resource(rnti_t                 crnti,
                                      srs_periodicity        res_period,
                                      unsigned               res_offset,
                                      srs_config::srs_res_id res_id)
{
  // For each offset in the periodic SRS slot wheel.
  auto srs_period = static_cast<unsigned>(res_period);
  for (unsigned wheel_offset = res_offset, wheel_size = periodic_srs_slot_wheel.size(); wheel_offset < wheel_size;
       wheel_offset += srs_period) {
    auto& slot_wheel = periodic_srs_slot_wheel[wheel_offset];

    // Check if the UE-SRS resource element is still in the slot wheel.
    auto* it = std::find_if(slot_wheel.begin(), slot_wheel.end(), [crnti, res_id](const auto& r) {
      return r.rnti == crnti and r.srs_res_id == res_id;
    });

    if (it != slot_wheel.end()) {
      // Move resource to last position and delete it to avoid O(N) removal.
      if (it != slot_wheel.end() - 1) {
        auto* last_it = slot_wheel.end() - 1;
        std::swap(*it, *last_it);
      }
      slot_wheel.pop_back();

    } else {
      logger.error(
          "rnti={}: no SRS resource id={} with period={} and offset={} found in the SRS slot wheel during UE removal",
          crnti,
          fmt::underlying(res_id),
          fmt::underlying(res_period),
          res_offset);
    }
  }
}

/////////////////////          Helper functions        ////////////////////////////

const ue_cell_configuration* srs_scheduler_impl::get_ue_cfg(rnti_t rnti) const
{
  auto* u = ues.find_by_rnti(rnti);
  if (u != nullptr) {
    auto* ue_cc = u->find_cell(cell_cfg.cell_index);
    if (ue_cc != nullptr) {
      return &ue_cc->cfg();
    }
  }
  return nullptr;
}
