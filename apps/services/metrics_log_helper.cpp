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

#include "metrics_log_helper.h"
#include "srsran/support/format_utils.h"
#include <cfloat>
#include <iomanip>
#include <ostream>

using namespace srsran;

void metrics_log_helper::report_metrics(const scheduler_cell_metrics& metrics)
{
  fmt::memory_buffer buffer;

  // log cell-wide metrics
  fmt::format_to(buffer, "Cell Scheduler Metrics:");
  fmt::format_to(buffer,
                 " error_indications={} mean_latency={}usec latency_hist=[{}]",
                 metrics.nof_error_indications,
                 metrics.average_decision_latency.count(),
                 fmt::join(metrics.latency_histogram.begin(), metrics.latency_histogram.end(), ", "));
  logger.info("{}", to_c_str(buffer));
  buffer.clear();

  // log ue-specific metrics
  for (const auto& ue : metrics.ue_metrics) {
    fmt::format_to(buffer, "Scheduler UE Metrics:");
    fmt::format_to(buffer, " pci={}", ue.pci);
    fmt::format_to(buffer, " rnti={:x}", to_value(ue.rnti));
    if (ue.cqi_stats.get_nof_observations() > 0) {
      fmt::format_to(buffer, " cqi={}", static_cast<unsigned>(std::roundf(ue.cqi_stats.get_mean())));
    } else {
      fmt::format_to(buffer, " cqi=n/a");
    }

    if (ue.ri_stats.get_nof_observations() > 0) {
      fmt::format_to(buffer, " ri={:.1f}", ue.ri_stats.get_mean());
    } else {
      fmt::format_to(buffer, " ri=n/a");
    }

    fmt::format_to(buffer, " dl_mcs={}", int(ue.dl_mcs.to_uint()));
    if (ue.dl_brate_kbps > 0) {
      fmt::format_to(buffer, " dl_brate_kbps={}", float_to_eng_string(ue.dl_brate_kbps * 1e3, 1, false));
    } else {
      fmt::format_to(buffer, " dl_brate_kbps={}", 0);
    }
    fmt::format_to(buffer, " dl_nof_ok={}", ue.dl_nof_ok);
    fmt::format_to(buffer, " dl_nof_nok={}", ue.dl_nof_nok);
    unsigned dl_total = ue.dl_nof_ok + ue.dl_nof_nok;
    if (dl_total > 0) {
      fmt::format_to(buffer, " dl_error_rate={}%", int((float)100 * ue.dl_nof_nok / dl_total));
    } else {
      fmt::format_to(buffer, " dl_error_rate={}%", 0);
    }
    fmt::format_to(buffer, " dl_bs={}", scaled_fmt_integer(ue.dl_bs, false));

    if (!std::isnan(ue.pusch_snr_db) && !iszero(ue.pusch_snr_db)) {
      fmt::format_to(buffer, " pusch_snr_db={:.1f}", std::clamp(ue.pusch_snr_db, -99.9f, 99.9f));
    } else {
      fmt::format_to(buffer, " pusch_snr_db=n/a");
    }

    if (!std::isinf(ue.pusch_rsrp_db) && !std::isnan(ue.pusch_rsrp_db)) {
      if (ue.pusch_rsrp_db >= 0.0F) {
        fmt::format_to(buffer, " pusch_rsrp_db=ovl");
      } else {
        fmt::format_to(buffer, " pusch_rsrp_db={:.1f}", std::clamp(ue.pusch_rsrp_db, -99.9F, 0.0F));
      }
    } else {
      fmt::format_to(buffer, " pusch_rsrp_db=n/a");
    }

    fmt::format_to(buffer, " ul_mcs={}", ue.ul_mcs.to_uint());
    if (ue.ul_brate_kbps > 0) {
      fmt::format_to(buffer, " ul_brate_kbps={}", float_to_eng_string(ue.ul_brate_kbps * 1e3, 1, false));
    } else {
      fmt::format_to(buffer, " ul_brate_kbps={}", 0);
    }
    fmt::format_to(buffer, " ul_nof_ok={}", ue.ul_nof_ok);
    fmt::format_to(buffer, " ul_nof_nok={}", ue.ul_nof_nok);

    unsigned ul_total = ue.ul_nof_ok + ue.ul_nof_nok;
    if (ul_total > 0) {
      fmt::format_to(buffer, " ul_error_rate={}%", int((float)100 * ue.ul_nof_nok / ul_total));
    } else {
      fmt::format_to(buffer, " ul_error_rate={}%", 0);
    }
    fmt::format_to(buffer, " bsr={}", scaled_fmt_integer(ue.bsr, false));
    if (ue.last_ta.has_value()) {
      fmt::format_to(buffer, " last_ta={}s", float_to_eng_string(ue.last_ta->to_seconds<float>(), 0, false));
    } else {
      fmt::format_to(buffer, " last_ta=n/a");
    }
    if (ue.last_phr.has_value()) {
      fmt::format_to(buffer, " last_phr={}", ue.last_phr.value());
    } else {
      fmt::format_to(buffer, " last_phr=n/a");
    }

    logger.info("{}", to_c_str(buffer));
    buffer.clear();
  }
}

void metrics_log_helper::report_metrics(const rlc_metrics& metrics)
{
  if (!logger.debug.enabled()) {
    return;
  }
  fmt::memory_buffer buffer;
  fmt::format_to(buffer, "RLC Metrics:");
  fmt::format_to(buffer, " du={}", metrics.du_index);
  fmt::format_to(buffer, " ue={}", metrics.ue_index);
  fmt::format_to(buffer, " rb={}", metrics.rb_id);
  fmt::format_to(buffer, " mode={}", metrics.rx.mode);
  fmt::format_to(buffer, " TX=[{}]", format_rlc_tx_metrics(metrics.metrics_period, metrics.tx));
  fmt::format_to(buffer, " RX=[{}]  ", format_rlc_rx_metrics(metrics.metrics_period, metrics.rx));
  logger.debug("{}", to_c_str(buffer));
}
