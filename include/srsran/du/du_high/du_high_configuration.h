
#pragma once

#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_high/du_high_executor_mapper.h"
#include "srsran/du/du_high/du_qos_config.h"
#include "srsran/du/du_high/du_srb_config.h"
#include "srsran/du/du_high/du_test_mode_config.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2_connection_client.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/f1ap/gateways/f1c_connection_client.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/mac/mac_config.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/rlc/rlc_metrics.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include <map>

namespace srsran {

class timer_manager;

namespace srs_du {

class f1u_du_gateway;

/// RAN-specific parameters of the DU-high.
struct du_high_ran_config {
  std::string                        gnb_du_name = "srsdu";
  gnb_du_id_t                        gnb_du_id   = gnb_du_id_t::min;
  std::vector<du_cell_config>        cells;
  std::map<srb_id_t, du_srb_config>  srbs;
  std::map<five_qi_t, du_qos_config> qos;
  mac_expert_config                  mac_cfg;
  scheduler_expert_config            sched_cfg;
};

/// Configuration passed to DU-High.
struct du_high_configuration {
  srs_du::du_high_ran_config       ran;
  srs_du::du_high_executor_mapper* exec_mapper               = nullptr;
  f1c_connection_client*           f1c_client                = nullptr;
  f1u_du_gateway*                  f1u_gw                    = nullptr;
  mac_result_notifier*             phy_adapter               = nullptr;
  timer_manager*                   timers                    = nullptr;
  scheduler_metrics_notifier*      sched_ue_metrics_notifier = nullptr;
  rlc_metrics_notifier*            rlc_metrics_notif         = nullptr;
  e2_connection_client*            e2_client                 = nullptr;
  e2_du_metrics_interface*         e2_du_metric_iface        = nullptr;
  mac_pcap*                        mac_p                     = nullptr;
  rlc_pcap*                        rlc_p                     = nullptr;
  du_test_mode_config              test_cfg;
  e2ap_configuration               e2ap_config;
};

} // namespace srs_du
} // namespace srsran
