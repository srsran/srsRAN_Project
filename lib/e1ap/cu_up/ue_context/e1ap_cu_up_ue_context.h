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

#include "e1ap_ue_logger.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/cu_up/cu_up_types.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_up {

struct e1ap_ue_ids {
  ue_index_t                   ue_index         = INVALID_UE_INDEX;
  const gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::invalid;
  gnb_cu_cp_ue_e1ap_id_t       cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::invalid;
};

struct e1ap_ue_context {
  e1ap_ue_ids                   ue_ids;
  activity_notification_level_t activity_notification_level = activity_notification_level_t::invalid;
  e1ap_ue_logger                logger;

  e1ap_ue_context(ue_index_t                    ue_index_,
                  gnb_cu_up_ue_e1ap_id_t        cu_up_ue_e1ap_id_,
                  gnb_cu_cp_ue_e1ap_id_t        cu_cp_ue_e1ap_id_,
                  activity_notification_level_t activity_notification_level_) :
    ue_ids({ue_index_, cu_up_ue_e1ap_id_, cu_cp_ue_e1ap_id_}),
    activity_notification_level(activity_notification_level_),
    logger("CU-UP-E1", {ue_index_, cu_up_ue_e1ap_id_, cu_cp_ue_e1ap_id_})
  {
  }
};

class e1ap_ue_context_list
{
public:
  e1ap_ue_context_list(srslog::basic_logger& logger_) : logger(logger_) {}

  bool contains(gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id) const { return ues.find(cu_up_ue_e1ap_id) != ues.end(); }

  /// \brief Checks whether a UE with the given UE index exists.
  /// \param[in] ue_index The UE index used to find the UE.
  /// \return The UE Index.
  bool contains(ue_index_t ue_index) const
  {
    if (ue_index_to_ue_e1ap_id.find(ue_index) == ue_index_to_ue_e1ap_id.end()) {
      return false;
    }
    if (ues.find(ue_index_to_ue_e1ap_id.at(ue_index)) == ues.end()) {
      logger.warning("No UE context found for cu_up_ue_e1ap_id={}.", ue_index_to_ue_e1ap_id.at(ue_index));
      return false;
    }
    return true;
  }

  e1ap_ue_context& operator[](gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
  {
    srsran_assert(
        ues.find(cu_up_ue_e1ap_id) != ues.end(), "cu_up_ue_e1ap_id={}: E1AP UE context not found", cu_up_ue_e1ap_id);
    return ues.at(cu_up_ue_e1ap_id);
  }
  e1ap_ue_context& operator[](ue_index_t ue_index)
  {
    srsran_assert(ue_index_to_ue_e1ap_id.find(ue_index) != ue_index_to_ue_e1ap_id.end(),
                  "ue={} gNB-CU-UP-UE-E1AP-ID not found",
                  ue_index);
    srsran_assert(ues.find(ue_index_to_ue_e1ap_id.at(ue_index)) != ues.end(),
                  "cu_up_ue_e1ap_id={}: E1AP UE context not found",
                  ue_index_to_ue_e1ap_id.at(ue_index));
    return ues.at(ue_index_to_ue_e1ap_id.at(ue_index));
  }

  e1ap_ue_context& add_ue(ue_index_t                    ue_index,
                          gnb_cu_up_ue_e1ap_id_t        cu_up_ue_e1ap_id,
                          gnb_cu_cp_ue_e1ap_id_t        cu_cp_ue_e1ap_id,
                          activity_notification_level_t activity_notification_level)
  {
    srsran_assert(ue_index != INVALID_UE_INDEX, "Invalid ue_index={}", ue_index);
    srsran_assert(cu_up_ue_e1ap_id != gnb_cu_up_ue_e1ap_id_t::invalid, "Invalid cu_up_ue_e1ap_id={}", cu_up_ue_e1ap_id);
    srsran_assert(cu_cp_ue_e1ap_id != gnb_cu_cp_ue_e1ap_id_t::invalid, "Invalid cu_cp_ue_e1ap_id={}", cu_cp_ue_e1ap_id);

    logger.debug("ue={} cu_up_ue_e1ap_id={} cu_cp_ue_e1ap_id={}: Adding E1AP UE context",
                 ue_index,
                 cu_up_ue_e1ap_id,
                 cu_cp_ue_e1ap_id);
    ues.emplace(std::piecewise_construct,
                std::forward_as_tuple(cu_up_ue_e1ap_id),
                std::forward_as_tuple(ue_index, cu_up_ue_e1ap_id, cu_cp_ue_e1ap_id, activity_notification_level));
    ue_index_to_ue_e1ap_id.emplace(ue_index, cu_up_ue_e1ap_id);
    return ues.at(cu_up_ue_e1ap_id);
  }

  void remove_ue(ue_index_t ue_index)
  {
    srsran_assert(ue_index != INVALID_UE_INDEX, "Invalid ue_index={}", ue_index);

    if (ue_index_to_ue_e1ap_id.find(ue_index) == ue_index_to_ue_e1ap_id.end()) {
      logger.warning("ue={}: GNB-CU-UP-UE-E1AP-ID not found", ue_index);
      return;
    }

    // Remove UE from lookup
    gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = ue_index_to_ue_e1ap_id.at(ue_index);
    ue_index_to_ue_e1ap_id.erase(ue_index);

    if (ues.find(cu_up_ue_e1ap_id) == ues.end()) {
      logger.warning("cu_up_ue_e1ap_id={}: UE context not found", cu_up_ue_e1ap_id);
      return;
    }

    ues.at(cu_up_ue_e1ap_id).logger.log_debug("Removing UE context");
    ues.erase(cu_up_ue_e1ap_id);
  }

  size_t size() const { return ues.size(); }

  /// \brief Get the next available GNB-CU-UP-E1AP-UE-ID.
  gnb_cu_up_ue_e1ap_id_t next_gnb_cu_up_ue_e1ap_id()
  {
    if (ue_index_to_ue_e1ap_id.size() == MAX_NOF_UES) {
      return gnb_cu_up_ue_e1ap_id_t::invalid;
    }

    // Check if the next_cu_up_ue_e1ap_id is available
    if (ues.find(next_cu_up_ue_e1ap_id) == ues.end()) {
      gnb_cu_up_ue_e1ap_id_t ret = next_cu_up_ue_e1ap_id;
      // increase the next cu-up ue e1ap id
      increase_next_cu_up_ue_e1ap_id();
      return ret;
    }

    // Find holes in the allocated IDs by iterating over all ids starting with the next_cu_up_ue_e1ap_id to find the
    // available id
    while (true) {
      // Only iterate over ue_index_to_ue_e1ap_id (size=MAX NOF CU UEs)
      // to avoid iterating over all possible values of gnb_cu_up_ue_e1ap_id_t (size=2^32-1)
      auto it = std::find_if(ue_index_to_ue_e1ap_id.begin(), ue_index_to_ue_e1ap_id.end(), [this](auto& u) {
        return u.second == next_cu_up_ue_e1ap_id;
      });

      // return the id if it is not already used
      if (it == ue_index_to_ue_e1ap_id.end()) {
        gnb_cu_up_ue_e1ap_id_t ret = next_cu_up_ue_e1ap_id;
        // increase the next cu-up ue e1ap id
        increase_next_cu_up_ue_e1ap_id();
        return ret;
      }

      // increase the next cu-up ue e1ap id and try again
      increase_next_cu_up_ue_e1ap_id();
    }

    return gnb_cu_up_ue_e1ap_id_t::invalid;
  }

protected:
  gnb_cu_up_ue_e1ap_id_t next_cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;

private:
  srslog::basic_logger& logger;

  inline void increase_next_cu_up_ue_e1ap_id()
  {
    if (next_cu_up_ue_e1ap_id == gnb_cu_up_ue_e1ap_id_t::max) {
      // reset cu-up ue e1ap id counter
      next_cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;
    } else {
      // increase cu-up ue e1ap id counter
      next_cu_up_ue_e1ap_id = int_to_gnb_cu_up_ue_e1ap_id(gnb_cu_up_ue_e1ap_id_to_uint(next_cu_up_ue_e1ap_id) + 1);
    }
  }

  std::unordered_map<gnb_cu_up_ue_e1ap_id_t, e1ap_ue_context> ues;                    // indexed by gnb_cu_up_ue_e1ap_id
  std::unordered_map<ue_index_t, gnb_cu_up_ue_e1ap_id_t>      ue_index_to_ue_e1ap_id; // indexed by ue_index
};

} // namespace srs_cu_up
} // namespace srsran
