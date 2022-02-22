
#ifndef SRSGNB_MAC_UE_CTRL_H
#define SRSGNB_MAC_UE_CTRL_H

#include "mac_ctxt.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/manual_event.h"
#include <queue>

namespace srsgnb {

struct mac_ue_context {
  du_ue_index_t   du_ue_index = MAX_NOF_UES;
  rnti_t          rnti        = INVALID_RNTI;
  du_cell_index_t pcell_idx   = -1;
};

class mac_ctrl_worker
{
public:
  mac_ctrl_worker(mac_context& ctx_);

  /// UE create methods
  void ue_create_request(const mac_ue_create_request_message& msg);
  void sched_ue_create_response(rnti_t rnti);

  /// UE deletion methods
  void ue_delete_request(const mac_ue_delete_request_message& msg);
  void sched_ue_delete_response(rnti_t rnti);

  /// Fetch UE context
  mac_ue_context* find_ue(du_ue_index_t ue_index);
  mac_ue_context* find_by_rnti(rnti_t rnti);

private:
  struct ue_element {
    mac_ue_context ue_ctx;

    /// Events
    manual_event_flag ul_ue_create_response_ev;
    manual_event_flag sched_response_ev;

    async_task<void>                                 ctrl_loop; ///< UE Control loop
    std::queue<unique_function<async_task<void>()> > pending_events;
    manual_event_flag                                notify_event; // TODO: Make notification automatic
  };

  ue_element* add_ue(du_ue_index_t ue_index, rnti_t crnti, du_cell_index_t cell_index);
  bool        remove_ue(du_ue_index_t ue_index);

  /// Launches task that handles incoming UE events
  static void launch_ue_ctrl_loop(ue_element& u);

  // args
  mac_context&          mac_ctx;
  srslog::basic_logger& logger;

  // UE database
  slot_array<ue_element, MAX_NOF_UES>    ue_db;
  std::array<du_ue_index_t, MAX_NOF_UES> rnti_to_ue_index_map;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UE_CTRL_H
