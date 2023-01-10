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

namespace srsgnb {
namespace srs_cu_cp {

struct f1ap_ue_context {
  const ue_index_t          ue_index      = INVALID_UE_INDEX;
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

  bool contains(gnb_cu_ue_f1ap_id_t cu_ue_id) const { return ues.contains(gnb_cu_ue_f1ap_id_to_uint(cu_ue_id)); }

  /// \brief Checks whether a UE with the given UE index exists.
  /// \param[in] ue_index The UE index used to find the UE.
  /// \return The CU UE ID.
  bool contains(ue_index_t ue_idx) const
  {
    return std::any_of(ues.begin(), ues.end(), [ue_idx](const f1ap_ue_context& u) { return u.ue_index == ue_idx; });
  }

  f1ap_ue_context&       operator[](gnb_cu_ue_f1ap_id_t cu_ue_id) { return ues[gnb_cu_ue_f1ap_id_to_uint(cu_ue_id)]; }
  const f1ap_ue_context& operator[](gnb_cu_ue_f1ap_id_t cu_ue_id) const
  {
    return ues[gnb_cu_ue_f1ap_id_to_uint(cu_ue_id)];
  }
  f1ap_ue_context& operator[](ue_index_t ue_idx)
  {
    auto it = std::find_if(ues.begin(), ues.end(), [ue_idx](const f1ap_ue_context& u) { return u.ue_index == ue_idx; });
    return *it;
  }

  f1ap_ue_context& add_ue(ue_index_t ue_idx, gnb_cu_ue_f1ap_id_t cu_ue_id)
  {
    ues.emplace(gnb_cu_ue_f1ap_id_to_uint(cu_ue_id), ue_idx, cu_ue_id, timers);
    return ues[gnb_cu_ue_f1ap_id_to_uint(cu_ue_id)];
  }

  void remove_ue(gnb_cu_ue_f1ap_id_t cu_ue_id) { ues.erase(gnb_cu_ue_f1ap_id_to_uint(cu_ue_id)); }

  size_t size() const { return ues.size(); }

  /// \brief Get the next available GNB-CU-F1AP-UE-ID.
  gnb_cu_ue_f1ap_id_t next_gnb_cu_ue_f1ap_id() const
  {
    unsigned i = ues.find_first_empty(0);
    return i == MAX_NOF_UES ? gnb_cu_ue_f1ap_id_t::invalid : int_to_gnb_cu_ue_f1ap_id(i);
  }

private:
  timer_manager& timers;

  slotted_array<f1ap_ue_context, MAX_NOF_UES> ues;
};

} // namespace srs_cu_cp
} // namespace srsgnb