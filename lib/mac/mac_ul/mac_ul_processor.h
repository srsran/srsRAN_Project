
#ifndef SRSGNB_MAC_UL_PROCESSOR_H
#define SRSGNB_MAC_UL_PROCESSOR_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "mac_ul_ue_manager.h"
#include "pdu_rx_handler.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/async/execute_on.h"

namespace srsgnb {

class mac_ul_processor final : public mac_ul_configurer
{
public:
  mac_ul_processor(mac_common_config_t& cfg_, mac_sdu_rx_notifier& ul_ccch_notifier_, sched_interface& sched_) :
    cfg(cfg_), ue_manager(cfg, ul_ccch_notifier_), pdu_handler(cfg, sched_, ul_ccch_notifier_, ue_manager)
  {}

  async_task<bool> add_ue(const mac_ue_create_request_message& request) override
  {
    // Update UE executor due to new PCell
    task_executor& ul_exec = cfg.ul_exec_mapper.rebind_executor(request.crnti, request.cell_index);

    // Dispatch UE creation in new UL executor
    return dispatch_and_resume_on(ul_exec, cfg.ctrl_exec, [this, request]() { return ue_manager.add_ue(request); });
  }

  async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& request) override
  {
    return dispatch_and_resume_on(cfg.ul_exec_mapper.executor(request.crnti), cfg.ctrl_exec, [this, request]() {
      return ue_manager.reconfigure_ue(request);
    });
  }

  async_task<void> remove_ue(const mac_ue_delete_request_message& msg) override
  {
    return dispatch_and_resume_on(cfg.ul_exec_mapper.executor(msg.rnti),
                                  cfg.ctrl_exec,
                                  [this, ue_index = msg.ue_index]() { ue_manager.remove_ue(ue_index); });
  }

  /// Handles FAPI Rx_Data.Indication.
  /// The PDUs contained in the Rx_Data.Indication are dispatched to different executors, depending on their RNTI.
  void handle_rx_data_indication(mac_rx_data_indication& msg)
  {
    for (mac_rx_pdu& pdu : msg.pdus) {
      // 1. Fork each PDU handling to different executors based on the PDU RNTI.
      cfg.ul_exec_mapper.executor(pdu.rnti).execute(
          [this, slot_rx = msg.sl_rx, cell_idx = msg.cell_index, pdu = std::move(pdu)]() mutable {
            // 2. Decode Rx PDU and handle respective subPDUs.
            pdu_handler.handle_rx_pdu(slot_rx, cell_idx, pdu);
          });
    }
  }

private:
  mac_common_config_t& cfg;

  mac_ul_ue_manager ue_manager;

  /// Object that handles incoming UL MAC PDUs.
  pdu_rx_handler pdu_handler;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UL_PROCESSOR_H
