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

#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/ran/s_nssai.h"
#include <map>

namespace srsran {

// PDU session management metrics, see TS 28.552 section 5.1.1.5.
struct pdu_session_metrics_t {
  unsigned                         nof_pdu_sessions_requested_to_setup;
  unsigned                         nof_pdu_sessions_successfully_setup;
  std::map<ngap_cause_t, unsigned> nof_pdu_sessions_failed_to_setup;
};

// Mobility Management metrics, see TS 28.552 section 5.1.1.6.
struct mobility_metrics_t {
  unsigned nof_handover_preparations_requested;
  unsigned nof_successful_handover_preparations;
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
