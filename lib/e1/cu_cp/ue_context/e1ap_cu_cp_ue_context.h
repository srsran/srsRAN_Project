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

#include "srsgnb/adt/slotted_array.h"

namespace srsgnb {
namespace srs_cu_cp {

struct e1ap_ue_context {
  cu_cp_ue_id_t          cu_cp_ue_id      = cu_cp_ue_id_t::invalid;
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::invalid;
  gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = gnb_cu_up_ue_e1ap_id_t::invalid;

  e1ap_ue_context(cu_cp_ue_id_t cu_cp_ue_id_, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id_) :
    cu_cp_ue_id(cu_cp_ue_id_), cu_cp_ue_e1ap_id(cu_cp_ue_e1ap_id_)
  {
  }
};

class e1ap_ue_context_list
{
public:
  e1ap_ue_context_list() {}

  bool contains(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id) const
  {
    return ues.contains(gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id));
  }

  /// \brief Checks whether a UE with the given UE index exists.
  /// \param[in] cu_cp_ue_id The UE index used to find the UE.
  /// \return The CU CP UE ID.
  bool contains(cu_cp_ue_id_t cu_cp_ue_id) const
  {
    return std::any_of(
        ues.begin(), ues.end(), [cu_cp_ue_id](const e1ap_ue_context& u) { return u.cu_cp_ue_id == cu_cp_ue_id; });
  }

  e1ap_ue_context& operator[](gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_id)
  {
    return ues[gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_id)];
  }
  const e1ap_ue_context& operator[](gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_id) const
  {
    return ues[gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_id)];
  }
  e1ap_ue_context& operator[](cu_cp_ue_id_t cu_cp_ue_id)
  {
    auto it = std::find_if(
        ues.begin(), ues.end(), [cu_cp_ue_id](const e1ap_ue_context& u) { return u.cu_cp_ue_id == cu_cp_ue_id; });
    return *it;
  }

  e1ap_ue_context& add_ue(cu_cp_ue_id_t cu_cp_ue_id, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id)
  {
    ues.emplace(gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id), cu_cp_ue_id, cu_cp_ue_e1ap_id);
    return ues[gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_e1ap_id)];
  }

  void remove_ue(gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_id) { ues.erase(gnb_cu_cp_ue_e1ap_id_to_uint(cu_cp_ue_id)); }

  size_t size() const { return ues.size(); }

  /// \brief Get the next available GNB-CU-CP-E1AP-UE-ID.
  gnb_cu_cp_ue_e1ap_id_t next_gnb_cu_cp_ue_e1ap_id() const
  {
    unsigned i = ues.find_first_empty(0);
    return i == MAX_NOF_UES ? gnb_cu_cp_ue_e1ap_id_t::invalid : int_to_gnb_cu_cp_ue_e1ap_id(i);
  }

private:
  slotted_array<e1ap_ue_context, MAX_NOF_CU_UES> ues; // indexed by gnb_cu_cp_ue_e1ap_id
};

} // namespace srs_cu_cp
} // namespace srsgnb