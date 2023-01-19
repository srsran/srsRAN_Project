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

#include "e1ap_bearer_transaction_manager.h"
#include "srsgnb/adt/slotted_array.h"
#include <unordered_map>

namespace srsgnb {
namespace srs_cu_cp {

struct e1ap_ue_context {
  cu_cp_ue_id_t          cu_cp_ue_id      = cu_cp_ue_id_t::invalid;
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::invalid;
  gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::invalid;

  e1ap_bearer_transaction_manager bearer_ev_mng;

  e1ap_ue_context(cu_cp_ue_id_t cu_cp_ue_id_, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id_, timer_manager& timers_) :
    cu_cp_ue_id(cu_cp_ue_id_), cu_cp_ue_e1ap_id(cu_cp_ue_e1ap_id_), bearer_ev_mng(timers_)
  {
  }
};

class e1ap_ue_context_list
{
public:
  e1ap_ue_context_list(timer_manager& timers_) : timers(timers_) {}

  bool contains(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id) const { return ues.find(cu_cp_ue_e1ap_id) != ues.end(); }

  /// \brief Checks whether a UE with the given CU-CP UE ID exists.
  /// \param[in] cu_cp_ue_id The CU-CP UE ID used to find the UE.
  /// \return The CU CP UE ID.
  bool contains(cu_cp_ue_id_t cu_cp_ue_id) const
  {
    return cu_cp_ue_id_to_ue_e1ap_id.find(cu_cp_ue_id) != cu_cp_ue_id_to_ue_e1ap_id.end();
  }

  e1ap_ue_context& operator[](gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_id) { return ues.at(cu_cp_ue_id); }
  e1ap_ue_context& operator[](cu_cp_ue_id_t cu_cp_ue_id) { return ues.at(cu_cp_ue_id_to_ue_e1ap_id.at(cu_cp_ue_id)); }

  e1ap_ue_context& add_ue(cu_cp_ue_id_t cu_cp_ue_id, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id)
  {
    ues.emplace(std::piecewise_construct,
                std::forward_as_tuple(cu_cp_ue_e1ap_id),
                std::forward_as_tuple(cu_cp_ue_id, cu_cp_ue_e1ap_id, timers));
    cu_cp_ue_id_to_ue_e1ap_id.emplace(cu_cp_ue_id, cu_cp_ue_e1ap_id);
    return ues.at(cu_cp_ue_e1ap_id);
  }

  void remove_ue(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_id)
  {
    cu_cp_ue_id_to_ue_e1ap_id.erase(ues.at(cu_cp_ue_id).cu_cp_ue_id);
    ues.erase(cu_cp_ue_id);
  }

  size_t size() const { return ues.size(); }

  /// \brief Get the next available GNB-CU-CP-E1AP-UE-ID.
  gnb_cu_cp_ue_e1ap_id_t next_gnb_cu_cp_ue_e1ap_id() const
  {
    if (cu_cp_ue_id_to_ue_e1ap_id.size() == MAX_NOF_CU_UES) {
      return gnb_cu_cp_ue_e1ap_id_t::invalid;
    }

    for (unsigned it = 0; it < gnb_cu_cp_ue_e1ap_id_to_uint(gnb_cu_cp_ue_e1ap_id_t::max); it++) {
      // Only iterate over cu_cp_ue_id_to_ue_e1ap_id (size=MAX NOF CU UEs)
      // to avoid iterating over all possible values of gnb_cu_cp_ue_e1ap_id_t (size=2^32-1)
      auto it2 = std::find_if(cu_cp_ue_id_to_ue_e1ap_id.begin(), cu_cp_ue_id_to_ue_e1ap_id.end(), [it](auto& u) {
        return u.second == int_to_gnb_cu_cp_ue_e1ap_id(it);
      });

      if (it2 == cu_cp_ue_id_to_ue_e1ap_id.end()) {
        return int_to_gnb_cu_cp_ue_e1ap_id(it);
      }
    }

    return gnb_cu_cp_ue_e1ap_id_t::invalid;
  }

private:
  timer_manager& timers;

  std::unordered_map<gnb_cu_cp_ue_e1ap_id_t, e1ap_ue_context> ues; // indexed by gnb_cu_cp_ue_e1ap_id
  std::unordered_map<cu_cp_ue_id_t, gnb_cu_cp_ue_e1ap_id_t>   cu_cp_ue_id_to_ue_e1ap_id; // indexed by cu_cp_ue_id
};

} // namespace srs_cu_cp
} // namespace srsgnb