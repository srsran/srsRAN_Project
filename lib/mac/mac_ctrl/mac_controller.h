
#ifndef SRSGNB_MAC_UE_CTRL_H
#define SRSGNB_MAC_UE_CTRL_H

#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "mac_scheduler_configurator.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/du_ue_list.h"

namespace srsgnb {

struct mac_ue_context {
  du_ue_index_t   du_ue_index = MAX_NOF_DU_UES;
  rnti_t          rnti        = INVALID_RNTI;
  du_cell_index_t pcell_idx   = MAX_NOF_DU_CELLS;
};

class du_rnti_table;

class mac_controller : public mac_ctrl_configurator, public mac_ue_configurator, public mac_cell_manager
{
public:
  mac_controller(mac_common_config_t&        cfg,
                 mac_ul_configurator&        ul_unit_,
                 mac_dl_configurator&        dl_unit_,
                 du_rnti_table&              rnti_table_,
                 mac_scheduler_configurator& sched_cfg_);

  /// Adds new cell configuration to MAC. The configuration is forwarded to the scheduler.
  void add_cell(const mac_cell_creation_request& cell_cfg) override;

  /// Removes cell configuration from MAC. The cell is also removed from the scheduler.
  void remove_cell(du_cell_index_t cell_index) override;

  mac_cell_controller& get_cell_controller(du_cell_index_t cell_index) override;

  /// Creates UE in MAC and scheduler.
  async_task<mac_ue_create_response_message>
  handle_ue_create_request(const mac_ue_create_request_message& msg) override;

  /// Deletes UE from MAC and scheduler.
  async_task<mac_ue_delete_response_message>
  handle_ue_delete_request(const mac_ue_delete_request_message& msg) override;

  /// Reconfigures UE in MAC and scheduler.
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
  /// Adds UE solely in MAC controller.
  bool add_ue(du_ue_index_t ue_index, rnti_t crnti, du_cell_index_t pcell_index) override;

  /// Interface to MAC controller main class used by MAC controller procedures.
  void remove_ue(du_ue_index_t ue_index) override;

  // args
  mac_common_config_t&        cfg;
  srslog::basic_logger&       logger;
  mac_ul_configurator&        ul_unit;
  mac_dl_configurator&        dl_unit;
  du_rnti_table&              rnti_table;
  mac_scheduler_configurator& sched_cfg;

  // UE database
  du_ue_list<mac_ue_context> ue_db;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UE_CTRL_H
