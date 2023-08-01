
#pragma once

#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_qos_config.h"
#include "srsran/du/du_test_config.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2_connection_client.h"
#include "srsran/e2/e2_du_metrics_manager.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/f1ap/du/f1c_connection_client.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/mac/mac_config.h"
#include "srsran/pcap/pcap.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include <map>

namespace srsran {

class timer_manager;

namespace srs_du {

class f1u_du_gateway;

/// Configuration passed to DU-High.
struct du_high_configuration {
  du_high_executor_mapper*           exec_mapper          = nullptr;
  f1c_connection_client*             f1c_client           = nullptr;
  f1u_du_gateway*                    f1u_gw               = nullptr;
  mac_result_notifier*               phy_adapter          = nullptr;
  timer_manager*                     timers               = nullptr;
  scheduler_ue_metrics_notifier*     metrics_notifier     = nullptr;
  e2_connection_client*              e2_client            = nullptr;
  e2_du_metrics_interface*           e2_du_metric_manager = nullptr;
  std::string                        gnb_du_name;
  uint64_t                           gnb_du_id;
  transport_layer_address            du_bind_addr;
  std::vector<du_cell_config>        cells;
  std::map<five_qi_t, du_qos_config> qos; // 5QI as key
  mac_expert_config                  mac_cfg;
  scheduler_expert_config            sched_cfg;
  mac_pcap*                          pcap = nullptr;
  du_test_config                     test_cfg;
  e2ap_configuration                 e2ap_config;
};

} // namespace srs_du
} // namespace srsran
