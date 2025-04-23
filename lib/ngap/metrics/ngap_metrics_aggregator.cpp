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

#include "ngap_metrics_aggregator.h"

using namespace srsran;
using namespace srs_cu_cp;

void ngap_metrics_aggregator::handle_requested_pdu_session(s_nssai_t s_nssai)
{
  if (aggregated_ngap_metrics.pdu_session_metrics.find(s_nssai) == aggregated_ngap_metrics.pdu_session_metrics.end()) {
    aggregated_ngap_metrics.pdu_session_metrics.emplace(s_nssai, pdu_session_metrics_t{1, 0, {}});
  } else {
    aggregated_ngap_metrics.pdu_session_metrics[s_nssai].nof_pdu_sessions_requested_to_setup++;
  }
}

void ngap_metrics_aggregator::handle_successful_pdu_session_setup(s_nssai_t s_nssai)
{
  if (aggregated_ngap_metrics.pdu_session_metrics.find(s_nssai) == aggregated_ngap_metrics.pdu_session_metrics.end()) {
    aggregated_ngap_metrics.pdu_session_metrics.emplace(s_nssai, pdu_session_metrics_t{0, 1, {}});
  } else {
    aggregated_ngap_metrics.pdu_session_metrics[s_nssai].nof_pdu_sessions_successfully_setup++;
  }
}

void ngap_metrics_aggregator::handle_failed_pdu_session_setup(s_nssai_t s_nssai, ngap_cause_t cause)
{
  if (aggregated_ngap_metrics.pdu_session_metrics.find(s_nssai) == aggregated_ngap_metrics.pdu_session_metrics.end()) {
    aggregated_ngap_metrics.pdu_session_metrics.emplace(s_nssai, pdu_session_metrics_t{0, 0, {{cause, 1}}});
  } else {
    std::map<ngap_cause_t, unsigned>& failed_sessions =
        aggregated_ngap_metrics.pdu_session_metrics[s_nssai].nof_pdu_sessions_failed_to_setup;

    if (failed_sessions.find(cause) == failed_sessions.end()) {
      failed_sessions.emplace(cause, 1);
    } else {
      failed_sessions[cause]++;
    }
  }
}

void ngap_metrics_aggregator::handle_requested_handover_preparation()
{
  aggregated_ngap_metrics.mobility_metrics.nof_handover_preparations_requested++;
}

void ngap_metrics_aggregator::handle_successful_handover_preparation()
{
  aggregated_ngap_metrics.mobility_metrics.nof_successful_handover_preparations++;
}

ngap_metrics ngap_metrics_aggregator::request_metrics_report() const
{
  return aggregated_ngap_metrics;
}
