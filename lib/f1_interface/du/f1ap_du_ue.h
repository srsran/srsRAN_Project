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

#include "f1c_du_bearer_impl.h"
#include "f1c_ue_context.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/f1_interface/common/f1c_types.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/ran/du_types.h"
#include <mutex>
#include <unordered_map>

namespace srsgnb {
namespace srs_du {

class f1ap_du_ue
{
public:
  f1ap_du_ue(du_ue_index_t         ue_index_,
             gnb_du_ue_f1ap_id_t   gnb_f1_du_ue_id_,
             f1c_du_configurator&  du_handler_,
             f1c_message_notifier& f1c_msg_notifier_) :
    context(ue_index_, gnb_f1_du_ue_id_), du_handler(du_handler_), f1c_msg_notifier(f1c_msg_notifier_)
  {
  }

  f1c_ue_context                          context;
  std::vector<std::unique_ptr<f1_bearer>> bearers;
  f1c_du_configurator&                    du_handler;
  f1c_message_notifier&                   f1c_msg_notifier;

  /// \brief Handles UE CONTEXT SETUP REQUEST as per TS38.473, Section 8.3.1.
  void handle_ue_context_setup_request(const asn1::f1ap::ue_context_setup_request_s& msg);

  /// \brief Handles UE CONTEXT MODIFICATION REQUEST as per TS38.473, Section 8.3.2.
  void handle_ue_context_modification_request(const asn1::f1ap::ue_context_mod_request_s& msg);
};

/// \brief Stores the list of UE contexts currently registered in the F1AP-DU.
class f1ap_du_ue_manager
{
public:
  f1ap_du_ue_manager(f1c_du_configurator& du_handler_, f1c_message_notifier& f1c_msg_notifier_) :
    du_handler(du_handler_), f1c_msg_notifier(f1c_msg_notifier_)
  {
  }

  f1ap_du_ue& add_ue(du_ue_index_t ue_index)
  {
    srsgnb_assert(not ues.contains(ue_index), "Duplicate ueId={} detected", ue_index);

    gnb_du_ue_f1ap_id_t f1ap_id = static_cast<gnb_du_ue_f1ap_id_t>(next_gnb_f1_du_ue_id++);
    ues.emplace(ue_index, ue_index, f1ap_id, du_handler, f1c_msg_notifier);

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
      f1ap_ue_id_to_du_ue_id_map.erase(ues[ue_index].context.gnb_du_ue_f1ap_id);
    }
    ues.erase(ue_index);
  }

  f1ap_du_ue&       operator[](du_ue_index_t ue_index) { return ues[ue_index]; }
  const f1ap_du_ue& operator[](du_ue_index_t ue_index) const { return ues[ue_index]; }

  f1ap_du_ue*       find(du_ue_index_t ue_index) { return ues.contains(ue_index) ? &ues[ue_index] : nullptr; }
  const f1ap_du_ue* find(du_ue_index_t ue_index) const { return ues.contains(ue_index) ? &ues[ue_index] : nullptr; }

  f1ap_du_ue* find(gnb_du_ue_f1ap_id_t du_ue_f1ap_id)
  {
    std::lock_guard<std::mutex> lock(map_mutex);
    auto                        it = f1ap_ue_id_to_du_ue_id_map.find(du_ue_f1ap_id);
    return it != f1ap_ue_id_to_du_ue_id_map.end() ? find(it->second) : nullptr;
  }
  const f1ap_du_ue* find(gnb_du_ue_f1ap_id_t du_ue_f1ap_id) const
  {
    std::lock_guard<std::mutex> lock(map_mutex);
    auto                        it = f1ap_ue_id_to_du_ue_id_map.find(du_ue_f1ap_id);
    return it != f1ap_ue_id_to_du_ue_id_map.end() ? find(it->second) : nullptr;
  }

  bool contains(du_ue_index_t ue_index) const { return find(ue_index) != nullptr; }
  bool contains(gnb_du_ue_f1ap_id_t du_ue_f1ap_id) const { return find(du_ue_f1ap_id) != nullptr; }

  du_ue_index_t get_ue_index(gnb_du_ue_f1ap_id_t du_ue_f1ap_id) const
  {
    std::lock_guard<std::mutex> lock(map_mutex);
    auto                        it = f1ap_ue_id_to_du_ue_id_map.find(du_ue_f1ap_id);
    return it == f1ap_ue_id_to_du_ue_id_map.end() ? INVALID_DU_UE_INDEX : it->second;
  }

private:
  f1c_du_configurator&  du_handler;
  f1c_message_notifier& f1c_msg_notifier;

  uint64_t next_gnb_f1_du_ue_id = 0;

  slot_array<f1ap_du_ue, MAX_NOF_DU_UES> ues;

  mutable std::mutex                                     map_mutex;
  std::unordered_map<gnb_du_ue_f1ap_id_t, du_ue_index_t> f1ap_ue_id_to_du_ue_id_map;
};

} // namespace srs_du
} // namespace srsgnb
