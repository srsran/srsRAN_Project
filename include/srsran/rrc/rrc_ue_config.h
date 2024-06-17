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

#include "rrc_types.h"
#include "srsran/pdcp/pdcp_t_reordering.h"

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
