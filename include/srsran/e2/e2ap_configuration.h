/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/gnb_cu_up_id.h"
#include "srsran/ran/gnb_du_id.h"
#include "srsran/ran/gnb_id.h"
#include <optional>
#include <string>

namespace srsran {

enum class e2_type_t { DU, CU_UP, CU_CP };
/// \brief E2AP configuration
struct e2ap_configuration {
  e2_type_t                     e2_type = e2_type_t::DU;
  gnb_id_t                      gnb_id  = {0, 22};
  std::string                   plmn; /// Full PLMN as string (without possible filler digit) e.g. "00101"
  std::optional<gnb_du_id_t>    gnb_du_id;
  std::optional<gnb_cu_up_id_t> gnb_cu_up_id;
  unsigned                      max_setup_retries = 5;
  bool                          e2sm_kpm_enabled  = false;
  bool                          e2sm_rc_enabled   = false;
};

/// E2 Agent configuration.
struct e2_config {
  /// Whether to enable E2 agent.
  bool enable_unit_e2 = false;
  /// RIC IP address.
  std::string ip_addr = "127.0.0.1";
  /// RIC port.
  uint16_t port = 36421;
  /// Local IP address to bind for RIC connection.
  std::string bind_addr = "127.0.0.1";
  /// SCTP initial RTO value for RIC connection.
  int sctp_rto_initial = 120;
  /// SCTP RTO min for RIC connection.
  int sctp_rto_min = 120;
  /// SCTP RTO max for RIC connection.
  int sctp_rto_max = 500;
  /// SCTP init max attempts for RIC connection.
  int sctp_init_max_attempts = 3;
  /// SCTP max init timeout for RIC connection.
  int sctp_max_init_timeo = 500;
  /// Whether to enable KPM service module.
  bool e2sm_kpm_enabled = false;
  /// Whether to enable RC service module.
  bool e2sm_rc_enabled = false;
};
} // namespace srsran
