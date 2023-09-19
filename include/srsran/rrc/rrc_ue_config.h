/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
