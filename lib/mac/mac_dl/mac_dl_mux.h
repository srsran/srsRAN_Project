
#ifndef SRSGNB_MAC_DL_MUX_H
#define SRSGNB_MAC_DL_MUX_H

#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

class mac_dl_mux
{
public:
  /// Check if UE exists
  /// \param ue_index DU UE Index
  /// \return True if UE exists. False, otherwise
  bool contains_ue(du_ue_index_t ue_index) const
  {
    srsran_sanity_check(ue_index < MAX_NOF_UES, "Invalid ueId=%d", ue_index);
    return ue_db.contains(ue_index);
  }
  bool contains_lcid(du_ue_index_t ue_index, lcid_t lcid) const
  {
    return contains_ue(ue_index) and ue_db[ue_index].dl_bearers.contains(lcid);
  }
  bool contains_rnti(rnti_t rnti) const { return rnti_to_ue_index.contains(rnti); }

  rnti_t get_rnti(du_ue_index_t ue_index) const { return contains_ue(ue_index) ? ue_db[ue_index].rnti : INVALID_RNTI; }

  bool add_ue(du_ue_index_t ue_index, rnti_t crnti, span<const logical_channel_addmod> bearers)
  {
    srsran_sanity_check(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);
    srsran_sanity_check(crnti != INVALID_RNTI, "Invalid RNTI");
    if (contains_ue(ue_index) or contains_rnti(crnti)) {
      return false;
    }
    ue_db.emplace(ue_index);
    ue_db[ue_index].ue_index = ue_index;
    ue_db[ue_index].rnti     = crnti;
    for (const logical_channel_addmod& l : bearers) {
      ue_db[ue_index].dl_bearers.insert(l.lcid, l.dl_bearer);
    }

    rnti_to_ue_index.insert(crnti, ue_index);
    return true;
  }

  bool remove_ue(du_ue_index_t ue_index)
  {
    srsran_sanity_check(ue_index < MAX_NOF_UES, "Invalid ueId=%d", ue_index);
    if (not contains_ue(ue_index)) {
      return false;
    }
    rnti_to_ue_index[ue_db[ue_index].rnti] = MAX_NOF_UES;
    ue_db.erase(ue_index);
    return true;
  }

  bool addmod_bearers(du_ue_index_t ue_index, span<const logical_channel_addmod> dl_logical_channels)
  {
    srsran_sanity_check(ue_index < MAX_NOF_UES, "Invalid ueId=%d", ue_index);
    if (not contains_ue(ue_index)) {
      return false;
    }
    auto& u = ue_db[ue_index];
    for (const logical_channel_addmod& lc : dl_logical_channels) {
      u.dl_bearers.insert(lc.lcid, lc.dl_bearer);
    }
    return true;
  }

  bool remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids)
  {
    srsran_sanity_check(ue_index < MAX_NOF_UES, "Invalid ueId=%d", ue_index);
    if (not contains_ue(ue_index)) {
      return false;
    }
    auto& u = ue_db[ue_index];
    for (lcid_t lcid : lcids) {
      u.dl_bearers.erase(lcid);
    }
    return true;
  }

private:
  class ue_item
  {
  public:
    du_ue_index_t                    ue_index = MAX_NOF_UES;
    rnti_t                           rnti     = INVALID_RNTI;
    slot_vector<mac_dl_sdu_builder*> dl_bearers;
  };

  slot_array<ue_item, MAX_NOF_UES>                 ue_db;
  circular_map<rnti_t, du_ue_index_t, MAX_NOF_UES> rnti_to_ue_index;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_DL_MUX_H
