/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "ue_configuration.h"
#include "srsran/adt/detail/operations.h"
#include "srsran/scheduler/config/scheduler_config.h"

namespace srsran {

class sched_config_manager;
class sched_metrics_ue_configurator;

/// Event to create/reconfigure a UE in the scheduler.
class ue_config_update_event
{
public:
  ue_config_update_event() = default;
  ue_config_update_event(du_ue_index_t                     ue_index_,
                         sched_config_manager&             parent_,
                         std::unique_ptr<ue_configuration> next_cfg     = nullptr,
                         const optional<bool>&             set_fallback = {});
  ue_config_update_event(ue_config_update_event&&) noexcept            = default;
  ue_config_update_event& operator=(ue_config_update_event&&) noexcept = default;
  ~ue_config_update_event();

  bool valid() const { return next_ded_cfg != nullptr; }

  du_ue_index_t           get_ue_index() const { return ue_index; }
  const ue_configuration& next_config() const { return *next_ded_cfg; }
  optional<bool>          get_fallback_command() const { return set_fallback_mode; }

  void abort();

private:
  du_ue_index_t ue_index = INVALID_DU_UE_INDEX;
  // We use a unique_ptr with no deleter to automatically set the ptr to null on move.
  std::unique_ptr<sched_config_manager, detail::noop_operation> parent;
  std::unique_ptr<ue_configuration>                             next_ded_cfg;
  optional<bool>                                                set_fallback_mode;
};

/// Event to delete a UE in the scheduler.
class ue_config_delete_event
{
public:
  ue_config_delete_event() = default;
  ue_config_delete_event(du_ue_index_t ue_index_, sched_config_manager& parent_);
  ue_config_delete_event(ue_config_delete_event&&) noexcept            = default;
  ue_config_delete_event& operator=(ue_config_delete_event&&) noexcept = default;
  ~ue_config_delete_event();

  bool valid() const { return parent != nullptr; }

  void reset();

  du_ue_index_t ue_index() const { return ue_idx; }

private:
  du_ue_index_t                                                 ue_idx = INVALID_DU_UE_INDEX;
  std::unique_ptr<sched_config_manager, detail::noop_operation> parent;
};

/// \brief Internal scheduler interface to create/update/delete UEs.
class sched_ue_configuration_handler
{
public:
  virtual ~sched_ue_configuration_handler() = default;

  /// \brief Create a new UE instance.
  virtual void handle_ue_creation(ue_config_update_event ev) = 0;

  /// \brief Reconfigure an existing UE instance.
  virtual void handle_ue_reconfiguration(ue_config_update_event ev) = 0;

  /// \brief Reconfigure an existing UE instance.
  virtual void handle_ue_deletion(ue_config_delete_event ev) = 0;
};

/// Class that handles the creation/reconfiguration/deletion of cell and UE configurations in the scheduler.
///
/// In particular, this class is responsible for:
/// - notifying back to the MAC when the configurations are complete,
/// - adding/removing UE entries in the scheduler reported metrics,
/// - validating the configuration requests provided by the MAC.
class sched_config_manager
{
public:
  sched_config_manager(const scheduler_config& sched_cfg_, sched_metrics_ue_configurator& metrics_handler_);

  const cell_configuration* add_cell(const sched_cell_configuration_request_message& msg);

  ue_config_update_event add_ue(const sched_ue_creation_request_message& cfg_req);

  ue_config_update_event update_ue(const sched_ue_reconfiguration_message& cfg_req);

  ue_config_delete_event remove_ue(du_ue_index_t ue_index);

  bool contains(du_cell_index_t cell_index) const { return added_cells.contains(cell_index); }

  du_cell_group_index_t get_cell_group_index(du_cell_index_t cell_index) const
  {
    return du_cell_to_cell_group_index.contains(cell_index) ? du_cell_to_cell_group_index[cell_index]
                                                            : INVALID_DU_CELL_GROUP_INDEX;
  }

  du_cell_group_index_t get_cell_group_index(du_ue_index_t ue_index) const
  {
    srsran_assert(ue_index < MAX_NOF_DU_UES, "Invalid ue_index={}", ue_index);
    return ue_to_cell_group_index[ue_index].load(std::memory_order_relaxed);
  }

  const cell_common_configuration_list& common_cell_list() const { return added_cells; }

private:
  friend class ue_config_update_event;
  friend class ue_config_delete_event;

  void handle_ue_config_complete(du_ue_index_t ue_index, std::unique_ptr<ue_configuration> next_cfg);
  void handle_ue_delete_complete(du_ue_index_t ue_index);

  const scheduler_expert_config  expert_params;
  sched_configuration_notifier&  config_notifier;
  sched_metrics_ue_configurator& metrics_handler;
  srslog::basic_logger&          logger;

  // List of common configs for the scheduler cells.
  cell_common_configuration_list added_cells;

  std::array<std::unique_ptr<ue_configuration>, MAX_NOF_DU_UES> ue_cfg_list;

  /// Mapping of DU cells to DU Cell Groups.
  slotted_id_table<du_cell_index_t, du_cell_group_index_t, MAX_NOF_DU_CELLS> du_cell_to_cell_group_index;

  /// Mapping of UEs to DU Cell Groups.
  std::array<std::atomic<du_cell_group_index_t>, MAX_NOF_DU_UES> ue_to_cell_group_index;
};

} // namespace srsran
