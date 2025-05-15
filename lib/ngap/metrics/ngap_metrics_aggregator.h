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

#include "srsran/ngap/ngap_metrics.h"
#include "srsran/ran/s_nssai.h"
#include <cmath>

namespace srsran::srs_cu_cp {

class ngap_metrics_aggregator
{
public:
  void aggregate_requested_pdu_session(s_nssai_t s_nssai);

  void aggregate_successful_pdu_session_setup(s_nssai_t s_nssai);

  void aggregate_failed_pdu_session_setup(s_nssai_t s_nssai, ngap_cause_t cause);

  void aggregate_cn_initiated_paging_request();

  ngap_metrics request_metrics_report() const;

private:
  ngap_metrics aggregated_ngap_metrics;
};

} // namespace srsran::srs_cu_cp
