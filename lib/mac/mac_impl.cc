
#include "mac_impl.h"
#include <mutex>

namespace srsgnb {

mac_impl::mac_impl(mac_event_notifier&       event_notifier,
                   du_l2_ul_executor_mapper& ul_exec_mapper_,
                   span<task_executor*>      dl_execs_,
                   task_executor&            ctrl_exec_) :
  cfg(event_notifier, ul_exec_mapper_, dl_execs_, ctrl_exec_),
  logger(cfg.logger),
  dl_unit(cfg),
  ul_unit(cfg, dl_unit.get_sched()),
  ctrl_unit(cfg, ul_unit, dl_unit)
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

void mac_impl::start_cell(du_cell_index_t cell_index)
{
  dl_unit.start_cell(cell_index);
}

void mac_impl::stop_cell(du_cell_index_t cell_index)
{
  dl_unit.stop_cell(cell_index);
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

void mac_impl::flush_ul_ccch_msg(rnti_t rnti)
{
  ul_unit.flush_ul_ccch_msg(rnti);
}

void mac_impl::slot_indication(slot_point sl_tx, du_cell_index_t cell_index)
{
  dl_unit.slot_indication(sl_tx, cell_index);

  // for each cc, generate MAC DL SDUs
  mac_ue_context* u = ctrl_unit.find_by_rnti(0x4601);
  if (u != nullptr) {
    byte_buffer pdu;

    // TODO: Call MAC SDU builder
  }
}

} // namespace srsgnb
