/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rrc_types.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include "srsran/cu_cp/up_resource_manager.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

namespace srs_cu_cp {
struct rrc_ue_cfg_t {
  srslog::basic_logger&        logger = srslog::fetch_basic_logger("RRC");
  asn1::rrc_nr::pdcp_cfg_s     srb1_pdcp_cfg; ///< PDCP configuration for SRB1.
  std::vector<rrc_meas_timing> meas_timings;
  bool                         force_reestablishment_fallback = false;
  unsigned rrc_procedure_timeout_ms; ///< Timeout used for RRC message exchange with UE. It needs to suit the expected
                                     ///< communication delay and account for potential retransmissions (HARQ and RLC),
                                     ///< UE processing delays (see Sec 12 in TS 38.331), SR delays, etc.
};

} // namespace srs_cu_cp

} // namespace srsran
