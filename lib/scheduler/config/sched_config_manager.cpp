/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sched_config_manager.h"
#include "../logging/scheduler_metrics_ue_configurator.h"
#include "srsran/scheduler/config/scheduler_cell_config_validator.h"
#include "srsran/scheduler/config/scheduler_ue_config_validator.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

ue_config_update_event::ue_config_update_event(du_ue_index_t                     ue_index_,
                                               sched_config_manager&             parent_,
                                               std::unique_ptr<ue_configuration> next_cfg,
                                               const std::optional<bool>&        set_fallback) :
  ue_index(ue_index_), parent(&parent_), next_ded_cfg(std::move(next_cfg)), set_fallback_mode(set_fallback)
{
}

ue_config_update_event::~ue_config_update_event()
{
  if (parent != nullptr) {
    // Event completed with success or failure.
    parent->handle_ue_config_complete(ue_index, std::move(next_ded_cfg));
  }
}

void ue_config_update_event::abort()
{
  next_ded_cfg = nullptr;
}

ue_config_delete_event::ue_config_delete_event(du_ue_index_t ue_index_, sched_config_manager& parent_) :
  ue_idx(ue_index_), parent(&parent_)
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

sched_config_manager::sched_config_manager(const scheduler_config& sched_cfg) :
  expert_params(sched_cfg.expert_params),
  config_notifier(sched_cfg.config_notifier),
  logger(srslog::fetch_basic_logger("SCHED"))
{
  std::fill(ue_to_cell_group_index.begin(), ue_to_cell_group_index.end(), INVALID_DU_CELL_GROUP_INDEX);
}

const cell_configuration* sched_config_manager::add_cell(const sched_cell_configuration_request_message& msg,
                                                         sched_metrics_ue_configurator& metrics_handler_)
{
  srsran_assert(msg.cell_index < MAX_NOF_DU_CELLS, "cell index={} is not valid", msg.cell_index);
  srsran_assert(not added_cells.contains(msg.cell_index), "cell={} already exists", msg.cell_index);

  // Ensure the common cell config is valid.
  auto ret = config_validators::validate_sched_cell_configuration_request_message(msg, expert_params);
  srsran_assert(ret.has_value(), "Invalid cell configuration request message. Cause: {}", ret.error().c_str());

  added_cells.emplace(msg.cell_index, std::make_unique<cell_configuration>(expert_params, msg));
  cell_metrics.emplace(msg.cell_index, &metrics_handler_);

  return added_cells[msg.cell_index].get();
}

ue_config_update_event sched_config_manager::add_ue(const sched_ue_creation_request_message& cfg_req)
{
  srsran_assert(cfg_req.ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", cfg_req.ue_index);

  // Ensure PCell exists.
  if (not cfg_req.cfg.cells.has_value() or cfg_req.cfg.cells->empty()) {
    logger.warning("ue={} rnti={}: Discarding invalid UE creation request. Cause: PCell config not provided",
                   cfg_req.ue_index,
                   cfg_req.crnti);
    return ue_config_update_event{cfg_req.ue_index, *this};
  }

  // Ensure configured cells exist.
  const du_cell_index_t pcell_index = (*cfg_req.cfg.cells)[0].serv_cell_cfg.cell_index;
  if (not contains(pcell_index)) {
    logger.warning("ue={} rnti={}: Discarding invalid UE creation request. Cause: PCell={} does not exist",
                   cfg_req.ue_index,
                   cfg_req.crnti,
                   pcell_index);
    return ue_config_update_event{cfg_req.ue_index, *this};
  }

  error_type<std::string> result =
      config_validators::validate_sched_ue_creation_request_message(cfg_req, *added_cells[pcell_index]);
  if (not result.has_value()) {
    logger.warning("ue={} rnti={}: Discarding invalid UE creation request. Cause: {}",
                   cfg_req.ue_index,
                   cfg_req.crnti,
                   result.error());
    return ue_config_update_event{cfg_req.ue_index, *this};
  }

  // Ensure no UE with the same identifier exists. Lock this UE index.
  const du_cell_group_index_t target_grp_idx = added_cells[pcell_index]->cell_group_index;
  du_cell_group_index_t       last_grp_idx   = INVALID_DU_CELL_GROUP_INDEX;
  if (not ue_to_cell_group_index[cfg_req.ue_index].compare_exchange_strong(last_grp_idx,
                                                                           target_grp_idx,
                                                                           std::memory_order::memory_order_acquire,
                                                                           std::memory_order::memory_order_acquire)) {
    logger.warning(
        "ue={} rnti={}: Discarding invalid UE creation request. Cause: UE with the same index already exists",
        cfg_req.ue_index,
        cfg_req.crnti);
    return ue_config_update_event{cfg_req.ue_index, *this};
  }
  srsran_assert(ue_cfg_list[cfg_req.ue_index] == nullptr, "Invalid ue_index={}", cfg_req.ue_index);

  // Create UE configuration.
  auto next_ded_cfg = std::make_unique<ue_configuration>(cfg_req.ue_index, cfg_req.crnti, added_cells, cfg_req.cfg);

  return ue_config_update_event{cfg_req.ue_index, *this, std::move(next_ded_cfg), cfg_req.starts_in_fallback};
}

ue_config_update_event sched_config_manager::update_ue(const sched_ue_reconfiguration_message& cfg_req)
{
  srsran_assert(cfg_req.ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", cfg_req.ue_index);

  // Check if UE already exists.
  const du_cell_group_index_t group_idx = ue_to_cell_group_index[cfg_req.ue_index].load(std::memory_order_relaxed);
  if (group_idx == INVALID_DU_CELL_GROUP_INDEX) {
    logger.error("ue={}: Discarding UE configuration. Cause: UE does not exist", cfg_req.ue_index);
    return ue_config_update_event{cfg_req.ue_index, *this};
  }
  srsran_assert(ue_cfg_list[cfg_req.ue_index] != nullptr, "Invalid ue_index={}", cfg_req.ue_index);
  const ue_configuration& current_ue_cfg = *ue_cfg_list[cfg_req.ue_index];
  if (current_ue_cfg.crnti != cfg_req.crnti) {
    logger.error("ue={} c-rnti={}: Discarding UE configuration. Cause: UE with provided C-RNTI does not exist.",
                 cfg_req.ue_index,
                 cfg_req.crnti);
    return ue_config_update_event{cfg_req.ue_index, *this};
  }

  // Make a copy of the current UE dedicated config.
  auto next_ded_cfg = std::make_unique<ue_configuration>(current_ue_cfg);

  // Apply the delta config.
  next_ded_cfg->update(added_cells, cfg_req.cfg);

  // Return RAII event.
  return ue_config_update_event{cfg_req.ue_index, *this, std::move(next_ded_cfg)};
}

ue_config_delete_event sched_config_manager::remove_ue(du_ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);

  // Check if UE already exists.
  const du_cell_group_index_t group_idx = ue_to_cell_group_index[ue_index].load(std::memory_order_relaxed);
  if (group_idx == INVALID_DU_CELL_GROUP_INDEX) {
    srsran_assert(ue_cfg_list[ue_index] == nullptr, "Invalid ue_index={}", ue_index);
    logger.error("ue={}: Discarding UE deletion command. Cause: UE does not exist", ue_index);

    // Notifies MAC that event is complete.
    // Note: There is no failure path for the deletion of a UE.
    config_notifier.on_ue_delete_response(ue_index);

    return ue_config_delete_event{};
  }

  srsran_assert(ue_cfg_list[ue_index] != nullptr, "Invalid ue_index={}", ue_index);
  return ue_config_delete_event{ue_index, *this};
}

void sched_config_manager::handle_ue_config_complete(du_ue_index_t ue_index, std::unique_ptr<ue_configuration> next_cfg)
{
  if (next_cfg != nullptr) {
    // Creation/Reconfig succeeded.

    if (ue_cfg_list[ue_index] == nullptr) {
      // UE creation case.
      cell_metrics[next_cfg->pcell_common_cfg().cell_index]->handle_ue_creation(
          ue_index,
          next_cfg->crnti,
          next_cfg->pcell_common_cfg().pci,
          next_cfg->pcell_common_cfg().nof_dl_prbs,
          next_cfg->pcell_common_cfg().nof_slots_per_frame);
    } else {
      // Reconfiguration case.
      cell_metrics[next_cfg->pcell_common_cfg().cell_index]->handle_ue_reconfiguration(ue_index, next_cfg->crnti);
    }

    // Stores new UE config and deletes old config.
    ue_cfg_list[ue_index] = std::move(next_cfg);

    // Notifies MAC that event is complete.
    config_notifier.on_ue_config_complete(ue_index, true);

  } else {
    if (ue_cfg_list[ue_index] == nullptr) {
      // In case of failed UE creation, mark the UE as released.
      ue_to_cell_group_index[ue_index].store(INVALID_DU_CELL_GROUP_INDEX, std::memory_order_release);
    }

    // Notifies MAC that event has failed.
    config_notifier.on_ue_config_complete(ue_index, false);
  }
}

void sched_config_manager::handle_ue_delete_complete(du_ue_index_t ue_index)
{
  du_cell_index_t pcell_idx = ue_cfg_list[ue_index]->pcell_common_cfg().cell_index;

  // Deletes UE config.
  ue_cfg_list[ue_index].reset();

  // Remove UE from metrics.
  cell_metrics[pcell_idx]->handle_ue_deletion(ue_index);

  // Mark the UE as released.
  ue_to_cell_group_index[ue_index].store(INVALID_DU_CELL_GROUP_INDEX, std::memory_order_release);

  // Notifies MAC that event is complete.
  config_notifier.on_ue_delete_response(ue_index);
}
