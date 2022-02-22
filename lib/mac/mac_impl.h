
#ifndef SRSGNB_MAC_IMPL_H
#define SRSGNB_MAC_IMPL_H

#include "mac_ctrl.h"
#include "mac_ctxt.h"
#include "srsgnb/adt/circular_map.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/srslog/srslog.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/task_executor.h"
#include "ue_creation_procedure.h"
#include "ue_delete_procedure.h"
#include <mutex>

namespace srsgnb {

class mac_impl : public mac_interface
{
public:
  explicit mac_impl(mac_config_notifier& listener,
                    mac_ul_sdu_notifier& ul_ccch_notifier,
                    task_executor&       ul_exec,
                    span<task_executor*> dl_execs,
                    task_executor&       ctrl_exec);

  void ue_create_request(const mac_ue_create_request_message& cfg) override;
  void ue_reconfiguration_request(const mac_ue_reconfiguration_request& cfg) override {}
  void ue_delete_request(const mac_ue_delete_request_message& cfg) override;

  void push_ul_pdu(mac_rx_data_indication pdu) override;

  void slot_indication(slot_point sl_tx, du_cell_index_t cc) override;

  // Sched->MAC interface
  void sched_ue_delete_response(rnti_t rnti);
  void sched_ue_config_response(rnti_t rnti);

private:
  srslog::basic_logger& logger;

  std::unique_ptr<sched_cfg_notifier> sched_notifier;
  mac_dl_worker                       dl_worker;
  mac_ul_worker                       ul_worker;
  mac_context                         ctxt;
  mac_ctrl_worker                     ctrl_worker;

  std::mutex dl_mutex;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_IMPL_H
