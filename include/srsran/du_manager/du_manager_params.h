/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "srsran/du/du_qos_config.h"
#include "srsran/du/du_srb_config.h"
#include "srsran/du/du_test_config.h"
#include "srsran/du_high/du_high_executor_mapper.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/f1u/du/f1u_gateway.h"
#include "srsran/mac/mac.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/rlc/rlc_metrics.h"
#include "srsran/scheduler/config/scheduler_expert_config.h"
#include <map>

namespace srsran {

class timer_manager;

namespace srs_du {

struct du_manager_params {
  struct ran_params {
    std::string                        gnb_du_name;
    uint64_t                           gnb_du_id;
    uint8_t                            rrc_version;
    transport_layer_address            du_bind_addr;
    std::vector<du_cell_config>        cells;
    std::map<srb_id_t, du_srb_config>  srbs;
    std::map<five_qi_t, du_qos_config> qos;
  };
  struct service_params {
    timer_manager&                timers;
    task_executor&                du_mng_exec;
    du_high_ue_executor_mapper&   ue_execs;
    du_high_cell_executor_mapper& cell_execs;
  };
  struct f1ap_config_params {
    f1ap_connection_manager& conn_mng;
    f1ap_ue_context_manager& ue_mng;
  };
  struct f1u_config_params {
    f1u_du_gateway& f1u_gw;
  };
  struct rlc_config_params {
    mac_ue_control_information_handler&          mac_ue_info_handler;
    f1ap_message_handler&                        f1ap_rx_msg_handler;
    f1ap_rrc_message_transfer_procedure_handler& f1ap_rx_proc_handler;
    rlc_pcap&                                    pcap_writer;
    rlc_metrics_notifier*                        rlc_metrics_notif = nullptr;
  };
  struct mac_config_params {
    mac_cell_manager&       cell_mng;
    mac_ue_configurator&    ue_cfg;
    scheduler_expert_config sched_cfg;
  };

  ran_params         ran;
  service_params     services;
  f1ap_config_params f1ap;
  f1u_config_params  f1u;
  rlc_config_params  rlc;
  mac_config_params  mac;
};

} // namespace srs_du
} // namespace srsran
