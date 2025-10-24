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

#pragma once

#include "f1ap_cu_ue_transaction_manager.h"
#include "f1ap_ue_logger.h"
#include "ue_ul_bearer_manager.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/f1ap/f1ap_message_notifier.h"
#include "srsran/f1ap/f1ap_ue_id_types.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

struct f1ap_ue_context {
  f1ap_ue_ids ue_ids;
  bool        marked_for_release = false;
  /// Whether the old gNB-DU UE F1AP UE ID IE needs to be notified back to the DU, due to reestablishment.
  std::optional<gnb_du_ue_f1ap_id_t> pending_old_ue_id;
  f1ap_ue_transaction_manager        ev_mng;
  f1ap_ue_logger                     logger;

  f1ap_ue_context(ue_index_t ue_index_, gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id_, timer_factory timers_) :
    ue_ids({ue_index_, cu_ue_f1ap_id_}), ev_mng(timers_), logger("CU-CP-F1", {ue_ids}, ": ")
  {
  }

  /// Returns the instance of the Uplink bearer manager of this UE.
  ue_ul_bearer_manager&       get_ul_bearer_manager() { return bearer_manager; }
  const ue_ul_bearer_manager& get_ul_bearer_manager() const { return bearer_manager; }

  /// Handles Downlink RRC messages.
  void handle_dl_rrc_message(const f1ap_dl_rrc_message& msg, f1ap_message_notifier& msg_notifier);

private:
  ue_ul_bearer_manager bearer_manager;
};

class f1ap_ue_context_list
{
public:
  f1ap_ue_context_list(timer_factory timers_, srslog::basic_logger& logger_) : timers(timers_), logger(logger_) {}

  bool contains(gnb_cu_ue_f1ap_id_t cu_ue_id) const { return ues.find(cu_ue_id) != ues.end(); }

  /// \brief Checks whether a UE with the given UE index exists.
  /// \param[in] ue_index The UE index used to find the UE.
  /// \return The CU UE ID.
  bool contains(ue_index_t ue_index) const
  {
    if (ue_index_to_ue_f1ap_id.find(ue_index) == ue_index_to_ue_f1ap_id.end()) {
      return false;
    }
    if (ues.find(ue_index_to_ue_f1ap_id.at(ue_index)) == ues.end()) {
      return false;
    }
    return true;
  }

  f1ap_ue_context& operator[](gnb_cu_ue_f1ap_id_t cu_ue_id)
  {
    srsran_assert(ues.find(cu_ue_id) != ues.end(), "cu_ue={}: F1AP UE context not found", fmt::underlying(cu_ue_id));
    return ues.at(cu_ue_id);
  }
  f1ap_ue_context& operator[](ue_index_t ue_index)
  {
    srsran_assert(ue_index_to_ue_f1ap_id.find(ue_index) != ue_index_to_ue_f1ap_id.end(),
                  "ue={} gNB-CU-UE-F1AP-ID not found",
                  fmt::underlying(ue_index));
    srsran_assert(ues.find(ue_index_to_ue_f1ap_id.at(ue_index)) != ues.end(),
                  "cu_ue={}: F1AP UE context not found",
                  fmt::underlying(ue_index_to_ue_f1ap_id.at(ue_index)));
    return ues.at(ue_index_to_ue_f1ap_id.at(ue_index));
  }

  const f1ap_ue_context* find(gnb_du_ue_f1ap_id_t du_ue_id) const
  {
    auto it = std::find_if(
        ues.begin(), ues.end(), [du_ue_id](const std::pair<const gnb_cu_ue_f1ap_id_t, f1ap_ue_context>& u) {
          return u.second.ue_ids.du_ue_f1ap_id && u.second.ue_ids.du_ue_f1ap_id == du_ue_id;
        });
    return it != ues.end() ? &it->second : nullptr;
  }

  const f1ap_ue_context* find(gnb_cu_ue_f1ap_id_t cu_ue_id) const
  {
    auto it = ues.find(cu_ue_id);
    return it != ues.end() ? &it->second : nullptr;
  }
  f1ap_ue_context* find(gnb_cu_ue_f1ap_id_t cu_ue_id)
  {
    auto it = ues.find(cu_ue_id);
    return it != ues.end() ? &it->second : nullptr;
  }

  const f1ap_ue_context* find(ue_index_t ue_idx) const
  {
    auto it = ue_index_to_ue_f1ap_id.find(ue_idx);
    return it != ue_index_to_ue_f1ap_id.end() ? &ues.at(it->second) : nullptr;
  }

  f1ap_ue_context* find(ue_index_t ue_idx)
  {
    auto it = ue_index_to_ue_f1ap_id.find(ue_idx);
    return it != ue_index_to_ue_f1ap_id.end() ? &ues.at(it->second) : nullptr;
  }

  f1ap_ue_context& add_ue(ue_index_t ue_index, gnb_cu_ue_f1ap_id_t cu_ue_id)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", fmt::underlying(ue_index));
    srsran_assert(cu_ue_id != gnb_cu_ue_f1ap_id_t::invalid, "Invalid cu_ue={}", fmt::underlying(cu_ue_id));

    logger.debug("ue={} cu_ue={}: Adding F1AP UE context", fmt::underlying(ue_index), fmt::underlying(cu_ue_id));
    ues.emplace(
        std::piecewise_construct, std::forward_as_tuple(cu_ue_id), std::forward_as_tuple(ue_index, cu_ue_id, timers));
    ue_index_to_ue_f1ap_id.emplace(ue_index, cu_ue_id);
    return ues.at(cu_ue_id);
  }

  void add_du_ue_f1ap_id(gnb_cu_ue_f1ap_id_t cu_ue_id, gnb_du_ue_f1ap_id_t du_ue_id)
  {
    srsran_assert(cu_ue_id != gnb_cu_ue_f1ap_id_t::invalid, "Invalid cu_ue={}", fmt::underlying(cu_ue_id));
    srsran_assert(du_ue_id != gnb_du_ue_f1ap_id_t::invalid, "Invalid du_ue={}", fmt::underlying(du_ue_id));
    srsran_assert(ues.find(cu_ue_id) != ues.end(), "cu_ue={}: F1AP UE context not found", fmt::underlying(cu_ue_id));

    auto& ue = ues.at(cu_ue_id);
    ue.logger.log_debug("Adding du_ue={}", fmt::underlying(du_ue_id));
    ue.ue_ids.du_ue_f1ap_id = du_ue_id;

    ue.logger.set_prefix({ue.ue_ids.ue_index, ue.ue_ids.cu_ue_f1ap_id, ue.ue_ids.du_ue_f1ap_id}, ": ");
  }

  void remove_ue(ue_index_t ue_index)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);

    if (ue_index_to_ue_f1ap_id.find(ue_index) == ue_index_to_ue_f1ap_id.end()) {
      logger.warning("ue={}: gNB-CU-UE-F1AP-ID not found", fmt::underlying(ue_index));
      return;
    }

    // Remove UE from lookup
    gnb_cu_ue_f1ap_id_t cu_ue_id = ue_index_to_ue_f1ap_id.at(ue_index);
    ue_index_to_ue_f1ap_id.erase(ue_index);

    if (ues.find(cu_ue_id) == ues.end()) {
      logger.warning("cu_ue={}: F1AP UE context not found", fmt::underlying(cu_ue_id));
      return;
    }

    logger.debug("ue={} cu_ue={}: Removing F1AP UE context", fmt::underlying(ue_index), fmt::underlying(cu_ue_id));
    ues.erase(cu_ue_id);
  }

  void add_srb0_rrc_notifier(ue_index_t ue_index, f1ap_ul_ccch_notifier* srb0_notifier)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
    srsran_assert(ue_index_to_ue_f1ap_id.find(ue_index) != ue_index_to_ue_f1ap_id.end(),
                  "ue={}: gNB-CU-UE-F1AP-ID not found",
                  fmt::underlying(ue_index));
    srsran_assert(ues.find(ue_index_to_ue_f1ap_id.at(ue_index)) != ues.end(),
                  "cu_ue={}: F1AP UE context not found",
                  fmt::underlying(ue_index_to_ue_f1ap_id.at(ue_index)));
    srsran_assert(srb0_notifier, "Invalid SRB0 notifier");

    ues.at(ue_index_to_ue_f1ap_id.at(ue_index)).get_ul_bearer_manager().activate_srb0(*srb0_notifier);
  }

  void add_srb1_rrc_notifier(ue_index_t ue_index, f1ap_ul_dcch_notifier* srb1_notifier)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
    srsran_assert(ue_index_to_ue_f1ap_id.find(ue_index) != ue_index_to_ue_f1ap_id.end(),
                  "ue={}: gNB-CU-UE-F1AP-ID not found",
                  ue_index);
    srsran_assert(ues.find(ue_index_to_ue_f1ap_id.at(ue_index)) != ues.end(),
                  "cu_ue={}: F1AP UE context not found",
                  fmt::underlying(ue_index_to_ue_f1ap_id.at(ue_index)));
    srsran_assert(srb1_notifier, "Invalid SRB1 notifier");

    ues.at(ue_index_to_ue_f1ap_id.at(ue_index)).get_ul_bearer_manager().activate_srb1(*srb1_notifier);
  }

  void add_srb2_rrc_notifier(ue_index_t ue_index, f1ap_ul_dcch_notifier* srb2_notifier)
  {
    srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
    srsran_assert(ue_index_to_ue_f1ap_id.find(ue_index) != ue_index_to_ue_f1ap_id.end(),
                  "ue={}: gNB-CU-UE-F1AP-ID not found",
                  ue_index);
    srsran_assert(ues.find(ue_index_to_ue_f1ap_id.at(ue_index)) != ues.end(),
                  "cu_ue={}: F1AP UE context not found",
                  fmt::underlying(ue_index_to_ue_f1ap_id.at(ue_index)));
    srsran_assert(srb2_notifier, "Invalid SRB2 notifier");

    ues.at(ue_index_to_ue_f1ap_id.at(ue_index)).get_ul_bearer_manager().activate_srb2(*srb2_notifier);
  }

  size_t size() const { return ues.size(); }

  /// \brief Get the next available GNB-CU-F1AP-UE-ID.
  gnb_cu_ue_f1ap_id_t allocate_gnb_cu_ue_f1ap_id()
  {
    // return invalid when no cu ue f1ap id is available
    if (ue_index_to_ue_f1ap_id.size() == MAX_NOF_CU_F1AP_UES) {
      return gnb_cu_ue_f1ap_id_t::invalid;
    }

    // Check if the next_cu_ue_f1ap_id is available
    if (ues.find(next_cu_ue_f1ap_id) == ues.end()) {
      gnb_cu_ue_f1ap_id_t ret = next_cu_ue_f1ap_id;
      // increase the next cu ue f1ap id
      increase_next_cu_ue_f1ap_id();
      return ret;
    }

    // Find holes in the allocated IDs by iterating over all ids starting with the next_cu_ue_f1ap_id to find the
    // available id
    while (true) {
      // Iterate over ue_index_to_ue_f1ap_id
      auto it = std::find_if(ue_index_to_ue_f1ap_id.begin(), ue_index_to_ue_f1ap_id.end(), [this](auto& u) {
        return u.second == next_cu_ue_f1ap_id;
      });

      // return the id if it is not already used
      if (it == ue_index_to_ue_f1ap_id.end()) {
        gnb_cu_ue_f1ap_id_t ret = next_cu_ue_f1ap_id;
        // increase the next cu ue f1ap id
        increase_next_cu_ue_f1ap_id();
        return ret;
      }

      // increase the next cu ue f1ap id and try again
      increase_next_cu_ue_f1ap_id();
    }

    return gnb_cu_ue_f1ap_id_t::invalid;
  }

  std::unordered_map<gnb_cu_ue_f1ap_id_t, f1ap_ue_context>::iterator       begin() { return ues.begin(); }
  std::unordered_map<gnb_cu_ue_f1ap_id_t, f1ap_ue_context>::const_iterator begin() const { return ues.begin(); }
  std::unordered_map<gnb_cu_ue_f1ap_id_t, f1ap_ue_context>::iterator       end() { return ues.end(); }
  std::unordered_map<gnb_cu_ue_f1ap_id_t, f1ap_ue_context>::const_iterator end() const { return ues.end(); }

protected:
  gnb_cu_ue_f1ap_id_t next_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_t::min;

private:
  timer_factory         timers;
  srslog::basic_logger& logger;

  void increase_next_cu_ue_f1ap_id()
  {
    if (next_cu_ue_f1ap_id == gnb_cu_ue_f1ap_id_t::max) {
      // reset cu ue f1ap id counter
      next_cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_t::min;
    } else {
      // increase cu ue f1ap id counter
      next_cu_ue_f1ap_id = int_to_gnb_cu_ue_f1ap_id(gnb_cu_ue_f1ap_id_to_uint(next_cu_ue_f1ap_id) + 1);
    }
  }

  // Note: Given that UEs will self-remove from the map, we don't want to destructor to clear the lookups beforehand.
  std::unordered_map<ue_index_t, gnb_cu_ue_f1ap_id_t>      ue_index_to_ue_f1ap_id; // indexed by ue_index
  std::unordered_map<gnb_cu_ue_f1ap_id_t, f1ap_ue_context> ues;                    // indexed by gnb_cu_ue_f1ap_id
};

} // namespace srs_cu_cp
} // namespace srsran
