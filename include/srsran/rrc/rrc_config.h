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

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ran/gnb_id.h"
#include "srsran/rrc/rrc_ue_config.h"
#include <map>
#include <memory>

namespace srsran {

namespace srs_cu_cp {

struct rrc_cfg_t {
  /// The gNodeB identifier to use for measurement configs.
  gnb_id_t gnb_id;
  /// PDCP config to use when UE SRB2 are configured.
  srb_pdcp_config srb2_cfg;
  /// Configuration for available 5QI.
  std::map<five_qi_t, cu_cp_qos_config> drb_config;
  /// Integrity protection algorithms preference list
  security::preferred_integrity_algorithms int_algo_pref_list{security::integrity_algorithm::nia0};
  /// Encryption algorithms preference list
  security::preferred_ciphering_algorithms enc_algo_pref_list{security::ciphering_algorithm::nea0};
  /// Force re-establishment fallback.
  bool force_reestablishment_fallback = false;
  /// Timeout for RRC procedures.
  std::chrono::milliseconds rrc_procedure_timeout_ms{360};
};

} // namespace srs_cu_cp

} // namespace srsran
