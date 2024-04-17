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
#include "e1ap_ue_ids.h"
#include "e1ap_ue_logger.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

/// \brief E1AP UE context.
class e1ap_ue_context
{
public:
  e1ap_ue_ids ue_ids;

  e1ap_bearer_transaction_manager bearer_ev_mng;

  e1ap_ue_logger logger;

  e1ap_ue_context(ue_index_t ue_index_, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id_, timer_factory timers_) :
    ue_ids({ue_index_, cu_cp_ue_e1ap_id_}), bearer_ev_mng(timers_), logger("CU-CP-E1", {ue_index_, cu_cp_ue_e1ap_id_})
  {
  }

  const e1ap_ue_ids& get_ue_ids() const { return ue_ids; }

  void update_cu_up_ue_e1ap_id(gnb_cu_up_ue_e1ap_id_t up_ue_id)
  {
    srsran_assert(up_ue_id != gnb_cu_up_ue_e1ap_id_t::invalid, "Invalid cu_up_ue_e1ap_id={}", up_ue_id);
    ue_ids.cu_up_ue_e1ap_id = up_ue_id;

    // Update prefix logger.
    logger.set_prefix({ue_ids.ue_index, ue_ids.cu_cp_ue_e1ap_id, ue_ids.cu_up_ue_e1ap_id});
    logger.log_debug("Updated CU-UP-E1AP-ID");
  }
};

/// \brief List of E1AP UE Contexts in the CU-CP for a given CU-UP.
class e1ap_ue_context_list
{
public:
  e1ap_ue_context_list(timer_factory timers_, unsigned max_nof_supported_ues_, srslog::basic_logger& logger_) :
    timers(timers_), max_nof_supported_ues(max_nof_supported_ues_), logger(logger_)
  {
  }

  /// \brief Checks whether a UE with the given CU-CP-UE-E1AP-ID exists.
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

  /// \brief Search for a UE based on its CU-CP-UE-E1AP-ID.
  const e1ap_ue_context* find_ue(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id) const
  {
    auto it = ues.find(cu_cp_ue_e1ap_id);
    if (it == ues.end()) {
      return nullptr;
    }
    return &it->second;
  }
  e1ap_ue_context* find_ue(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id)
  {
    auto it = ues.find(cu_cp_ue_e1ap_id);
    if (it == ues.end()) {
      return nullptr;
    }
    return &it->second;
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

  /// \brief Create new UE E1AP Context.
  e1ap_ue_context* add_ue(ue_index_t ue_index, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id);

  /// Remove existing E1AP UE Context.
  void remove_ue(ue_index_t ue_index);

  size_t size() const { return ues.size(); }

  /// \brief Get the next available GNB-CU-CP-E1AP-UE-ID.
  gnb_cu_cp_ue_e1ap_id_t allocate_gnb_cu_cp_ue_e1ap_id();

  /// \brief Transfer E1AP UE context to new CU-CP specific UE index.
  void update_ue_index(ue_index_t new_ue_index, ue_index_t old_ue_index);

private:
  gnb_cu_cp_ue_e1ap_id_t next_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::min;

  timer_factory         timers;
  unsigned              max_nof_supported_ues = MAX_NOF_CU_UES;
  srslog::basic_logger& logger;

  void increase_next_cu_cp_ue_e1ap_id();

  std::unordered_map<gnb_cu_cp_ue_e1ap_id_t, e1ap_ue_context> ues;                    // indexed by gnb_cu_cp_ue_e1ap_id
  std::unordered_map<ue_index_t, gnb_cu_cp_ue_e1ap_id_t>      ue_index_to_ue_e1ap_id; // indexed by ue_index
};

} // namespace srs_cu_cp
} // namespace srsran