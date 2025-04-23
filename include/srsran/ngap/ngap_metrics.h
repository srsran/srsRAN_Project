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

#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/ran/s_nssai.h"
#include <map>

namespace srsran {

// PDU session management metrics, see TS 28.552 section 5.1.1.5.
struct pdu_session_metrics_t {
  unsigned                         nof_pdu_sessions_requested_to_setup = 0;
  unsigned                         nof_pdu_sessions_successfully_setup = 0;
  std::map<ngap_cause_t, unsigned> nof_pdu_sessions_failed_to_setup;
};

// Mobility Management metrics, see TS 28.552 section 5.1.1.6.
struct mobility_metrics_t {
  unsigned nof_handover_preparations_requested  = 0;
  unsigned nof_successful_handover_preparations = 0;
};

/// \brief NGAP metrics for all UEs connected to an AMF.
struct ngap_metrics {
  std::map<s_nssai_t, pdu_session_metrics_t> pdu_session_metrics;
  mobility_metrics_t                         mobility_metrics;
};

struct ngap_info {
  /// Name of the AMF connected to the CU-CP.
  std::string  amf_name;
  ngap_metrics metrics;
};

} // namespace srsran
