
#ifndef SRSGNB_MAC_UE_CTRL_H
#define SRSGNB_MAC_UE_CTRL_H

#include "../mac_config.h"
#include "../mac_dl/mac_dl_component.h"
#include "../mac_ul/mac_ul_component.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/adt/slot_array.h"
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

class mac_ctrl_component
{
public:
  mac_ctrl_component(mac_common_config_t& cfg, mac_ul_component& ul_unit_, mac_dl_component& dl_unit_);

  /// UE create methods
  void ue_create_request(const mac_ue_create_request_message& msg);

  /// UE deletion methods
  void ue_delete_request(const mac_ue_delete_request_message& msg);

  /// Fetch UE context
  mac_ue_context* find_ue(du_ue_index_t ue_index);
  mac_ue_context* find_by_rnti(rnti_t rnti);

private:
  struct ue_element {
    mac_ue_context ue_ctx;

    async_task<void>                                 ctrl_loop; ///< UE Control loop
    std::queue<unique_function<async_task<void>()> > pending_events;
    manual_event_flag                                notify_event; // TODO: Make notification automatic
  };

  ue_element* add_ue(du_ue_index_t ue_index, rnti_t crnti, du_cell_index_t cell_index);
  bool        remove_ue(du_ue_index_t ue_index);

  /// Launches task that handles incoming UE events
  static void launch_ue_ctrl_loop(ue_element& u);

  // args
  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;
  mac_ul_component&     ul_unit;
  mac_dl_component&     dl_unit;

  // UE database
  slot_array<ue_element, MAX_NOF_UES>    ue_db;
  std::array<du_ue_index_t, MAX_NOF_UES> rnti_to_ue_index_map;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UE_CTRL_H
