
#include "mac_impl.h"
#include "sched/sched.h"

namespace srsgnb {

mac_impl::mac_impl(mac_event_notifier&       event_notifier,
                   du_l2_ul_executor_mapper& ul_exec_mapper_,
                   span<task_executor*>      dl_execs_,
                   task_executor&            ctrl_exec_,
                   mac_result_notifier&      phy_notifier_) :
  cfg(event_notifier, ul_exec_mapper_, dl_execs_, ctrl_exec_, phy_notifier_),
  logger(cfg.logger),
  sched_obj(std::make_unique<sched>(sched_cfg_notif.get_notifier())),
  dl_unit(cfg, sched_cfg_notif, *sched_obj, rnti_table),
  ul_unit(cfg, *sched_obj, rnti_table),
  ctrl_unit(cfg, ul_unit, dl_unit, rnti_table),
  rach_hdl(*sched_obj, rnti_table)
{
  // :TODO: remove when the log is used.
  (void)(logger);
}

void mac_impl::add_cell(const mac_cell_configuration& cell_cfg)
{
  dl_unit.add_cell(cell_cfg);
}

void mac_impl::remove_cell(du_cell_index_t cell_index)
{
  dl_unit.remove_cell(cell_index);
}

void mac_impl::handle_rx_data_indication(mac_rx_data_indication msg)
{
  ul_unit.handle_rx_data_indication(msg);
}

async_task<mac_ue_create_response_message> mac_impl::ue_create_request(const mac_ue_create_request_message& msg)
{
  return ctrl_unit.ue_create_request(msg);
}

async_task<mac_ue_reconfiguration_response_message>
mac_impl::ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg)
{
  return ctrl_unit.ue_reconfiguration_request(msg);
}

async_task<mac_ue_delete_response_message> mac_impl::ue_delete_request(const mac_ue_delete_request_message& msg)
{
  return ctrl_unit.ue_delete_request(msg);
}

void mac_impl::flush_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer ccch_pdu)
{
  ul_unit.flush_ul_ccch_msg(ue_index, std::move(ccch_pdu));
}

} // namespace srsgnb
