/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/du/du_cell_config.h"
#include "srsran/du/du_high/du_high_executor_mapper.h"
#include "srsran/du/du_high/du_qos_config.h"
#include "srsran/du/du_high/du_srb_config.h"
#include "srsran/du/du_high/du_test_mode_config.h"
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
class du_metrics_notifier;

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
  struct metrics_config {
    bool                      enable_sched = false;
    bool                      enable_mac   = false;
    bool                      enable_rlc   = false;
    bool                      enable_f1ap  = false;
    std::chrono::milliseconds period{1000};
    unsigned                  max_nof_sched_ue_events = 64;
  };

  du_high_ran_config  ran;
  metrics_config      metrics;
  du_test_mode_config test_cfg;
};

/// DU high dependencies
struct du_high_dependencies {
  du_high_executor_mapper* exec_mapper       = nullptr;
  f1c_connection_client*   f1c_client        = nullptr;
  f1u_du_gateway*          f1u_gw            = nullptr;
  mac_result_notifier*     phy_adapter       = nullptr;
  mac_clock_controller*    timer_ctrl        = nullptr;
  du_metrics_notifier*     du_notifier       = nullptr;
  rlc_metrics_notifier*    rlc_metrics_notif = nullptr;
  mac_pcap*                mac_p             = nullptr;
  rlc_pcap*                rlc_p             = nullptr;
};

} // namespace srs_du
} // namespace srsran
