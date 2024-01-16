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

#include "rrc_ue_config.h"
#include "srsran/adt/optional.h"
#include "srsran/rrc/rrc_ue_config.h"
#include <map>
#include <memory>

namespace srsran {

namespace srs_cu_cp {

struct rrc_cfg_t {
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
  unsigned rrc_procedure_timeout_ms = 360;
};

} // namespace srs_cu_cp

} // namespace srsran
