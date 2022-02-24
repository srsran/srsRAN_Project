
#include "mac_impl.h"
#include <mutex>

namespace srsgnb {

class sched_response_adapter final : public sched_cfg_notifier
{
public:
  sched_response_adapter(mac_impl& mac_) : mac_ref(mac_) {}

  void on_ue_config_complete(rnti_t rnti) override { mac_ref.sched_ue_config_response(rnti); }
  void on_ue_delete_response(rnti_t rnti) override { mac_ref.sched_ue_delete_response(rnti); }

  mac_impl& mac_ref;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

mac_impl::mac_impl(mac_config_notifier& cfg_notifier_,
                   mac_ul_sdu_notifier& ul_ccch_notifier_,
                   task_executor&       ul_exec_,
                   span<task_executor*> dl_execs_,
                   task_executor&       ctrl_exec_) :
  cfg(cfg_notifier_, ul_exec_, dl_execs_, ctrl_exec_),
  logger(cfg.logger),
  dl_unit(cfg),
  ul_unit(cfg, ul_ccch_notifier_),
  ctrl_unit(cfg, ul_unit, dl_unit)
{}

void mac_impl::push_ul_pdu(mac_rx_data_indication msg)
{
  ul_unit.push_ul_pdu(msg);
}

void mac_impl::ue_create_request(const mac_ue_create_request_message& msg)
{
  ctrl_unit.ue_create_request(msg);
}

void mac_impl::ue_delete_request(const mac_ue_delete_request_message& msg)
{
  ctrl_unit.ue_delete_request(msg);
}

void mac_impl::sched_ue_config_response(rnti_t rnti)
{
  ctrl_unit.sched_ue_create_response(rnti);
}

void mac_impl::sched_ue_delete_response(rnti_t rnti)
{
  ctrl_unit.sched_ue_delete_response(rnti);
}

void mac_impl::slot_indication(slot_point sl_tx, du_cell_index_t cc)
{
  {
    std::lock_guard<std::mutex> lock(dl_mutex);
    // synchronize sched
  }

  // for each cc, generate MAC DL SDUs
  mac_ue_context* u = ctrl_unit.find_by_rnti(0x4601);
  if (u != nullptr) {
    byte_buffer pdu;

    // TODO: Call MAC SDU builder
  }
}

} // namespace srsgnb
