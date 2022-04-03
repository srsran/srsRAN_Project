
#ifndef SRSGNB_MAC_DL_UE_MANAGER_H
#define SRSGNB_MAC_DL_UE_MANAGER_H

#include "srsgnb/adt/circular_array.h"
#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti_map.h"
#include <mutex>

namespace srsgnb {

class mac_dl_ue_manager
{
public:
  /// Check if UE with provided C-RNTI exists.
  /// \param rnti C-RNTI of the UE.
  /// \return True if UE exists. False, otherwise.
  bool contains(rnti_t rnti) const
  {
    std::lock_guard<std::mutex> lock(rnti_mutex[rnti]);
    return ue_db.contains(rnti);
  }

  /// Checks if bearer with provided C-RNTI and LCID exists.
  bool contains_lcid(rnti_t rnti, lcid_t lcid) const
  {
    std::lock_guard<std::mutex> lock(rnti_mutex[rnti]);
    return ue_db.contains(rnti) and ue_db[rnti].dl_bearers.contains(lcid);
  }

  mac_sdu_tx_builder* get_bearer(rnti_t rnti, lcid_t lcid)
  {
    std::lock_guard<std::mutex> lock(rnti_mutex[rnti]);
    if (not ue_db.contains(rnti)) {
      return nullptr;
    }
    ue_item& u = ue_db[rnti];
    return u.dl_bearers.contains(lcid) ? u.dl_bearers[lcid] : nullptr;
  }

  bool add_ue(rnti_t crnti, du_ue_index_t ue_index, span<const logical_channel_addmod> bearers)
  {
    std::lock_guard<std::mutex> lock(rnti_mutex[crnti]);
    if (ue_db.contains(crnti)) {
      return false;
    }
    ue_db.emplace(crnti);
    auto& u    = ue_db[crnti];
    u.rnti     = crnti;
    u.ue_index = ue_index;
    for (const logical_channel_addmod& l : bearers) {
      u.dl_bearers.insert(l.lcid, l.dl_bearer);
    }
    return true;
  }

  bool remove_ue(rnti_t rnti)
  {
    std::lock_guard<std::mutex> lock(rnti_mutex[rnti]);
    if (not ue_db.contains(rnti)) {
      return false;
    }
    ue_db.erase(rnti);
    return true;
  }

  bool addmod_bearers(rnti_t rnti, span<const logical_channel_addmod> dl_logical_channels)
  {
    std::lock_guard<std::mutex> lock(rnti_mutex[rnti]);
    if (not ue_db.contains(rnti)) {
      return false;
    }
    auto& u = ue_db[rnti];
    for (const logical_channel_addmod& lc : dl_logical_channels) {
      u.dl_bearers.insert(lc.lcid, lc.dl_bearer);
    }
    return true;
  }

  bool remove_bearers(rnti_t rnti, span<const lcid_t> lcids)
  {
    std::lock_guard<std::mutex> lock(rnti_mutex[rnti]);
    if (not ue_db.contains(rnti)) {
      return false;
    }
    auto& u = ue_db[rnti];
    for (lcid_t lcid : lcids) {
      u.dl_bearers.erase(lcid);
    }
    return true;
  }

private:
  struct ue_item {
    rnti_t                           rnti     = INVALID_RNTI;
    du_ue_index_t                    ue_index = MAX_NOF_UES;
    slot_vector<mac_sdu_tx_builder*> dl_bearers;
  };

  rnti_map<ue_item> ue_db;

  mutable circular_array<std::mutex, MAX_NOF_UES> rnti_mutex;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_DL_UE_MANAGER_H
