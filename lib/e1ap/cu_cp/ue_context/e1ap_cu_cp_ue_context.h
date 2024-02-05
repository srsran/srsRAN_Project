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

#include "e1ap_bearer_transaction_manager.h"
#include "e1ap_ue_logger.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/srsran_assert.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

struct e1ap_ue_ids {
  ue_index_t                   ue_index         = ue_index_t::invalid;
  const gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::invalid;
  gnb_cu_up_ue_e1ap_id_t       cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::invalid;
};

struct e1ap_ue_context {
  e1ap_ue_ids ue_ids;

  e1ap_bearer_transaction_manager bearer_ev_mng;

  e1ap_ue_logger logger;

  e1ap_ue_context(ue_index_t ue_index_, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id_, timer_factory timers_) :
    ue_ids({ue_index_, cu_cp_ue_e1ap_id_}), bearer_ev_mng(timers_), logger("CU-CP-E1", {ue_index_, cu_cp_ue_e1ap_id_})
  {
  }
};

class e1ap_ue_context_list
{
public:
  e1ap_ue_context_list(timer_factory timers_, unsigned max_nof_supported_ues_, srslog::basic_logger& logger_) :
    timers(timers_), max_nof_supported_ues(max_nof_supported_ues_), logger(logger_)
  {
  }

  bool contains(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id) const { return ues.find(cu_cp_ue_e1ap_id) != ues.end(); }

  /// \brief Checks whether a UE with the given UE Index exists.
  /// \param[in] ue_index The UE Index used to find the UE.
  /// \return The UE Index.
  bool contains(ue_index_t ue_index) const
  {
    if (ue_index_to_ue_e1ap_id.find(ue_index) == ue_index_to_ue_e1ap_id.end()) {
      return false;
    }
    if (ues.find(ue_index_to_ue_e1ap_id.at(ue_index)) == ues.end()) {
      logger.warning("No UE context found for cu_cp_ue_e1ap_id={}", ue_index_to_ue_e1ap_id.at(ue_index));
      return false;
    }
    return true;
  }

  e1ap_ue_context& operator[](gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id)
  {
    srsran_assert(
        ues.find(cu_cp_ue_e1ap_id) != ues.end(), "cu_cp_ue_e1ap_id={}: E1AP UE context not found", cu_cp_ue_e1ap_id);
    return ues.at(cu_cp_ue_e1ap_id);
  }
  e1ap_ue_context& operator[](ue_index_t ue_index)
  {
    srsran_assert(ue_index_to_ue_e1ap_id.find(ue_index) != ue_index_to_ue_e1ap_id.end(),
                  "ue={} gNB-CU-CP-UE-E1AP-ID not found",
                  ue_index);
    srsran_assert(ues.find(ue_index_to_ue_e1ap_id.at(ue_index)) != ues.end(),
                  "cu_cp_ue_e1ap_id={}: E1AP UE context not found",
                  ue_index_to_ue_e1ap_id.at(ue_index));
    return ues.at(ue_index_to_ue_e1ap_id.at(ue_index));
  }

  e1ap_ue_context& add_ue(ue_index_t ue_index, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
    srsran_assert(cu_cp_ue_e1ap_id != gnb_cu_cp_ue_e1ap_id_t::invalid, "Invalid cu_cp_ue_e1ap_id={}", cu_cp_ue_e1ap_id);

    logger.debug("ue={} cu_cp_ue_e1ap_id={}: Adding E1AP UE context", ue_index, cu_cp_ue_e1ap_id);
    ues.emplace(std::piecewise_construct,
                std::forward_as_tuple(cu_cp_ue_e1ap_id),
                std::forward_as_tuple(ue_index, cu_cp_ue_e1ap_id, timers));
    ue_index_to_ue_e1ap_id.emplace(ue_index, cu_cp_ue_e1ap_id);
    return ues.at(cu_cp_ue_e1ap_id);
  }

  void add_cu_up_ue_e1ap_id(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id, gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id)
  {
    srsran_assert(cu_cp_ue_e1ap_id != gnb_cu_cp_ue_e1ap_id_t::invalid, "Invalid ={}", cu_cp_ue_e1ap_id);
    srsran_assert(cu_up_ue_e1ap_id != gnb_cu_up_ue_e1ap_id_t::invalid, "Invalid cu_up_ue_e1ap_id={}", cu_up_ue_e1ap_id);
    srsran_assert(
        ues.find(cu_cp_ue_e1ap_id) != ues.end(), "cu_cp_ue_e1ap_id={}: E1AP UE context not found", cu_cp_ue_e1ap_id);

    auto& ue = ues.at(cu_cp_ue_e1ap_id);
    ue.logger.log_debug("Adding cu_up_ue_e1ap_id={}", cu_up_ue_e1ap_id);
    ue.ue_ids.cu_up_ue_e1ap_id = cu_up_ue_e1ap_id;

    ue.logger.set_prefix({ue.ue_ids.ue_index, ue.ue_ids.cu_cp_ue_e1ap_id, ue.ue_ids.cu_up_ue_e1ap_id});
  }

  void remove_ue(ue_index_t ue_index)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);

    if (ue_index_to_ue_e1ap_id.find(ue_index) == ue_index_to_ue_e1ap_id.end()) {
      logger.warning("ue={}: GNB-CU-CP-UE-E1AP-ID not found", ue_index);
      return;
    }

    // Remove UE from lookup
    gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = ue_index_to_ue_e1ap_id.at(ue_index);
    ue_index_to_ue_e1ap_id.erase(ue_index);

    if (ues.find(cu_cp_ue_e1ap_id) == ues.end()) {
      logger.warning("cu_cp_ue_e1ap_id={}: UE context not found", cu_cp_ue_e1ap_id);
      return;
    }

    ues.at(cu_cp_ue_e1ap_id).logger.log_debug("Removing UE context");
    ues.erase(cu_cp_ue_e1ap_id);
  }

  size_t size() const { return ues.size(); }

  /// \brief Get the next available GNB-CU-CP-E1AP-UE-ID.
  gnb_cu_cp_ue_e1ap_id_t allocate_gnb_cu_cp_ue_e1ap_id()
  {
    if (ue_index_to_ue_e1ap_id.size() == max_nof_supported_ues) {
      return gnb_cu_cp_ue_e1ap_id_t::invalid;
    }

    // Check if the next_cu_cp_ue_e1ap_id is available
    if (ues.find(next_cu_cp_ue_e1ap_id) == ues.end()) {
      gnb_cu_cp_ue_e1ap_id_t ret = next_cu_cp_ue_e1ap_id;
      // increase the next cu-cp ue e1ap id
      increase_next_cu_cp_ue_e1ap_id();
      return ret;
    }

    // Find holes in the allocated IDs by iterating over all ids starting with the next_cu_cp_ue_e1ap_id to find the
    // available id
    while (true) {
      // Only iterate over ue_index_to_ue_e1ap_id (size=MAX NOF CU UEs)
      // to avoid iterating over all possible values of gnb_cu_cp_ue_e1ap_id_t (size=2^32-1)
      auto it = std::find_if(ue_index_to_ue_e1ap_id.begin(), ue_index_to_ue_e1ap_id.end(), [this](auto& u) {
        return u.second == next_cu_cp_ue_e1ap_id;
      });

      // return the id if it is not already used
      if (it == ue_index_to_ue_e1ap_id.end()) {
        gnb_cu_cp_ue_e1ap_id_t ret = next_cu_cp_ue_e1ap_id;
        // increase the next cu-cp ue e1ap id
        increase_next_cu_cp_ue_e1ap_id();
        return ret;
      }

      // increase the next cu-cp ue e1ap id and try again
      increase_next_cu_cp_ue_e1ap_id();
    }

    return gnb_cu_cp_ue_e1ap_id_t::invalid;
  }

  void update_ue_index(ue_index_t new_ue_index, ue_index_t old_ue_index)
  {
    srsran_assert(new_ue_index != ue_index_t::invalid, "Invalid new_ue_index={}", new_ue_index);
    srsran_assert(old_ue_index != ue_index_t::invalid, "Invalid old_ue_index={}", old_ue_index);
    srsran_assert(ue_index_to_ue_e1ap_id.find(old_ue_index) != ue_index_to_ue_e1ap_id.end(),
                  "ue={}: GNB-CU-CP-UE-E1AP-ID not found",
                  old_ue_index);

    // no need to update if the ue indexes are equal
    if (new_ue_index == old_ue_index) {
      return;
    }

    gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = ue_index_to_ue_e1ap_id.at(old_ue_index);

    srsran_assert(
        ues.find(cu_cp_ue_e1ap_id) != ues.end(), "cu_cp_ue_e1ap_id={}: UE context not found", cu_cp_ue_e1ap_id);

    // Update UE context
    ues.at(cu_cp_ue_e1ap_id).ue_ids.ue_index = new_ue_index;

    // Update lookup
    ue_index_to_ue_e1ap_id.emplace(new_ue_index, cu_cp_ue_e1ap_id);
    ue_index_to_ue_e1ap_id.erase(old_ue_index);

    // Update logger
    ues.at(cu_cp_ue_e1ap_id)
        .logger.set_prefix({ues.at(cu_cp_ue_e1ap_id).ue_ids.ue_index,
                            ues.at(cu_cp_ue_e1ap_id).ue_ids.cu_cp_ue_e1ap_id,
                            ues.at(cu_cp_ue_e1ap_id).ue_ids.cu_up_ue_e1ap_id});

    ues.at(cu_cp_ue_e1ap_id).logger.log_debug("Updated UE index from ue_index={}", old_ue_index);
  }

protected:
  gnb_cu_cp_ue_e1ap_id_t next_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::min;

private:
  timer_factory         timers;
  unsigned              max_nof_supported_ues = MAX_NOF_CU_UES;
  srslog::basic_logger& logger;

  inline void increase_next_cu_cp_ue_e1ap_id()
  {
    if (next_cu_cp_ue_e1ap_id == gnb_cu_cp_ue_e1ap_id_t::max) {
      // reset cu-cp ue e1ap id counter
      next_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::min;
    } else {
      // increase cu-cp ue e1ap id counter
      next_cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(gnb_cu_cp_ue_e1ap_id_to_uint(next_cu_cp_ue_e1ap_id) + 1);
    }
  }

  std::unordered_map<gnb_cu_cp_ue_e1ap_id_t, e1ap_ue_context> ues;                    // indexed by gnb_cu_cp_ue_e1ap_id
  std::unordered_map<ue_index_t, gnb_cu_cp_ue_e1ap_id_t>      ue_index_to_ue_e1ap_id; // indexed by ue_index
};

} // namespace srs_cu_cp
} // namespace srsran