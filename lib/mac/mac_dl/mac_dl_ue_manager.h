/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_MAC_DL_UE_MANAGER_H
#define SRSGNB_MAC_DL_UE_MANAGER_H

#include "../du_rnti_table.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/du_ue_list.h"
#include <mutex>

namespace srsgnb {

class mac_dl_ue_manager
{
public:
  mac_dl_ue_manager(du_rnti_table& rnti_table_) : rnti_table(rnti_table_) {}

  /// Check if UE with provided C-RNTI exists.
  /// \param rnti C-RNTI of the UE.
  /// \return True if UE exists. False, otherwise.
  bool contains_rnti(rnti_t rnti) const
  {
    du_ue_index_t ue_index = rnti_table[rnti];
    if (is_du_ue_index_valid(ue_index)) {
      std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
      return ue_db.contains(ue_index);
    }
    return false;
  }

  /// Checks if bearer with provided C-RNTI and LCID exists.
  bool contains_lcid(rnti_t rnti, lcid_t lcid) const
  {
    du_ue_index_t ue_index = rnti_table[rnti];
    if (is_du_ue_index_valid(ue_index)) {
      std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
      return ue_db.contains(ue_index) and ue_db[ue_index].dl_bearers.contains(lcid);
    }
    return false;
  }

  mac_sdu_tx_builder* get_bearer(rnti_t rnti, lcid_t lcid)
  {
    du_ue_index_t ue_index = rnti_table[rnti];
    if (not is_du_ue_index_valid(ue_index)) {
      return nullptr;
    }
    std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
    if (not ue_db.contains(ue_index)) {
      return nullptr;
    }
    ue_item& u = ue_db[ue_index];
    return u.dl_bearers.contains(lcid) ? u.dl_bearers[lcid] : nullptr;
  }

  bool add_ue(const mac_ue_create_request_message& request)
  {
    std::lock_guard<std::mutex> lock(ue_mutex[request.ue_index]);
    return add_ue_nolock(request.ue_index, request.crnti, request.ul_ccch_msg) and
           addmod_bearers_nolock(request.ue_index, request.bearers);
  }

  bool remove_ue(du_ue_index_t ue_index)
  {
    std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
    if (not ue_db.contains(ue_index)) {
      return false;
    }
    ue_db.erase(ue_index);
    return true;
  }

  bool addmod_bearers(du_ue_index_t ue_index, span<const mac_logical_channel_addmod> dl_logical_channels)
  {
    std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
    return addmod_bearers_nolock(ue_index, dl_logical_channels);
  }

  bool remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids)
  {
    std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
    if (not ue_db.contains(ue_index)) {
      return false;
    }
    auto& u = ue_db[ue_index];
    for (lcid_t lcid : lcids) {
      u.dl_bearers.erase(lcid);
    }
    return true;
  }

  /// Returns received Msg3 bytes. In case of ConRes CE, the packed Msg3 is used for disambiguation.
  byte_buffer pop_msg3(rnti_t rnti)
  {
    du_ue_index_t ue_index = rnti_table[rnti];
    if (not ue_db.contains(ue_index)) {
      return {};
    }
    return std::move(ue_db[ue_index].msg3_subpdu);
  }

private:
  struct ue_item {
    rnti_t                           rnti     = INVALID_RNTI;
    du_ue_index_t                    ue_index = MAX_NOF_DU_UES;
    slot_vector<mac_sdu_tx_builder*> dl_bearers;
    byte_buffer                      msg3_subpdu;
  };

  bool add_ue_nolock(du_ue_index_t ue_index, rnti_t crnti, const byte_buffer* ul_ccch_msg)
  {
    if (ue_db.contains(ue_index)) {
      return false;
    }
    ue_db.emplace(ue_index);
    auto& u    = ue_db[ue_index];
    u.ue_index = ue_index;
    u.rnti     = crnti;
    if (ul_ccch_msg != nullptr) {
      // Store Msg3.
      u.msg3_subpdu = ul_ccch_msg->copy();
    }
    return true;
  }

  bool addmod_bearers_nolock(du_ue_index_t ue_index, span<const mac_logical_channel_addmod> dl_logical_channels)
  {
    if (not ue_db.contains(ue_index)) {
      return false;
    }
    auto& u = ue_db[ue_index];
    for (const mac_logical_channel_addmod& lc : dl_logical_channels) {
      u.dl_bearers.insert(lc.lcid, lc.dl_bearer);
    }
    return true;
  }

  du_rnti_table& rnti_table;

  mutable std::array<std::mutex, MAX_NOF_DU_UES> ue_mutex;

  du_ue_list<ue_item> ue_db;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_DL_UE_MANAGER_H
