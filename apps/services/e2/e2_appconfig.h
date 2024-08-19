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
#include <string>

namespace srsran {
/// E2 Agent configuration
struct e2_appconfig {
  bool        enable_du_e2           = false;       ///< Whether to enable DU E2 agent
  bool        enable_cu_e2           = false;       ///< Whether to enable CU E2 agent
  std::string ip_addr                = "127.0.0.1"; ///< RIC IP address
  uint16_t    port                   = 36421;       ///< RIC port
  std::string bind_addr              = "127.0.0.1"; ///< Local IP address to bind for RIC connection
  int         sctp_rto_initial       = 120;         ///< SCTP initial RTO value for RIC connection
  int         sctp_rto_min           = 120;         ///< SCTP RTO min for RIC connection
  int         sctp_rto_max           = 500;         ///< SCTP RTO max for RIC connection
  int         sctp_init_max_attempts = 3;           ///< SCTP init max attempts for RIC connection
  int         sctp_max_init_timeo    = 500;         ///< SCTP max init timeout for RIC connection
  bool        e2sm_kpm_enabled       = false;       ///< Whether to enable KPM service module
  bool        e2sm_rc_enabled        = false;       ///< Whether to enable RC service module
};

} // namespace srsran