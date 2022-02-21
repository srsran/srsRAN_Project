
#ifndef SRSGNB_MAC_UE_CTRL_H
#define SRSGNB_MAC_UE_CTRL_H

#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/manual_event.h"

namespace srsgnb {

struct mac_ue_ctrl {
  du_cell_index_t pcell_idx   = -1;
  du_ue_index_t   du_ue_index = MAX_NOF_UES;
  rnti_t          rnti        = INVALID_RNTI;

  manual_event_flag ul_ue_create_response_ev;
  manual_event_flag sched_ue_create_response_ev;

  async_task<void> ue_create_proc;
};

/// Class to manage MAC UE control/metric context
class mac_ue_map
{
public:
  mac_ue_map();

  bool insert(du_ue_index_t ue_index, rnti_t crnti, du_cell_index_t cell_index);
  bool erase(du_ue_index_t ue_index);

  mac_ue_ctrl* find(du_ue_index_t ue_index);
  mac_ue_ctrl* find_by_rnti(rnti_t rnti);

private:
  std::array<optional<mac_ue_ctrl>, MAX_NOF_UES> ue_db;
  std::array<du_ue_index_t, MAX_NOF_UES>         rnti_to_ue_index_map;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UE_CTRL_H
