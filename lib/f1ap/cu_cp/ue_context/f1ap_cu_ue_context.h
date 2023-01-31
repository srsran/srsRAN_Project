/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "f1ap_cu_ue_transaction_manager.h"
#include "srsgnb/adt/slotted_array.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include <unordered_map>

namespace srsgnb {
namespace srs_cu_cp {

struct f1ap_ue_context {
  const ue_index_t          ue_index      = ue_index_t::invalid;
  const gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = gnb_cu_ue_f1ap_id_t::invalid;
  gnb_du_ue_f1ap_id_t       du_ue_f1ap_id = gnb_du_ue_f1ap_id_t::invalid;
  f1ap_srb_notifiers        srbs;

  f1ap_ue_transaction_manager ev_mng;

  f1ap_ue_context(ue_index_t ue_idx_, gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id_, timer_manager& timers_) :
    ue_index(ue_idx_), cu_ue_f1ap_id(cu_ue_f1ap_id_), ev_mng(timers_)
  {
  }
};

class f1ap_ue_context_list
{
public:
  f1ap_ue_context_list(timer_manager& timers_) : timers(timers_) {}

  bool contains(gnb_cu_ue_f1ap_id_t cu_ue_id) const { return ues.find(cu_ue_id) != ues.end(); }

  /// \brief Checks whether a UE with the given UE index exists.
  /// \param[in] ue_index The UE index used to find the UE.
  /// \return The CU UE ID.
  bool contains(ue_index_t ue_idx) const { return ue_index_to_ue_f1ap_id.find(ue_idx) != ue_index_to_ue_f1ap_id.end(); }

  f1ap_ue_context& operator[](gnb_cu_ue_f1ap_id_t cu_ue_id) { return ues.at(cu_ue_id); }
  f1ap_ue_context& operator[](ue_index_t ue_idx) { return ues.at(ue_index_to_ue_f1ap_id.at(ue_idx)); }

  f1ap_ue_context& add_ue(ue_index_t ue_idx, gnb_cu_ue_f1ap_id_t cu_ue_id)
  {
    ues.emplace(
        std::piecewise_construct, std::forward_as_tuple(cu_ue_id), std::forward_as_tuple(ue_idx, cu_ue_id, timers));
    ue_index_to_ue_f1ap_id.emplace(ue_idx, cu_ue_id);
    return ues.at(cu_ue_id);
  }

  void remove_ue(gnb_cu_ue_f1ap_id_t cu_ue_id)
  {
    ue_index_to_ue_f1ap_id.erase(ues.at(cu_ue_id).ue_index);
    ues.erase(cu_ue_id);
  }

  size_t size() const { return ues.size(); }

  /// \brief Get the next available GNB-CU-F1AP-UE-ID.
  gnb_cu_ue_f1ap_id_t next_gnb_cu_ue_f1ap_id() const
  {
    if (ue_index_to_ue_f1ap_id.size() == MAX_NOF_UES_PER_DU) {
      return gnb_cu_ue_f1ap_id_t::invalid;
    }

    for (unsigned it = 0; it < gnb_cu_ue_f1ap_id_to_uint(gnb_cu_ue_f1ap_id_t::max); it++) {
      // Only iterate over ue_index_to_ue_f1ap_id (size=MAX_NOF_UES_PER_DU)
      // to avoid iterating over all possible values of gnb_cu_ue_f1ap_id_t (size=2^32-1)
      auto it2 = std::find_if(ue_index_to_ue_f1ap_id.begin(), ue_index_to_ue_f1ap_id.end(), [it](auto& u) {
        return u.second == int_to_gnb_cu_ue_f1ap_id(it);
      });

      if (it2 == ue_index_to_ue_f1ap_id.end()) {
        return int_to_gnb_cu_ue_f1ap_id(it);
      }
    }

    return gnb_cu_ue_f1ap_id_t::invalid;
  }

private:
  timer_manager& timers;

  std::unordered_map<gnb_cu_ue_f1ap_id_t, f1ap_ue_context> ues;                    // indexed by gnb_cu_ue_f1ap_id
  std::unordered_map<ue_index_t, gnb_cu_ue_f1ap_id_t>      ue_index_to_ue_f1ap_id; // indexed by ue_index
};

} // namespace srs_cu_cp
} // namespace srsgnb