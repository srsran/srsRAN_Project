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

#include "srsgnb/f1_interface/common/f1c_types.h"
#include "srsgnb/ran/du_types.h"
#include <unordered_map>

namespace srsgnb {
namespace srs_du {

class f1ap_du_ue
{
public:
  f1ap_du_ue(du_ue_index_t ue_index_, gnb_du_ue_f1ap_id_t gnb_f1_du_ue_id_) :
    ue_index(ue_index_), gnb_du_ue_f1ap_id(gnb_f1_du_ue_id_)
  {
  }

  const du_ue_index_t       ue_index;
  const gnb_du_ue_f1ap_id_t gnb_du_ue_f1ap_id;
  optional<uint64_t>        gnb_cu_ue_f1ap_id;
};

/// \brief Stores the list of UE contexts currently registered in the F1AP-DU.
class f1ap_du_ue_manager
{
public:
  f1ap_du_ue& add_ue(du_ue_index_t ue_index)
  {
    srsgnb_assert(not ues.contains(ue_index), "Duplicate ueId={} detected", ue_index);

    gnb_du_ue_f1ap_id_t f1ap_id = static_cast<gnb_du_ue_f1ap_id_t>(next_gnb_f1_du_ue_id++);
    ues.emplace(ue_index, ue_index, f1ap_id);

    {
      std::lock_guard<std::mutex> lock(map_mutex);
      f1ap_ue_id_to_du_ue_id_map[f1ap_id] = ue_index;
    }

    return ues[ue_index];
  }

  void remove_ue(du_ue_index_t ue_index)
  {
    srsgnb_assert(ues.contains(ue_index), "ueId={} does not exist", ue_index);
    {
      std::lock_guard<std::mutex> lock(map_mutex);
      f1ap_ue_id_to_du_ue_id_map.erase(ues[ue_index].gnb_du_ue_f1ap_id);
    }
    ues.erase(ue_index);
  }

  f1ap_du_ue&       operator[](du_ue_index_t ue_index) { return ues[ue_index]; }
  const f1ap_du_ue& operator[](du_ue_index_t ue_index) const { return ues[ue_index]; }

  du_ue_index_t get_ue_index(gnb_du_ue_f1ap_id_t du_ue_f1ap_id) const
  {
    std::lock_guard<std::mutex> lock(map_mutex);
    auto                        it = f1ap_ue_id_to_du_ue_id_map.find(du_ue_f1ap_id);
    return it == f1ap_ue_id_to_du_ue_id_map.end() ? INVALID_DU_UE_INDEX : it->second;
  }

private:
  uint64_t next_gnb_f1_du_ue_id = 0;

  slot_array<f1ap_du_ue, MAX_NOF_DU_UES> ues;

  mutable std::mutex                                     map_mutex;
  std::unordered_map<gnb_du_ue_f1ap_id_t, du_ue_index_t> f1ap_ue_id_to_du_ue_id_map;
};

} // namespace srs_du
} // namespace srsgnb
