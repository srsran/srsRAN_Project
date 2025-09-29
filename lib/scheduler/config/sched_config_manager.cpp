/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "sched_config_manager.h"
#include "../logging/scheduler_metrics_handler.h"
#include "srsran/scheduler/config/scheduler_cell_config_validator.h"
#include "srsran/scheduler/config/scheduler_ue_config_validator.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

ue_config_update_event::ue_config_update_event(du_ue_index_t                     ue_index_,
                                               sched_config_manager&             parent_,
                                               std::unique_ptr<ue_configuration> next_cfg,
                                               const std::optional<bool>&        set_fallback,
                                               slot_point                        ul_ccch_slot_rx_,
                                               bool                              reestablished_) :
  ue_index(ue_index_),
  parent(&parent_),
  next_ded_cfg(std::move(next_cfg)),
  set_fallback_mode(set_fallback),
  ul_ccch_slot_rx(ul_ccch_slot_rx_),
  reestablished(reestablished_)
{
}

ue_config_update_event::~ue_config_update_event()
{
  if (parent != nullptr) {
    // Event completed with failure.
    parent->handle_ue_config_complete(ue_index, nullptr);
  }
}

void ue_config_update_event::notify_completion()
{
  // Event completed with success.
  parent->handle_ue_config_complete(ue_index, std::move(next_ded_cfg));
  parent.reset();
}

ue_config_delete_event::ue_config_delete_event(du_ue_index_t         ue_index_,
                                               du_cell_index_t       pcell_index_,
                                               sched_config_manager& parent_) :
  ue_idx(ue_index_), pcell_idx(pcell_index_), parent(&parent_)
{
}

ue_config_delete_event::~ue_config_delete_event()
{
  reset();
}

void ue_config_delete_event::reset()
{
  if (parent != nullptr) {
    parent->handle_ue_delete_complete(ue_idx);
    parent = nullptr;
  }
}

// class: sched_config_manager

sched_config_manager::sched_config_manager(const scheduler_config&    sched_cfg,
                                           scheduler_metrics_handler& metrics_handler_) :
  expert_params(sched_cfg.expert_params),
  metrics_handler(metrics_handler_),
  config_notifier(sched_cfg.config_notifier),
  logger(srslog::fetch_basic_logger("SCHED")),
  ues_to_rem(MAX_NOF_DU_UES)
{
  std::fill(ue_to_pcell_index.begin(), ue_to_pcell_index.end(), INVALID_DU_CELL_INDEX);
}

const cell_configuration* sched_config_manager::add_cell(const sched_cell_configuration_request_message& msg)
{
  srsran_assert(msg.cell_index < MAX_NOF_DU_CELLS, "cell index={} is not valid", fmt::underlying(msg.cell_index));
  srsran_assert(not added_cells.contains(msg.cell_index), "cell={} already exists", fmt::underlying(msg.cell_index));

  // Ensure the common cell config is valid.
  auto ret = config_validators::validate_sched_cell_configuration_request_message(msg, expert_params);
  srsran_assert(ret.has_value(), "Invalid cell configuration request message. Cause: {}", ret.error().c_str());

  if (not group_cfg_pool.contains(msg.cell_group_index)) {
    group_cfg_pool.emplace(msg.cell_group_index, std::make_unique<du_cell_group_config_pool>());
  }
  group_cfg_pool[msg.cell_group_index]->add_cell(msg);

  added_cells.emplace(msg.cell_index, std::make_unique<cell_configuration>(expert_params, msg));

  cell_metrics_handler* cell_metrics = metrics_handler.add_cell(*added_cells[msg.cell_index], msg.metrics);
  srsran_assert(cell_metrics != nullptr, "Unable to create metrics handler");

  return added_cells[msg.cell_index].get();
}

void sched_config_manager::update_cell(const sched_cell_reconfiguration_request_message& msg)
{
  if (msg.slice_reconf_req.has_value()) {
    const auto& cell_index = msg.slice_reconf_req->cell_index;
    srsran_assert(added_cells.contains(cell_index), "cell={} does not exist", fmt::underlying(cell_index));
    for (const auto& rrm : msg.slice_reconf_req->rrm_policies) {
      bool found = false;
      for (auto& slice : added_cells[cell_index]->rrm_policy_members) {
        if (slice.rrc_member == rrm.rrc_member) {
          found         = true;
          slice.max_prb = rrm.max_prb;
          slice.min_prb = rrm.min_prb;
          break;
        }
      }

      if (not found) {
        logger.warning("No slice RRM policy found for {} in cell {}.", rrm.rrc_member, fmt::underlying(cell_index));
      }
    }
  }
}

void sched_config_manager::rem_cell(du_cell_index_t cell_index)
{
  const du_cell_group_index_t group_index = added_cells[cell_index]->cell_group_index;

  // Eliminate metrics.
  metrics_handler.rem_cell(cell_index);

  // Eliminate respective cell configuration.
  added_cells.erase(cell_index);

  // Remove cell configs from the group.
  group_cfg_pool[group_index]->rem_cell(cell_index);
}

ue_config_update_event sched_config_manager::add_ue(const sched_ue_creation_request_message& cfg_req)
{
  srsran_assert(cfg_req.ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", fmt::underlying(cfg_req.ue_index));

  // See if there are any pending events to process out of the critical path.
  flush_ues_to_rem();

  // Ensure PCell exists.
  if (not cfg_req.cfg.cells.has_value() or cfg_req.cfg.cells->empty()) {
    logger.warning("ue={} rnti={}: Discarding invalid UE creation request. Cause: PCell config not provided",
                   fmt::underlying(cfg_req.ue_index),
                   cfg_req.crnti);
    return ue_config_update_event{cfg_req.ue_index, *this};
  }

  // Ensure configured cells exist.
  const du_cell_index_t pcell_index = (*cfg_req.cfg.cells)[0].serv_cell_cfg.cell_index;
  if (not contains(pcell_index)) {
    logger.warning("ue={} rnti={}: Discarding invalid UE creation request. Cause: PCell={} does not exist",
                   fmt::underlying(cfg_req.ue_index),
                   cfg_req.crnti,
                   fmt::underlying(pcell_index));
    return ue_config_update_event{cfg_req.ue_index, *this};
  }

  error_type<std::string> result =
      config_validators::validate_sched_ue_creation_request_message(cfg_req, *added_cells[pcell_index]);
  if (not result.has_value()) {
    logger.warning("ue={} rnti={}: Discarding invalid UE creation request. Cause: {}",
                   fmt::underlying(cfg_req.ue_index),
                   cfg_req.crnti,
                   result.error());
    return ue_config_update_event{cfg_req.ue_index, *this};
  }

  // Ensure no UE with the same identifier exists. Lock this UE index.
  du_cell_index_t last_pcell_idx = INVALID_DU_CELL_INDEX;
  if (not ue_to_pcell_index[cfg_req.ue_index].compare_exchange_strong(last_pcell_idx,
                                                                      pcell_index,
                                                                      std::memory_order::memory_order_acquire,
                                                                      std::memory_order::memory_order_acquire)) {
    logger.warning(
        "ue={} rnti={}: Discarding invalid UE creation request. Cause: UE with the same index already exists",
        fmt::underlying(cfg_req.ue_index),
        cfg_req.crnti);
    return ue_config_update_event{cfg_req.ue_index, *this};
  }
  srsran_assert(ue_cfg_list[cfg_req.ue_index] == nullptr, "Invalid ue_index={}", fmt::underlying(cfg_req.ue_index));

  // Create UE configuration.
  const du_cell_group_index_t target_grp_idx = get_cell_group_index(pcell_index);
  auto                        next_ded_cfg   = std::make_unique<ue_configuration>(
      cfg_req.ue_index, cfg_req.crnti, added_cells, group_cfg_pool[target_grp_idx]->add_ue(cfg_req));

  return ue_config_update_event{cfg_req.ue_index,
                                *this,
                                std::move(next_ded_cfg),
                                cfg_req.starts_in_fallback,
                                cfg_req.ul_ccch_slot_rx.value_or(slot_point())};
}

ue_config_update_event sched_config_manager::update_ue(const sched_ue_reconfiguration_message& cfg_req)
{
  srsran_assert(cfg_req.ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", fmt::underlying(cfg_req.ue_index));

  // See if there are any pending events to process out of the critical path.
  flush_ues_to_rem();

  // Check if UE already exists.
  const du_cell_index_t       pcell_index = get_pcell_index(cfg_req.ue_index);
  const du_cell_group_index_t group_idx   = get_cell_group_index(pcell_index);
  if (group_idx == INVALID_DU_CELL_GROUP_INDEX) {
    logger.error("ue={}: Discarding UE configuration. Cause: UE does not exist", fmt::underlying(cfg_req.ue_index));
    return ue_config_update_event{cfg_req.ue_index, *this};
  }
  srsran_assert(ue_cfg_list[cfg_req.ue_index] != nullptr, "Invalid ue_index={}", fmt::underlying(cfg_req.ue_index));
  const ue_configuration& current_ue_cfg = *ue_cfg_list[cfg_req.ue_index];
  if (current_ue_cfg.crnti != cfg_req.crnti) {
    logger.error("ue={} c-rnti={}: Discarding UE configuration. Cause: UE with provided C-RNTI does not exist.",
                 fmt::underlying(cfg_req.ue_index),
                 cfg_req.crnti);
    return ue_config_update_event{cfg_req.ue_index, *this, nullptr, {}, slot_point(), cfg_req.reestablished};
  }

  // Make a copy of the current UE dedicated config.
  auto next_ded_cfg = std::make_unique<ue_configuration>(current_ue_cfg);

  // Apply the delta config.
  next_ded_cfg->update(added_cells, group_cfg_pool[group_idx]->reconf_ue(cfg_req));

  // Return RAII event.
  return ue_config_update_event{
      cfg_req.ue_index, *this, std::move(next_ded_cfg), {}, slot_point(), cfg_req.reestablished};
}

ue_config_delete_event sched_config_manager::remove_ue(du_ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", fmt::underlying(ue_index));

  // See if there are any pending events to process out of the critical path.
  flush_ues_to_rem();

  // Check if UE already exists.
  const du_cell_index_t       pcell_index = get_pcell_index(ue_index);
  const du_cell_group_index_t group_idx   = get_cell_group_index(pcell_index);
  if (group_idx == INVALID_DU_CELL_GROUP_INDEX) {
    srsran_assert(ue_cfg_list[ue_index] == nullptr, "Invalid ue_index={}", fmt::underlying(ue_index));
    logger.error("ue={}: Discarding UE deletion command. Cause: UE does not exist", fmt::underlying(ue_index));

    // Notifies MAC that event is complete.
    // Note: There is no failure path for the deletion of a UE.
    config_notifier.on_ue_deletion_completed(ue_index);

    return ue_config_delete_event{};
  }

  srsran_assert(ue_cfg_list[ue_index] != nullptr, "Invalid ue_index={}", fmt::underlying(ue_index));
  du_cell_index_t pcell_idx = ue_cfg_list[ue_index]->pcell_cfg().cell_cfg_common.cell_index;
  return ue_config_delete_event{ue_index, pcell_idx, *this};
}

void sched_config_manager::handle_ue_config_complete(du_ue_index_t ue_index, std::unique_ptr<ue_configuration> next_cfg)
{
  if (next_cfg != nullptr) {
    // Creation/Reconfig succeeded.

    cell_metrics_handler& cell_metrics = metrics_handler.at(next_cfg->pcell_common_cfg().cell_index);
    if (ue_cfg_list[ue_index] == nullptr) {
      // UE creation case.
      cell_metrics.handle_ue_creation(ue_index, next_cfg->crnti, next_cfg->pcell_common_cfg().pci);
    } else {
      // Reconfiguration case.
      cell_metrics.handle_ue_reconfiguration(ue_index);
    }

    // Stores new UE config and deletes old config.
    ue_cfg_list[ue_index].swap(next_cfg);
    if (not ues_to_rem.try_push(std::move(next_cfg))) {
      logger.warning("Failed to offload UE config removal. Performance may be affected");
    }

    // Notifies MAC that event is complete.
    config_notifier.on_ue_config_complete(ue_index, true);

  } else {
    if (ue_cfg_list[ue_index] == nullptr) {
      // In case of failed UE creation, mark the UE as released.
      ue_to_pcell_index[ue_index].store(INVALID_DU_CELL_INDEX, std::memory_order_release);
    }

    // Notifies MAC that event has failed.
    config_notifier.on_ue_config_complete(ue_index, false);
  }
}

void sched_config_manager::handle_ue_delete_complete(du_ue_index_t ue_index)
{
  du_cell_index_t pcell_idx = ue_cfg_list[ue_index]->pcell_common_cfg().cell_index;

  // Deletes UE config.
  auto old_ue_cfg = std::move(ue_cfg_list[ue_index]);
  ue_cfg_list[ue_index].reset();
  if (not ues_to_rem.try_push(std::move(old_ue_cfg))) {
    logger.warning("Failed to offload UE config removal. Performance may be affected");
  }

  // Remove UE from metrics.
  cell_metrics_handler& cell_metrics = metrics_handler.at(pcell_idx);
  cell_metrics.handle_ue_deletion(ue_index);

  // Mark the UE as released.
  ue_to_pcell_index[ue_index].store(INVALID_DU_CELL_INDEX, std::memory_order_release);

  // Notifies MAC that event is complete.
  config_notifier.on_ue_deletion_completed(ue_index);
}

void sched_config_manager::flush_ues_to_rem()
{
  // Note: This should be called by a thread outside of the critical path.

  // Clear the UEs to rem.
  std::unique_ptr<ue_configuration> obj;
  while (ues_to_rem.try_pop(obj)) {
  }
}
