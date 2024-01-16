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

#include "rrc_types.h"
#include "srsran/cu_cp/up_resource_manager.h"
#include "srsran/pdcp/pdcp_t_reordering.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace srs_cu_cp {

/// PDCP configuration for a SRB.
struct srb_pdcp_config {
  /// Value in ms of t-Reordering specified in TS 38.323.
  pdcp_t_reordering t_reordering = pdcp_t_reordering::infinity;
};

/// RRC UE configuration.
struct rrc_ue_cfg_t {
  /// PDCP configuration for SRB1.
  srb_pdcp_config              srb1_pdcp_cfg;
  std::vector<rrc_meas_timing> meas_timings;
  bool                         force_reestablishment_fallback = false;
  unsigned rrc_procedure_timeout_ms; ///< Timeout used for RRC message exchange with UE. It needs to suit the expected
                                     ///< communication delay and account for potential retransmissions (HARQ and RLC),
                                     ///< UE processing delays (see Sec 12 in TS 38.331), SR delays, etc.
  security::preferred_integrity_algorithms int_algo_pref_list; ///< Integrity protection algorithms preference list
  security::preferred_ciphering_algorithms enc_algo_pref_list; ///< Encryption algorithms preference list
};

} // namespace srs_cu_cp
} // namespace srsran
