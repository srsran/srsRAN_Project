
#ifndef SRSGNB_MAC_UE_CTRL_H
#define SRSGNB_MAC_UE_CTRL_H

#include "../mac_config.h"
#include "../mac_dl/mac_dl_processor.h"
#include "../mac_ul/mac_ul_processor.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/async_task_loop.h"
#include <queue>

namespace srsgnb {

struct mac_ue_context {
  du_ue_index_t   du_ue_index = MAX_NOF_UES;
  rnti_t          rnti        = INVALID_RNTI;
  du_cell_index_t pcell_idx   = -1;
};

class mac_ctrl_component : public mac_ctrl_configurer
{
public:
  mac_ctrl_component(mac_common_config_t& cfg, mac_ul_configurer& ul_unit_, mac_dl_configurer& dl_unit_);

  /// UE create methods
  async_task<mac_ue_create_response_message> ue_create_request(const mac_ue_create_request_message& msg);

  /// UE deletion methods
  async_task<mac_ue_delete_response_message> ue_delete_request(const mac_ue_delete_request_message& msg);

  /// UE reconfiguration methods
  async_task<mac_ue_reconfiguration_response_message>
  ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg);

  /// Fetch UE context
  mac_ue_context* find_ue(du_ue_index_t ue_index);
  mac_ue_context* find_by_rnti(rnti_t rnti);

private:
  struct ue_element {
    mac_ue_context       ue_ctx;
    async_task_sequencer ctrl_loop{16}; ///< UE Control loop
  };

  bool add_ue(du_ue_index_t ue_index, rnti_t rnti, du_cell_index_t pcell_index) override;

  /// Interface of CTRL procedures to CTRL class
  void remove_ue(du_ue_index_t ue_index) override;

  // args
  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;
  mac_ul_configurer&    ul_unit;
  mac_dl_configurer&    dl_unit;

  // UE database
  slot_array<ue_element, MAX_NOF_UES>    ue_db;
  std::array<du_ue_index_t, MAX_NOF_UES> rnti_to_ue_index_map;

  // CTRL main loop
  async_task_sequencer main_ctrl_loop{64};
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UE_CTRL_H
