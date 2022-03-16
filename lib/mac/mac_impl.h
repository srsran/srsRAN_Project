
#ifndef SRSGNB_MAC_IMPL_H
#define SRSGNB_MAC_IMPL_H

#include "mac_config.h"
#include "mac_ctrl/mac_ctrl_component.h"
#include "mac_ctrl/ue_creation_procedure.h"
#include "mac_ctrl/ue_delete_procedure.h"
#include "mac_ul/mac_ul_manager.h"
#include "srsgnb/adt/circular_map.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/srslog/srslog.h"
#include "srsgnb/support/async/manual_event.h"
#include <mutex>

namespace srsgnb {

class mac_impl : public mac_interface
{
public:
  explicit mac_impl(mac_rx_sdu_notifier&   ul_ccch_notifier,
                    du_l2_executor_mapper& ul_exec_mapper,
                    span<task_executor*>   dl_execs,
                    task_executor&         ctrl_exec);

  async_task<mac_ue_create_response_message> ue_create_request(const mac_ue_create_request_message& cfg) override;
  async_task<mac_ue_reconfiguration_response_message>
  ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg) override;
  async_task<mac_ue_delete_response_message> ue_delete_request(const mac_ue_delete_request_message& cfg) override;

  void push_rx_data_indication(mac_rx_data_indication msg) override;

  void slot_indication(slot_point sl_tx, du_cell_index_t cc) override;

private:
  mac_common_config_t   cfg;
  srslog::basic_logger& logger;

  mac_dl_component   dl_unit;
  mac_ul_manager     ul_unit;
  mac_ctrl_component ctrl_unit;

  std::mutex dl_mutex;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_IMPL_H
