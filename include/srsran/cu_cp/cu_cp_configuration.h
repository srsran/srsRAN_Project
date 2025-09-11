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

#include "srsran/cu_cp/cell_meas_manager_config.h"
#include "srsran/cu_cp/cu_cp_metrics_notifier.h"
#include "srsran/cu_cp/mobility_manager_config.h"
#include "srsran/cu_cp/ue_configuration.h"
#include "srsran/e1ap/cu_cp/e1ap_configuration.h"
#include "srsran/e2/e2_cu.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/e2/gateways/e2_connection_client.h"
#include "srsran/f1ap/cu_cp/f1ap_configuration.h"
#include "srsran/ran/tac.h"
#include "srsran/rrc/rrc_ue_config.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/executors/task_executor.h"
#include <chrono>

namespace srsran {

class pdcp_metrics_notifier;

namespace srs_cu_cp {
class n2_connection_client;
class ngap_repository;

struct plmn_item {
  plmn_identity plmn_id;
  /// Supported Slices by the RAN node.
  std::vector<s_nssai_t> slice_support_list;
};

struct supported_tracking_area {
  tac_t                  tac;
  std::vector<plmn_item> plmn_list;
};

/// Parameters of the CU-CP that will reported to the 5G core.
struct ran_node_configuration {
  /// The gNodeB identifier.
  gnb_id_t    gnb_id{411, 22};
  std::string ran_node_name = "srsgnb01";
};

struct mobility_configuration {
  cell_meas_manager_cfg meas_manager_config;
  mobility_manager_cfg  mobility_manager_config;
};

/// Configuration passed to CU-CP.
struct cu_cp_configuration {
  struct admission_params {
    /// Maximum number of DU connections that the CU-CP may accept.
    unsigned max_nof_dus = 6;
    /// Maximum number of CU-UP connections that the CU-CP may accept.
    unsigned max_nof_cu_ups = 6;
    /// Maximum number of UEs that the CU-CP may accept.
    unsigned max_nof_ues = 8192;
    /// Maximum number of DRBs per UE that the CU-CP will configure.
    uint8_t max_nof_drbs_per_ue = 8;
  };

  struct service_params {
    task_executor* cu_cp_executor = nullptr;
    task_executor* cu_cp_e2_exec  = nullptr;
    timer_manager* timers         = nullptr;
  };

  struct ngap_config {
    n2_connection_client* n2_gw = nullptr;
    // Supported TAs for each AMF.
    std::vector<supported_tracking_area> supported_tas;
  };

  struct ngap_params {
    /// NGAP configurations.
    std::vector<ngap_config> ngaps;
    /// Time to wait after a failed AMF reconnection attempt in ms.
    std::chrono::milliseconds amf_reconnection_retry_time = std::chrono::milliseconds{1000};
    /// Option to run CU-CP without a core.
    bool no_core = false;
  };

  struct rrc_params {
    /// Force re-establishment fallback.
    bool force_reestablishment_fallback = false;
    /// Guard time for RRC procedures.
    std::chrono::milliseconds rrc_procedure_guard_time_ms{1000};
    /// Version of the RRC.
    unsigned rrc_version = 2;
  };

  struct security_params {
    /// Integrity protection algorithms preference list
    security::preferred_integrity_algorithms int_algo_pref_list{security::integrity_algorithm::nia0};
    /// Encryption algorithms preference list
    security::preferred_ciphering_algorithms enc_algo_pref_list{security::ciphering_algorithm::nea0};
    /// Default security if not signaled via NGAP.
    security_indication_t default_security_indication;
  };
  struct bearer_params {
    /// PDCP config to use when UE SRB2 are configured.
    srb_pdcp_config srb2_cfg;
    /// Configuration for available 5QI.
    std::map<five_qi_t, cu_cp_qos_config> drb_config;
  };

  struct metrics_layers_config {
    /// Enable NGAP metrics.
    bool enable_ngap = false;
    /// Enable RRC metrics.
    bool enable_rrc = false;
  };

  struct metrics_params {
    /// CU-CP statistics report period.
    std::chrono::seconds      statistics_report_period{1};
    std::chrono::milliseconds metrics_report_period{0};
    metrics_layers_config     layers_cfg = {};
  };

  /// NG-RAN node parameters.
  ran_node_configuration node;
  /// Parameters to determine the admission of new CU-UP, DU and UE connections.
  admission_params admission;
  /// NGAP layer-specific parameters.
  ngap_params ngap;
  /// RRC layer-specific parameters.
  rrc_params rrc;
  /// F1AP layer-specific parameters.
  f1ap_configuration f1ap;
  /// E1AP layer-specific parameters.
  e1ap_configuration e1ap;
  /// UE Security-specific parameters.
  security_params security;
  /// SRB and DRB configuration of created UEs.
  bearer_params bearers;
  /// UE-specific parameters.
  ue_configuration ue;
  /// Parameters related with the mobility of UEs.
  mobility_configuration mobility;
  /// Parameters related with CU-CP metrics.
  metrics_params metrics;
  /// Timers, executors, and other services used by the CU-CP.
  service_params services;
  /// CU-CP metrics notifier.
  cu_cp_metrics_report_notifier* metrics_notifier = nullptr;
};

} // namespace srs_cu_cp
} // namespace srsran
