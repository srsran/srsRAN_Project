
#ifndef SRSGNB_DEMUX_H
#define SRSGNB_DEMUX_H

#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/support/srsran_assert.h"
#include <tuple>

namespace srsgnb {

class mac_ul_ue
{
public:
  du_ue_index_t                     ue_index = MAX_NOF_UES;
  rnti_t                            rnti     = INVALID_RNTI;
  slot_vector<mac_ul_sdu_notifier*> ul_bearers;
};

class mac_ul_demux
{
public:
  mac_ul_demux(mac_ul_sdu_notifier& ccch_notifier_) : ccch_notifier(ccch_notifier_) {}

  /// Check if UE exists
  /// \param ue_index DU UE Index
  /// \return True if UE exists. False, otherwise
  bool contains_ue(du_ue_index_t ue_index) const
  {
    srsran_assert(ue_index < MAX_NOF_UES, "Invalid ueId=%d", ue_index);
    return ue_db.contains(ue_index);
  }
  bool contains_lcid(du_ue_index_t ue_index, lcid_t lcid) const
  {
    return contains_ue(ue_index) and ue_db[ue_index].ul_bearers.contains(lcid);
  }
  bool contains_rnti(rnti_t rnti) const { return rnti_to_ue_index.contains(rnti); }

  bool insert(du_ue_index_t ue_index, rnti_t crnti)
  {
    srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);
    if (contains_ue(ue_index) or contains_rnti(crnti)) {
      return false;
    }
    ue_db.emplace(ue_index);
    ue_db[ue_index].ue_index = ue_index;
    ue_db[ue_index].rnti     = crnti;
    rnti_to_ue_index[crnti]  = ue_index;
    return true;
  }

  bool erase(du_ue_index_t ue_index)
  {
    srsran_assert(ue_index < MAX_NOF_UES, "Invalid ueId=%d", ue_index);
    if (not contains_ue(ue_index)) {
      return false;
    }
    rnti_to_ue_index[ue_db[ue_index].rnti] = MAX_NOF_UES;
    ue_db.erase(ue_index);
    return true;
  }

  mac_ul_ue* get_ue(du_ue_index_t ue_index)
  {
    if (not contains_ue(ue_index)) {
      return nullptr;
    }
    return &ue_db[ue_index];
  }

  std::tuple<du_ue_index_t, mac_ul_sdu_notifier*> find_ul_bearer(rnti_t rnti, lcid_t lcid)
  {
    if (lcid == 0) {
      return std::make_tuple(MAX_NOF_UES, &ccch_notifier);
    }
    if (contains_rnti(rnti)) {
      return std::make_tuple(0, nullptr);
    }
    mac_ul_ue* ent = &ue_db[rnti_to_ue_index[rnti]];
    if (contains_lcid(ent->ue_index, lcid)) {
      return std::make_tuple(0, nullptr);
    }
    return std::make_tuple(ent->ue_index, ent->ul_bearers[lcid]);
  }

private:
  mac_ul_sdu_notifier&                             ccch_notifier; /// upper layer handler of UL CCCH messages
  slot_array<mac_ul_ue, MAX_NOF_UES>               ue_db;
  circular_map<rnti_t, du_ue_index_t, MAX_NOF_UES> rnti_to_ue_index;
};

} // namespace srsgnb

#endif // SRSGNB_DEMUX_H
