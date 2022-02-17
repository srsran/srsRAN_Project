
#ifndef SRSGNB_MAC_LOGICAL_CHANNEL_H
#define SRSGNB_MAC_LOGICAL_CHANNEL_H

#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/support/srsran_assert.h"
#include <array>

namespace srsgnb {

class dl_dcch
{
public:
  const lcid_t lcid;
  const void*  rlc_dl_entity;

  dl_dcch(lcid_t lcid_) : lcid(lcid_) {}
};

class ul_dcch
{
public:
  const lcid_t lcid;
  const void*  ul_lc_ch_notifier;

  ul_dcch(lcid_t lcid_) : lcid(lcid_) {}
};

template <typename LogicalChannel>
class mac_dcch_channel_repository
{
  const static du_ue_index_t INVALID_UE_INDEX = std::numeric_limits<du_ue_index_t>::max();

public:
  bool contains_ue(du_ue_index_t ue_index) const
  {
    srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);
    return ue_db.contains(ue_index);
  }
  bool contains_lcid(du_ue_index_t ue_index, lcid_t lcid) const
  {
    return contains_ue(ue_index) and ue_db[ue_index].logical_channels.contains(lcid);
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
    ue_db[ue_index].logical_channels.emplace(0, 0); // TODO: fill
    rnti_to_ue_index[crnti] = ue_index;
    return true;
  }

  bool erase(du_ue_index_t ue_index)
  {
    srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);
    if (not contains_ue(ue_index)) {
      return false;
    }
    rnti_to_ue_index[ue_db[ue_index].rnti] = INVALID_UE_INDEX;
    ue_db.erase(ue_index);
    return true;
  }

  LogicalChannel* get_rnti_lc_ch(rnti_t rnti, lcid_t lcid)
  {
    if (contains_rnti(rnti)) {
      return nullptr;
    }
    mac_entity* ent = &ue_db[rnti_to_ue_index[rnti]];
    if (contains_lcid(ent->ue_index, lcid)) {
      return nullptr;
    }
    return &ent->logical_channels[lcid];
  }

private:
  struct mac_entity {
    du_ue_index_t                        ue_index = INVALID_UE_INDEX;
    rnti_t                               rnti     = INVALID_RNTI;
    slot_array<LogicalChannel, MAX_LCID> logical_channels;
  };

  slot_array<mac_entity, MAX_NOF_UES>              ue_db;
  circular_map<rnti_t, du_ue_index_t, MAX_NOF_UES> rnti_to_ue_index;
};

using mac_dl_dcch_manager = mac_dcch_channel_repository<dl_dcch>;
using mac_ul_dcch_manager = mac_dcch_channel_repository<ul_dcch>;

} // namespace srsgnb

#endif // SRSGNB_MAC_LOGICAL_CHANNEL_H
