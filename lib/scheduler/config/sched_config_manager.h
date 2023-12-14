/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ue_configuration.h"
#include "srsran/scheduler/config/scheduler_config.h"

namespace srsran {

class sched_config_manager;

class ue_config_update_event
{
public:
  ue_config_update_event() = default;
  ue_config_update_event(du_ue_index_t                     ue_index_,
                         sched_config_manager&             parent_,
                         std::unique_ptr<ue_configuration> next_cfg = nullptr);
  ue_config_update_event(ue_config_update_event&&) noexcept;
  ue_config_update_event& operator=(ue_config_update_event&&) noexcept;
  ~ue_config_update_event();

  bool valid() const { return next_ded_cfg != nullptr; }

  du_ue_index_t           get_ue_index() const { return ue_index; }
  const ue_configuration& next_config() const { return *next_ded_cfg; }

  void abort();

private:
  du_ue_index_t                     ue_index = INVALID_DU_UE_INDEX;
  sched_config_manager*             parent   = nullptr;
  std::unique_ptr<ue_configuration> next_ded_cfg;
};

class ue_config_delete_event
{
public:
  ue_config_delete_event() = default;
  ue_config_delete_event(du_ue_index_t ue_index_, sched_config_manager& parent_);
  ue_config_delete_event(ue_config_delete_event&& other) noexcept;
  ue_config_delete_event& operator=(ue_config_delete_event&& other) noexcept;
  ~ue_config_delete_event();

  bool valid() const { return parent != nullptr; }

  du_ue_index_t ue_index() const { return ue_idx; }

private:
  du_ue_index_t         ue_idx = INVALID_DU_UE_INDEX;
  sched_config_manager* parent = nullptr;
};

/// Note: We have the guarantee that each UE is not configured concurrently.
class sched_config_manager
{
public:
  sched_config_manager(const scheduler_config& sched_cfg_);

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

  const scheduler_expert_config expert_params;
  sched_configuration_notifier& config_notifier;
  srslog::basic_logger&         logger;

  // List of common configs for the scheduler cells.
  cell_common_configuration_list added_cells;

  std::array<std::unique_ptr<ue_configuration>, MAX_NOF_DU_UES> ue_cfg_list;

  /// Mapping of DU cells to DU Cell Groups.
  slotted_id_table<du_cell_index_t, du_cell_group_index_t, MAX_NOF_DU_CELLS> du_cell_to_cell_group_index;

  /// Mapping of UEs to DU Cell Groups.
  std::array<std::atomic<du_cell_group_index_t>, MAX_NOF_DU_UES> ue_to_cell_group_index;
};

} // namespace srsran