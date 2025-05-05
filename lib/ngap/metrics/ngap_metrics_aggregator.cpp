/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_metrics_aggregator.h"

using namespace srsran;
using namespace srs_cu_cp;

void ngap_metrics_aggregator::aggregate_requested_pdu_session(s_nssai_t s_nssai)
{
  if (aggregated_ngap_metrics.pdu_session_metrics.find(s_nssai) == aggregated_ngap_metrics.pdu_session_metrics.end()) {
    aggregated_ngap_metrics.pdu_session_metrics.emplace(s_nssai, pdu_session_metrics_t{1, 0, {}});
  } else {
    ++aggregated_ngap_metrics.pdu_session_metrics[s_nssai].nof_pdu_sessions_requested_to_setup;
  }
}

void ngap_metrics_aggregator::aggregate_successful_pdu_session_setup(s_nssai_t s_nssai)
{
  if (aggregated_ngap_metrics.pdu_session_metrics.find(s_nssai) == aggregated_ngap_metrics.pdu_session_metrics.end()) {
    aggregated_ngap_metrics.pdu_session_metrics.emplace(s_nssai, pdu_session_metrics_t{0, 1, {}});
  } else {
    ++aggregated_ngap_metrics.pdu_session_metrics[s_nssai].nof_pdu_sessions_successfully_setup;
  }
}

void ngap_metrics_aggregator::aggregate_failed_pdu_session_setup(s_nssai_t s_nssai, ngap_cause_t cause)
{
  if (aggregated_ngap_metrics.pdu_session_metrics.find(s_nssai) == aggregated_ngap_metrics.pdu_session_metrics.end()) {
    aggregated_ngap_metrics.pdu_session_metrics.emplace(s_nssai, pdu_session_metrics_t{0, 0, {}});
  }

  aggregated_ngap_metrics.pdu_session_metrics[s_nssai].nof_pdu_sessions_failed_to_setup.increase(cause);
}

ngap_metrics ngap_metrics_aggregator::request_metrics_report() const
{
  return aggregated_ngap_metrics;
}
