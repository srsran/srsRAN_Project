
#ifndef SRSGNB_MAC_DL_MUX_H
#define SRSGNB_MAC_DL_MUX_H

#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {

class mac_dl_mux
{
public:
  /// Check if UE with provided C-RNTI exists.
  /// \param rnti C-RNTI of the UE.
  /// \return True if UE exists. False, otherwise.
  bool contains(rnti_t rnti) const { return ue_db.contains(rnti); }

  /// Checks if bearer with provided C-RNTI and LCID exists.
  bool contains_lcid(rnti_t rnti, lcid_t lcid) const
  {
    return contains(rnti) and ue_db[rnti].dl_bearers.contains(lcid);
  }

  mac_sdu_tx_builder* get_bearer(rnti_t rnti, lcid_t lcid)
  {
    if (not contains(rnti)) {
      return nullptr;
    }
    ue_item& u = ue_db[rnti];
    return u.dl_bearers.contains(lcid) ? u.dl_bearers[lcid] : nullptr;
  }

  bool add_ue(rnti_t crnti, du_ue_index_t ue_index, span<const logical_channel_addmod> bearers)
  {
    srsran_sanity_check(crnti != INVALID_RNTI, "Invalid RNTI");
    if (contains(crnti)) {
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
    if (not contains(rnti)) {
      return false;
    }
    ue_db.erase(rnti);
    return true;
  }

  bool addmod_bearers(rnti_t rnti, span<const logical_channel_addmod> dl_logical_channels)
  {
    if (not contains(rnti)) {
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
    if (not contains(rnti)) {
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
};

} // namespace srsgnb

#endif // SRSGNB_MAC_DL_MUX_H
