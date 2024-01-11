/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "measurement_context.h"
#include "srsran/cu_cp/cell_meas_manager.h"
#include "srsran/cu_cp/mobility_manager.h"

namespace srsran {
namespace srs_cu_cp {

/// Basic cell manager implementation
class cell_meas_manager_impl final : public cell_meas_manager
{
public:
  cell_meas_manager_impl(const cell_meas_manager_cfg& cfg, cell_meas_mobility_manager_notifier& mobility_mng_notfier_);
  ~cell_meas_manager_impl() = default;

  optional<rrc_meas_cfg> get_measurement_config(nr_cell_id_t nci, optional<rrc_meas_cfg> current_meas_config) override;
  optional<cell_meas_config> get_cell_config(nr_cell_id_t nci) override;
  void                       update_cell_config(nr_cell_id_t                           nci,
                                                const serving_cell_meas_config&        serv_cell_cfg_,
                                                std::vector<neighbor_cell_meas_config> ncells_ = {}) override;
  void report_measurement(const ue_index_t ue_index, const rrc_meas_results& meas_results) override;

  /// \brief Get the next available meas_id.
  meas_id_t get_next_meas_id();

  /// \brief Get the next available meas_obj_id.
  meas_obj_id_t get_next_meas_obj_id();

private:
  cell_meas_manager_cfg                cfg;
  cell_meas_mobility_manager_notifier& mobility_mng_notifier;

  slotted_array<meas_id_t, MAX_NOF_MEAS>         meas_ids;
  slotted_array<meas_obj_id_t, MAX_NOF_MEAS_OBJ> meas_obj_ids;

  std::map<meas_id_t, meas_context_t>   meas_id_to_meas_context;
  std::map<meas_obj_id_t, nr_cell_id_t> meas_obj_id_to_nci;

  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
