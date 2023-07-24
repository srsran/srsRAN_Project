/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/adt/slotted_array.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/srsran_assert.h"
#include <unordered_map>

namespace srsran {
namespace srs_cu_cp {

struct e1ap_ue_context {
  ue_index_t             ue_index         = ue_index_t::invalid;
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::invalid;
  gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::invalid;

  e1ap_bearer_transaction_manager bearer_ev_mng;

  e1ap_ue_context(ue_index_t ue_index_, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id_, timer_factory timers_) :
    ue_index(ue_index_), cu_cp_ue_e1ap_id(cu_cp_ue_e1ap_id_), bearer_ev_mng(timers_)
  {
  }
};

class e1ap_ue_context_list
{
public:
  e1ap_ue_context_list(timer_factory timers_) : timers(timers_) {}

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
      srslog::fetch_basic_logger("CU-CP-E1")
          .warning("No UE context found for cu_cp_ue_e1ap_id={}.", ue_index_to_ue_e1ap_id.at(ue_index));
      return false;
    }
    return true;
  }

  e1ap_ue_context& operator[](gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id) { return ues.at(cu_cp_ue_e1ap_id); }
  e1ap_ue_context& operator[](ue_index_t ue_index) { return ues.at(ue_index_to_ue_e1ap_id.at(ue_index)); }

  e1ap_ue_context& add_ue(ue_index_t ue_index, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id)
  {
    ues.emplace(std::piecewise_construct,
                std::forward_as_tuple(cu_cp_ue_e1ap_id),
                std::forward_as_tuple(ue_index, cu_cp_ue_e1ap_id, timers));
    ue_index_to_ue_e1ap_id.emplace(ue_index, cu_cp_ue_e1ap_id);
    return ues.at(cu_cp_ue_e1ap_id);
  }

  void remove_ue(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id)
  {
    srsran_assert(
        ues.find(cu_cp_ue_e1ap_id) != ues.end(), "UE context for gnb_cu_cp_ue_e1ap_id={} not found.", cu_cp_ue_e1ap_id);

    ue_index_t ue_index = ues.at(cu_cp_ue_e1ap_id).ue_index;

    srsran_assert(ue_index_to_ue_e1ap_id.find(ue_index) != ue_index_to_ue_e1ap_id.end(),
                  "GNB-CU-CP-UE-E1AP-ID for ue_index={} not found.",
                  ue_index);

    ue_index_to_ue_e1ap_id.erase(ue_index);
    ues.erase(cu_cp_ue_e1ap_id);
  }

  void remove_ue(ue_index_t ue_index)
  {
    srsran_assert(ue_index_to_ue_e1ap_id.find(ue_index) != ue_index_to_ue_e1ap_id.end(),
                  "GNB-CU-CP-UE-E1AP-ID for ue_index={} not found.",
                  ue_index);

    gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = ue_index_to_ue_e1ap_id.at(ue_index);

    srsran_assert(
        ues.find(cu_cp_ue_e1ap_id) != ues.end(), "UE context for gnb_cu_cp_ue_e1ap_id={} not found.", cu_cp_ue_e1ap_id);

    ue_index_to_ue_e1ap_id.erase(ue_index);
    ues.erase(cu_cp_ue_e1ap_id);
  }

  size_t size() const { return ues.size(); }

  /// \brief Get the next available GNB-CU-CP-E1AP-UE-ID.
  gnb_cu_cp_ue_e1ap_id_t next_gnb_cu_cp_ue_e1ap_id() const
  {
    if (ue_index_to_ue_e1ap_id.size() == MAX_NOF_CU_UES) {
      return gnb_cu_cp_ue_e1ap_id_t::invalid;
    }

    for (unsigned it = 0; it < gnb_cu_cp_ue_e1ap_id_to_uint(gnb_cu_cp_ue_e1ap_id_t::max); it++) {
      // Only iterate over ue_index_to_ue_e1ap_id (size=MAX NOF CU UEs)
      // to avoid iterating over all possible values of gnb_cu_cp_ue_e1ap_id_t (size=2^32-1)
      auto it2 = std::find_if(ue_index_to_ue_e1ap_id.begin(), ue_index_to_ue_e1ap_id.end(), [it](auto& u) {
        return u.second == int_to_gnb_cu_cp_ue_e1ap_id(it);
      });

      if (it2 == ue_index_to_ue_e1ap_id.end()) {
        return int_to_gnb_cu_cp_ue_e1ap_id(it);
      }
    }

    return gnb_cu_cp_ue_e1ap_id_t::invalid;
  }

  void update_ue_index(ue_index_t ue_index, ue_index_t old_ue_index)
  {
    // no need to update if the ue indexes are equal
    if (ue_index == old_ue_index) {
      return;
    }

    srsran_assert(ue_index_to_ue_e1ap_id.find(old_ue_index) != ue_index_to_ue_e1ap_id.end(),
                  "GNB-CU-CP-UE-E1AP-ID for ue_index={} not found.",
                  old_ue_index);

    gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = ue_index_to_ue_e1ap_id.at(old_ue_index);

    srsran_assert(
        ues.find(cu_cp_ue_e1ap_id) != ues.end(), "UE context for gnb_cu_cp_ue_e1ap_id={} not found.", cu_cp_ue_e1ap_id);

    // update ue_index
    auto& ue_ctxt    = ues.at(cu_cp_ue_e1ap_id);
    ue_ctxt.ue_index = ue_index;

    // update lookup
    ue_index_to_ue_e1ap_id.emplace(ue_index, cu_cp_ue_e1ap_id);
    ue_index_to_ue_e1ap_id.erase(old_ue_index);
  }

private:
  timer_factory timers;

  std::unordered_map<gnb_cu_cp_ue_e1ap_id_t, e1ap_ue_context> ues;                    // indexed by gnb_cu_cp_ue_e1ap_id
  std::unordered_map<ue_index_t, gnb_cu_cp_ue_e1ap_id_t>      ue_index_to_ue_e1ap_id; // indexed by ue_index
};

} // namespace srs_cu_cp
} // namespace srsran