/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cell_meas_manager_config.h"
#include "srsran/cu_cp/mobility_manager_config.h"
#include "srsran/cu_cp/ue_configuration.h"
#include "srsran/f1ap/cu_cp/f1ap_configuration.h"
#include "srsran/rrc/rrc_ue_config.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace srs_cu_cp {

class n2_connection_client;

struct mobility_configuration {
  cell_meas_manager_cfg meas_manager_config;
  mobility_manager_cfg  mobility_manager_config;
};

/// Configuration passed to CU-CP.
struct cu_cp_configuration {
  struct node_params {
    /// The gNodeB identifier.
    gnb_id_t      gnb_id{411, 22};
    std::string   ran_node_name = "srsgnb01";
    plmn_identity plmn          = plmn_identity::test_value();
    unsigned      tac           = 7;
  };
  struct admission_params {
    /// Maximum number of DU connections that the CU-CP may accept.
    unsigned max_nof_dus = 6;
    /// Maximum number of CU-UP connections that the CU-CP may accept.
    unsigned max_nof_cu_ups = 6;
    /// Maximum number of UEs that the CU-CP may accept.
    unsigned max_nof_ues = 8192;
  };
  struct service_params {
    task_executor*        cu_cp_executor = nullptr;
    task_executor*        cu_cp_e2_exec  = nullptr;
    n2_connection_client* n2_gw          = nullptr;
    timer_manager*        timers         = nullptr;
  };
  struct rrc_params {
    /// Force re-establishment fallback.
    bool force_reestablishment_fallback = false;
    /// Timeout for RRC procedures.
    std::chrono::milliseconds rrc_procedure_timeout_ms{360};
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

  node_params            node;
  admission_params       admission;
  service_params         services;
  rrc_params             rrc;
  security_params        security;
  bearer_params          bearers;
  ue_configuration       ue_config;
  mobility_configuration mobility_config;
  f1ap_configuration     f1ap_config;
  /// Slice configurations.
  std::vector<s_nssai_t> slice_configurations;
  /// CU-CP statistics report period.
  std::chrono::seconds statistics_report_period{1};
};

} // namespace srs_cu_cp
} // namespace srsran
