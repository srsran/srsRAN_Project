
#ifndef SRSGNB_MAC_IMPL_H
#define SRSGNB_MAC_IMPL_H

#include "mac_config.h"
#include "mac_ctrl/mac_controller.h"
#include "mac_dl/sched_config_adapter.h"
#include "mac_ul/mac_ul_processor.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

class mac_impl : public mac_interface
{
public:
  explicit mac_impl(mac_event_notifier&       event_notifier,
                    du_l2_ul_executor_mapper& ul_exec_mapper,
                    span<task_executor*>      dl_execs,
                    task_executor&            ctrl_exec);

  void add_cell(const mac_cell_configuration& cell_cfg) override;
  void remove_cell(du_cell_index_t cell_index) override;

  async_task<mac_ue_create_response_message> ue_create_request(const mac_ue_create_request_message& cfg) override;
  async_task<mac_ue_reconfiguration_response_message>
                                             ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg) override;
  async_task<mac_ue_delete_response_message> ue_delete_request(const mac_ue_delete_request_message& cfg) override;
  void                                       flush_ul_ccch_msg(rnti_t rnti) override;

  void handle_rx_data_indication(mac_rx_data_indication msg) override;

  void handle_crc(const crc_indication_message& msg) override {}

  mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) override
  {
    return dl_unit.get_slot_handler(cell_index);
  }

  mac_cell_manager& get_cell_manager(du_cell_index_t cell_index) override
  {
    return dl_unit.get_cell_manager(cell_index);
  }

private:
  mac_common_config_t   cfg;
  srslog::basic_logger& logger;

  /// Handle used to await scheduler configurations.
  sched_config_adapter sched_cfg_notif;

  /// MAC Scheduler.
  std::unique_ptr<sched_interface> sched_obj;

  mac_dl_processor dl_unit;
  mac_ul_processor ul_unit;
  mac_controller   ctrl_unit;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_IMPL_H
