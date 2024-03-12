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

#include "measurement_context.h"
#include "srsran/cu_cp/cell_meas_manager_config.h"
#include "srsran/cu_cp/cu_cp_types.h"

namespace srsran {
namespace srs_cu_cp {

/// Methods used by cell measurement manager to signal measurement events to the mobility manager.
class cell_meas_mobility_manager_notifier
{
public:
  virtual ~cell_meas_mobility_manager_notifier() = default;

  /// \brief Notifies that a neighbor cell became stronger than the current serving cell.
  virtual void on_neighbor_better_than_spcell(ue_index_t ue_index, pci_t neighbor_pci) = 0;
};

class cell_meas_manager_ue_context
{
public:
  /// \brief Get the next available meas id.
  meas_id_t allocate_meas_id()
  {
    // return invalid when no meas id is available
    if (meas_ids.size() == MAX_NOF_MEAS) {
      return meas_id_t::invalid;
    }

    auto new_meas_id = (meas_id_t)meas_ids.find_first_empty();
    if (new_meas_id == meas_id_t::max) {
      return meas_id_t::invalid;
    }
    meas_ids.emplace(meas_id_to_uint(new_meas_id));
    return new_meas_id;
  }

  void remove_meas_id(meas_id_t meas_id) { meas_ids.erase(meas_id_to_uint(meas_id)); }

  /// \brief Get the next available meas obj id.
  meas_obj_id_t allocate_meas_obj_id()
  {
    // return invalid when no meas obj id is available
    if (meas_obj_ids.size() == MAX_NOF_MEAS_OBJ) {
      return meas_obj_id_t::invalid;
    }

    auto new_meas_obj_id = (meas_obj_id_t)meas_obj_ids.find_first_empty();
    if (new_meas_obj_id == meas_obj_id_t::max) {
      return meas_obj_id_t::invalid;
    }
    meas_obj_ids.emplace(meas_obj_id_to_uint(new_meas_obj_id));
    return new_meas_obj_id;
  }

  void remove_meas_obj_id(meas_obj_id_t meas_obj_id) { meas_obj_ids.erase(meas_obj_id_to_uint(meas_obj_id)); }

  slotted_array<meas_id_t, MAX_NOF_MEAS>         meas_ids;     // 0 is reserved for invalid meas_id
  slotted_array<meas_obj_id_t, MAX_NOF_MEAS_OBJ> meas_obj_ids; // 0 is reserved for invalid meas_obj_id

  std::map<meas_id_t, meas_context_t>   meas_id_to_meas_context;
  std::map<meas_obj_id_t, nr_cell_id_t> meas_obj_id_to_nci;

  cell_meas_manager_ue_context()
  {
    // Mark zero index as occupied as the first valid meas(-obj) ID is 1.
    meas_ids.emplace(0);
    meas_obj_ids.emplace(0);
  }
};

/// Basic cell manager implementation
class cell_meas_manager
{
public:
  cell_meas_manager(const cell_meas_manager_cfg& cfg_, cell_meas_mobility_manager_notifier& mobility_mng_notifier_);
  ~cell_meas_manager() = default;

  optional<rrc_meas_cfg>
  get_measurement_config(ue_index_t ue_index, nr_cell_id_t nci, optional<rrc_meas_cfg> current_meas_config = {});
  optional<cell_meas_config> get_cell_config(nr_cell_id_t nci);
  void                       update_cell_config(nr_cell_id_t                                  nci,
                                                const serving_cell_meas_config&               serv_cell_cfg_,
                                                const std::vector<neighbor_cell_meas_config>& ncells_ = {});
  void                       report_measurement(ue_index_t ue_index, const rrc_meas_results& meas_results);

  void remove_ue_context(ue_index_t ue_index);

private:
  cell_meas_manager_cfg                cfg;
  cell_meas_mobility_manager_notifier& mobility_mng_notifier;

  std::map<ue_index_t, cell_meas_manager_ue_context> ue_contexts;

  srslog::basic_logger& logger;
};

} // namespace srs_cu_cp
} // namespace srsran
