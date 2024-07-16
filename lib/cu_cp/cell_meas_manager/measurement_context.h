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

#include "srsran/adt/optional.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/ran/nr_cgi.h"
#include "srsran/ran/pci.h"
#include "srsran/rrc/meas_types.h"
#include "srsran/srslog/srslog.h"
#include <map>
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

struct meas_context_t {
  meas_obj_id_t    meas_obj_id   = meas_obj_id_t::invalid;
  report_cfg_id_t  report_cfg_id = report_cfg_id_t::invalid;
  unsigned         gnb_id_bit_length;
  nr_cell_identity nci;
  pci_t            pci;
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

  /// Remove all meas ids from the context, including lookups.
  void clear_meas_ids()
  {
    meas_id_to_meas_context.clear();
    meas_ids.clear();

    // Mark zero index as occupied as the first valid meas ID is 1.
    meas_ids.emplace(0);
  }

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

  /// Remove all meas obj ids from the context, including lookups.
  void clear_meas_obj_ids()
  {
    nci_to_meas_obj_id.clear();
    meas_obj_id_to_ncis.clear();
    meas_obj_ids.clear();

    // Mark zero index as occupied as the first valid meas obj ID is 1.
    meas_obj_ids.emplace(0);
  }

  slotted_array<meas_id_t, MAX_NOF_MEAS>         meas_ids;     // 0 is reserved for invalid meas_id
  slotted_array<meas_obj_id_t, MAX_NOF_MEAS_OBJ> meas_obj_ids; // 0 is reserved for invalid meas_obj_id

  std::map<meas_id_t, meas_context_t>                    meas_id_to_meas_context;
  std::map<nr_cell_identity, meas_obj_id_t>              nci_to_meas_obj_id;
  std::map<meas_obj_id_t, std::vector<nr_cell_identity>> meas_obj_id_to_ncis;

  cell_meas_manager_ue_context()
  {
    // Mark zero index as occupied as the first valid meas(-obj) ID is 1.
    meas_ids.emplace(0);
    meas_obj_ids.emplace(0);
  }
};

} // namespace srs_cu_cp
} // namespace srsran
