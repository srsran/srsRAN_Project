
#ifndef SRSGNB_MAC_UL_COMPONENT_H
#define SRSGNB_MAC_UL_COMPONENT_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "../mac_sch_pdu.h"
#include "mac_ul_demux.h"
#include "mac_ul_pdu_handler.h"
#include "mac_ul_ue_manager.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/async/execute_on.h"

namespace srsgnb {

class mac_ul_component final : public mac_ul_configurer
{
public:
  mac_ul_component(mac_common_config_t& cfg_, mac_ul_sdu_notifier& ul_ccch_notifier_, sched_interface& sched_) :
    cfg(cfg_), sched(sched_), ue_manager(cfg, ul_ccch_notifier_), sdu_to_ue_manager_adapter(*this)
  {
    pdu_handlers.push_back(std::make_unique<mac_ul_pdu_handler>(cfg, sched, sdu_to_ue_manager_adapter));
  }

  async_task<bool> add_ue(const mac_ue_create_request_message& request) override
  {
    // TODO: define dispatch policy to UL workers
    return dispatch_and_resume_on(cfg.ul_exec, cfg.ctrl_exec, [this, request]() { return ue_manager.add_ue(request); });
  }

  async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& request) override
  {
    return dispatch_and_resume_on(
        cfg.ul_exec, cfg.ctrl_exec, [this, request]() { return ue_manager.reconfigure_ue(request); });
  }

  async_task<void> remove_ue(const mac_ue_delete_request_message& msg) override
  {
    return dispatch_and_resume_on(
        cfg.ul_exec, cfg.ctrl_exec, [this, ue_index = msg.ue_index]() { ue_manager.remove_ue(ue_index); });
  }

  void handle_rx_data_indication(mac_rx_data_indication& msg)
  {
    for (mac_rx_pdu& pdu : msg.pdus) {
      // Decode PDU and dispatch respective subPDUs to respective {RNTI, LCID}
      if (not pdu_handlers[msg.cell_index]->handle_ul_pdu(msg.sl_rx, msg.cell_index, pdu)) {
        continue;
      }
    }
  }

private:
  /// Push decoded MAC UL SDU to UE manager to be forwarded to upper layers
  void handle_ul_sdu(mac_ul_sdu& sdu)
  {
    // TODO: Fetch the correct UL worker
    cfg.ul_exec.execute([this, sdu = std::move(sdu)]() mutable { ue_manager.handle_ul_sdu(sdu); });
  }

  mac_common_config_t& cfg;
  sched_interface&     sched;

  mac_ul_ue_manager ue_manager;

  class ue_manager_sdu_adapter final : public mac_ul_sdu_notifier
  {
  public:
    ue_manager_sdu_adapter(mac_ul_component& parent_) : parent(parent_) {}

    void on_ul_sdu(mac_ul_sdu sdu) override { parent.handle_ul_sdu(sdu); }

  private:
    mac_ul_component& parent;
  };

  ue_manager_sdu_adapter sdu_to_ue_manager_adapter;

  std::vector<std::unique_ptr<mac_ul_pdu_handler> > pdu_handlers;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UL_COMPONENT_H
