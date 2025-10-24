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

#include "srsran/cu_cp/mobility_management_metrics.h"
#include "srsran/ran/cause/ngap_cause.h"
#include "srsran/ran/plmn_identity.h"
#include "srsran/ran/s_nssai.h"
#include "srsran/support/format/fmt_to_c_str.h"
#include "srsran/support/srsran_assert.h"
#include <map>

namespace srsran {

/// Offsets for the cause values to use them as index for failure counters.
constexpr unsigned CAUSE_RADIO_NETWORK_OFFSET = 0;
constexpr unsigned CAUSE_TRANSPORT_OFFSET     = 57;
constexpr unsigned CAUSE_NAS_OFFSET           = 59;
constexpr unsigned CAUSE_PROTOCOL_OFFSET      = 63;
constexpr unsigned CAUSE_MISC_OFFSET          = 70;
constexpr unsigned NOF_CAUSES                 = 75;

/// \brief Convert a cause value to an index.
/// \param cause The cause value to convert.
/// \return The index corresponding to the cause value.
constexpr unsigned cause_to_index(ngap_cause_t cause)
{
  if (const auto* result = std::get_if<ngap_cause_radio_network_t>(&cause)) {
    return static_cast<unsigned>(*result) + CAUSE_RADIO_NETWORK_OFFSET;
  }

  if (const auto* result = std::get_if<ngap_cause_transport_t>(&cause)) {
    return static_cast<unsigned>(*result) + CAUSE_TRANSPORT_OFFSET;
  }

  if (const auto* result = std::get_if<cause_nas_t>(&cause)) {
    return static_cast<unsigned>(*result) + CAUSE_NAS_OFFSET;
  }

  if (const auto* result = std::get_if<cause_protocol_t>(&cause)) {
    return static_cast<unsigned>(*result) + CAUSE_PROTOCOL_OFFSET;
  }

  return static_cast<unsigned>(std::get<ngap_cause_misc_t>(cause)) + CAUSE_MISC_OFFSET;
}

/// \brief Convert an index to a cause value.
/// \param index The index to convert.
/// \return The cause value corresponding to the index.
/// \note The index must be in the range [0, NOF_CAUSES).
constexpr ngap_cause_t index_to_cause(unsigned index)
{
  srsran_assert(index < NOF_CAUSES, "Invalid cause");

  if (index < CAUSE_TRANSPORT_OFFSET) {
    return ngap_cause_radio_network_t(index);
  }

  if (index < CAUSE_NAS_OFFSET) {
    return ngap_cause_transport_t(index - CAUSE_TRANSPORT_OFFSET);
  }

  if (index < CAUSE_PROTOCOL_OFFSET) {
    return cause_nas_t(index - CAUSE_NAS_OFFSET);
  }

  if (index < CAUSE_MISC_OFFSET) {
    return cause_protocol_t(index - CAUSE_PROTOCOL_OFFSET);
  }

  return ngap_cause_misc_t(index - CAUSE_MISC_OFFSET);
}

/// \brief A counter for each cause value.
struct ngap_counter_with_cause {
  ngap_counter_with_cause() = default;

  void increase(ngap_cause_t cause)
  {
    srsran_assert(cause_to_index(cause) < counters_by_cause.size(), "Invalid cause");
    ++counters_by_cause[cause_to_index(cause)];
  }

  unsigned size() const { return counters_by_cause.size(); }

  unsigned get_count(ngap_cause_t cause) const { return counters_by_cause[cause_to_index(cause)]; }

  unsigned get_count(unsigned index) const { return counters_by_cause[index]; }

  ngap_cause_t get_cause(unsigned index) const { return index_to_cause(index); }

  /// Returns a const iterator to the beginning of the container.
  std::array<unsigned, NOF_CAUSES>::const_iterator begin() const { return counters_by_cause.begin(); }
  std::array<unsigned, NOF_CAUSES>::const_iterator cbegin() const { return counters_by_cause.cbegin(); }

  /// Returns a const iterator to the end of the container.
  std::array<unsigned, NOF_CAUSES>::const_iterator end() const { return counters_by_cause.end(); }
  std::array<unsigned, NOF_CAUSES>::const_iterator cend() const { return counters_by_cause.cend(); }

private:
  // The counters indexed by a cause.
  std::array<unsigned, NOF_CAUSES> counters_by_cause = {};
};

/// PDU session management metrics, see TS 28.552 section 5.1.1.5.
struct pdu_session_metrics_t {
  unsigned                nof_pdu_sessions_requested_to_setup = 0;
  unsigned                nof_pdu_sessions_successfully_setup = 0;
  ngap_counter_with_cause nof_pdu_sessions_failed_to_setup;
};

/// Paging Measurement metrics, see TS 28.552 section 5.1.1.27.

/// \brief NGAP metrics for all UEs connected to an AMF.
struct ngap_metrics {
  std::map<s_nssai_t, pdu_session_metrics_t> pdu_session_metrics;
  /// Paging Measurement metrics, see TS 28.552 section 5.1.1.27.
  unsigned nof_cn_initiated_paging_requests = 0;
};

/// \brief NGAP metrics per AMF.
struct ngap_info {
  /// Name of the AMF connected to the CU-CP.
  std::string                amf_name;
  bool                       connected = false;
  std::vector<plmn_identity> supported_plmns;
  ngap_metrics               metrics;
};

inline std::string format_ngap_metrics(const std::vector<ngap_info>&      report,
                                       const mobility_management_metrics& mobility_metrics)
{
  fmt::memory_buffer buffer;

  for (const auto& ngap_info : report) {
    // log ngap metrics
    fmt::format_to(std::back_inserter(buffer), "[");
    fmt::format_to(std::back_inserter(buffer),
                   " amf_name={} connected={} supported_plmns=[",
                   ngap_info.amf_name,
                   ngap_info.connected);
    for (const auto& plmn : ngap_info.supported_plmns) {
      fmt::format_to(std::back_inserter(buffer), " {},", plmn);
    }
    fmt::format_to(std::back_inserter(buffer), " ]");

    for (const auto& pdu_session_metric : ngap_info.metrics.pdu_session_metrics) {
      fmt::format_to(std::back_inserter(buffer),
                     " s-nssai=(sst={} sd={})",
                     pdu_session_metric.first.sst.value(),
                     pdu_session_metric.first.sd.is_set() ? fmt::format("{}", pdu_session_metric.first.sd.value())
                                                          : "na");

      fmt::format_to(std::back_inserter(buffer),
                     " nof_pdu_sessions_requested_to_setup={} nof_pdu_sessions_successfully_setup={}",
                     pdu_session_metric.second.nof_pdu_sessions_requested_to_setup,
                     pdu_session_metric.second.nof_pdu_sessions_successfully_setup);

      fmt::format_to(std::back_inserter(buffer), " nof_pdu_sessions_failed_to_setup=[");
      unsigned cause_index = 0;
      for (const auto& cause_count : pdu_session_metric.second.nof_pdu_sessions_failed_to_setup) {
        fmt::format_to(std::back_inserter(buffer),
                       " {}={}",
                       pdu_session_metric.second.nof_pdu_sessions_failed_to_setup.get_cause(cause_index),
                       cause_count);
        ++cause_index;
      }
      fmt::format_to(std::back_inserter(buffer), " ]");
    }
    fmt::format_to(std::back_inserter(buffer),
                   " nof_cn_initiated_paging_requests={}",
                   ngap_info.metrics.nof_cn_initiated_paging_requests);

    fmt::format_to(std::back_inserter(buffer), " ],");
  }

  fmt::format_to(std::back_inserter(buffer),
                 " nof_handover_preparations_requested={} nof_successful_handover_preparations={}",
                 mobility_metrics.nof_handover_preparations_requested,
                 mobility_metrics.nof_successful_handover_preparations);

  return to_c_str(buffer);
}

} // namespace srsran
