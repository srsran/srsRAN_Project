
#ifndef SRSGNB_MAC_UE_CTRL_H
#define SRSGNB_MAC_UE_CTRL_H

#include "../mac_config.h"
#include "../mac_dl/mac_dl_processor.h"
#include "../mac_ul/mac_ul_processor.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/async/eager_async_task.h"

namespace srsgnb {

struct mac_ue_context {
  du_ue_index_t   du_ue_index = MAX_NOF_UES;
  rnti_t          rnti        = INVALID_RNTI;
  du_cell_index_t pcell_idx   = -1;
};

class du_rnti_table;

class mac_controller : public mac_ctrl_configurer, public mac_ue_configurator
{
public:
  mac_controller(mac_common_config_t& cfg,
                 mac_ul_configurer&   ul_unit_,
                 mac_dl_configurer&   dl_unit_,
                 du_rnti_table&       rnti_table);

  /// UE create methods
  async_task<mac_ue_create_response_message>
  handle_ue_create_request(const mac_ue_create_request_message& msg) override;

  /// UE deletion methods
  async_task<mac_ue_delete_response_message>
  handle_ue_delete_request(const mac_ue_delete_request_message& msg) override;

  /// UE reconfiguration methods
  async_task<mac_ue_reconfiguration_response_message>
  handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg) override;

  void handle_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu) override
  {
    ul_unit.flush_ul_ccch_msg(ue_index, std::move(pdu));
  }

  /// Fetch UE context
  mac_ue_context* find_ue(du_ue_index_t ue_index);
  mac_ue_context* find_by_rnti(rnti_t rnti);

private:
  bool add_ue(du_ue_index_t ue_index, rnti_t crnti, du_cell_index_t pcell_index) override;

  /// Interface of CTRL procedures to CTRL class
  void remove_ue(du_ue_index_t ue_index) override;

  // args
  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;
  mac_ul_configurer&    ul_unit;
  mac_dl_configurer&    dl_unit;
  du_rnti_table&        rnti_table;

  // UE database
  du_ue_list<mac_ue_context> ue_db;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UE_CTRL_H
