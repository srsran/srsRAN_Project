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

#include "du_high_scheduler_cell_metrics_consumers.h"
#include "du_high_rlc_metrics.h"
#include "srsran/scheduler/scheduler_metrics.h"
#include "srsran/support/engineering_notation.h"
#include "srsran/support/format_utils.h"
#include "srsran/support/math_utils.h"
#include <iomanip>
#include <ostream>

using namespace srsran;

namespace {

/// UE container metrics.
DECLARE_METRIC("pci", metric_pci, pci_t, "");
DECLARE_METRIC("rnti", metric_rnti, uint16_t, "");
DECLARE_METRIC("cqi", metric_cqi, uint8_t, "");
DECLARE_METRIC("ri", metric_ri, float, "");
DECLARE_METRIC("dl_mcs", metric_dl_mcs, uint8_t, "");
DECLARE_METRIC("dl_brate", metric_dl_brate, double, "");
DECLARE_METRIC("dl_nof_ok", metric_dl_nof_ok, unsigned, "");
DECLARE_METRIC("dl_nof_nok", metric_dl_nof_nok, unsigned, "");
DECLARE_METRIC("dl_bs", metric_dl_bs, unsigned, "");
DECLARE_METRIC("pusch_snr_db", metric_pusch_snr_db, float, "");
DECLARE_METRIC("pucch_snr_db", metric_pucch_snr_db, float, "");
DECLARE_METRIC("ta_ns", metric_ta_ns, std::string, "");
DECLARE_METRIC("ul_mcs", metric_ul_mcs, uint8_t, "");
DECLARE_METRIC("ul_brate", metric_ul_brate, double, "");
DECLARE_METRIC("ul_nof_ok", metric_ul_nof_ok, unsigned, "");
DECLARE_METRIC("ul_nof_nok", metric_ul_nof_nok, unsigned, "");
DECLARE_METRIC("bsr", metric_bsr, unsigned, "");
DECLARE_METRIC_SET("ue_container",
                   mset_ue_container,
                   metric_pci,
                   metric_rnti,
                   metric_cqi,
                   metric_ri,
                   metric_dl_mcs,
                   metric_dl_brate,
                   metric_dl_nof_ok,
                   metric_dl_nof_nok,
                   metric_dl_bs,
                   metric_pusch_snr_db,
                   metric_pucch_snr_db,
                   metric_ta_ns,
                   metric_ul_mcs,
                   metric_ul_brate,
                   metric_ul_nof_ok,
                   metric_ul_nof_nok,
                   metric_bsr);

// Cell event metrics.
DECLARE_METRIC("sfn", metric_sfn, uint16_t, "");
DECLARE_METRIC("slot_index", metric_slot_index, uint16_t, "");
DECLARE_METRIC("event_type", metric_event_type, std::string, "");
DECLARE_METRIC_SET("cell_events", mset_cell_event, metric_sfn, metric_slot_index, metric_rnti, metric_event_type);

/// cell-wide metrics.
DECLARE_METRIC("error_indication_count", metric_error_indication_count, unsigned, "");
DECLARE_METRIC("average_latency", metric_average_latency, unsigned, "");
DECLARE_METRIC("latency_histogram", latency_histogram, std::vector<unsigned>, "");
DECLARE_METRIC_SET("cell_metrics",
                   cell_metrics,
                   metric_error_indication_count,
                   metric_average_latency,
                   latency_histogram);

/// Metrics root object.
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");
DECLARE_METRIC_LIST("ue_list", mlist_ues, std::vector<mset_ue_container>);
DECLARE_METRIC_LIST("event_list", mlist_events, std::vector<mset_cell_event>);

/// Metrics context.
using metric_context_t = srslog::build_context_type<metric_timestamp_tag, cell_metrics, mlist_ues, mlist_events>;

} // namespace

static void print_header()
{
  fmt::print("\n");
  fmt::print(
      "          "
      "|--------------------DL---------------------|-------------------------UL------------------------------\n");
  fmt::print(" pci rnti | cqi  ri  mcs  brate   ok  nok  (%)  dl_bs | pusch  rsrp  mcs  brate   ok  nok  (%)    bsr    "
             " ta  phr\n");
}

static const char* event_to_string(scheduler_cell_event::event_type ev)
{
  switch (ev) {
    case scheduler_cell_event::event_type::ue_add:
      return "ue_create";
    case scheduler_cell_event::event_type::ue_reconf:
      return "ue_reconf";
    case scheduler_cell_event::event_type::ue_rem:
      return "ue_rem";
    default:
      break;
  }
  return "invalid";
}

void scheduler_cell_metrics_consumer_stdout::handle_metric(const app_services::metrics_set& metric)
{
  if (!print_metrics) {
    return;
  }

  const scheduler_cell_metrics& metrics = static_cast<const scheduler_cell_metrics_impl&>(metric).get_metrics();

  if (metrics.ue_metrics.size() > 10) {
    print_header();
  } else if (++nof_lines > 10 && !metrics.ue_metrics.empty()) {
    nof_lines = 0;
    print_header();
  }

  for (const auto& ue : metrics.ue_metrics) {
    fmt::print("{:>4}", ue.pci);
    fmt::print("{:>5x}", to_value(ue.rnti));

    if (ue.cqi_stats.get_nof_observations() > 0) {
      fmt::print(" | {:>3}", static_cast<unsigned>(std::roundf(ue.cqi_stats.get_mean())));
    } else {
      fmt::print(" | {:>3.3}", "n/a");
    }

    if (ue.ri_stats.get_nof_observations() > 0) {
      fmt::print(" {:>3.1f}", ue.ri_stats.get_mean());
    } else {
      fmt::print(" {:>3.3}", "n/a");
    }

    fmt::print("   {:>2}", int(ue.dl_mcs.to_uint()));
    if (ue.dl_brate_kbps > 0) {
      fmt::print(" {:>6.6}", float_to_eng_string(ue.dl_brate_kbps * 1e3, 1, true));
    } else {
      fmt::print(" {:>6}", 0);
    }
    fmt::print(" {:>4}", ue.dl_nof_ok);
    fmt::print(" {:>4}", ue.dl_nof_nok);
    unsigned dl_total = ue.dl_nof_ok + ue.dl_nof_nok;
    if (dl_total > 0) {
      fmt::print(" {:>3}%", int((float)100 * ue.dl_nof_nok / dl_total));
    } else {
      fmt::print(" {:>3}%", 0);
    }
    fmt::print(" {}", scaled_fmt_integer(ue.dl_bs, true));

    fmt::print(" |");

    if (!std::isnan(ue.pusch_snr_db) && !iszero(ue.pusch_snr_db)) {
      fmt::print(" {:>5.1f}", std::clamp(ue.pusch_snr_db, -99.9f, 99.9f));
    } else {
      fmt::print(" {:>5.5}", "n/a");
    }

    if (!std::isinf(ue.pusch_rsrp_db) && !std::isnan(ue.pusch_rsrp_db)) {
      if (ue.pusch_rsrp_db >= 0.0F) {
        fmt::print(" {:>5.5}", "ovl");
      } else {
        fmt::print(" {:>5.1f}", std::clamp(ue.pusch_rsrp_db, -99.9F, 0.0F));
      }
    } else {
      fmt::print(" {:>5.5}", "n/a");
    }

    fmt::print("   {:>2}", ue.ul_mcs.to_uint());
    if (ue.ul_brate_kbps > 0) {
      fmt::print(" {:>6.6}", float_to_eng_string(ue.ul_brate_kbps * 1e3, 1, true));
    } else {
      fmt::print(" {:>6}", 0);
    }
    fmt::print(" {:>4}", ue.ul_nof_ok);
    fmt::print(" {:>4}", ue.ul_nof_nok);

    unsigned ul_total = ue.ul_nof_ok + ue.ul_nof_nok;
    if (ul_total > 0) {
      fmt::print(" {:>3}%", int((float)100 * ue.ul_nof_nok / ul_total));
    } else {
      fmt::print(" {:>3}%", 0);
    }
    fmt::print(" {}", scaled_fmt_integer(ue.bsr, true));
    if (ue.last_ta.has_value()) {
      fmt::print(" {}", float_to_eng_string(ue.last_ta->to_seconds<float>(), 0, true));
    } else {
      fmt::print("   n/a");
    }
    if (ue.last_phr.has_value()) {
      fmt::print(" {:>4}", ue.last_phr.value());
    } else {
      fmt::print("  n/a");
    }

    fmt::print("\n");
  }
}

/// Returns the current time in seconds with ms precision since UNIX epoch.
static double get_time_stamp()
{
  auto tp = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(tp).count() * 1e-3;
}

void scheduler_cell_metrics_consumer_json::handle_metric(const app_services::metrics_set& metric)
{
  const scheduler_cell_metrics& metrics = static_cast<const scheduler_cell_metrics_impl&>(metric).get_metrics();

  metric_context_t ctx("JSON Metrics");

  for (const auto& ue : metrics.ue_metrics) {
    ctx.get<mlist_ues>().emplace_back();
    auto& output = ctx.get<mlist_ues>().back();

    output.write<metric_pci>(ue.pci);
    output.write<metric_rnti>(to_value(ue.rnti));
    if (ue.cqi_stats.get_nof_observations() > 0) {
      output.write<metric_cqi>(static_cast<uint8_t>(std::roundf(ue.cqi_stats.get_mean())));
    }
    if (ue.ri_stats.get_nof_observations() > 0) {
      output.write<metric_ri>(ue.ri_stats.get_mean());
    }
    output.write<metric_dl_mcs>(ue.dl_mcs.to_uint());
    output.write<metric_dl_brate>(ue.dl_brate_kbps * 1e3);
    output.write<metric_dl_nof_ok>(ue.dl_nof_ok);
    output.write<metric_dl_nof_nok>(ue.dl_nof_nok);
    output.write<metric_dl_bs>(ue.dl_bs);
    if (!std::isnan(ue.pusch_snr_db) && !iszero(ue.pusch_snr_db)) {
      output.write<metric_pusch_snr_db>(std::clamp(ue.pusch_snr_db, -99.9f, 99.9f));
    }
    if (!std::isnan(ue.pucch_snr_db) && !iszero(ue.pucch_snr_db)) {
      output.write<metric_pucch_snr_db>(std::clamp(ue.pucch_snr_db, -99.9f, 99.9f));
    }
    output.write<metric_ta_ns>(ue.last_ta ? std::to_string(ue.last_ta->to_seconds<float>() * 1e9) : "n/a");
    output.write<metric_ul_mcs>(ue.ul_mcs.to_uint());
    output.write<metric_ul_brate>(ue.ul_brate_kbps * 1e3);
    output.write<metric_ul_nof_ok>(ue.ul_nof_ok);
    output.write<metric_ul_nof_nok>(ue.ul_nof_nok);
    output.write<metric_bsr>(ue.bsr);
  }

  for (const auto& event : metrics.events) {
    ctx.get<mlist_events>().emplace_back();
    auto& output = ctx.get<mlist_events>().back();

    output.write<metric_sfn>(event.slot.sfn());
    output.write<metric_slot_index>(event.slot.slot_index());
    output.write<metric_rnti>(to_value(event.rnti));
    output.write<metric_event_type>(event_to_string(event.type));
  }

  auto& cell_output = ctx.get<cell_metrics>();
  cell_output.write<metric_error_indication_count>(metrics.nof_error_indications);
  cell_output.write<metric_average_latency>(metrics.average_decision_latency.count());
  cell_output.write<latency_histogram>(
      std::vector<unsigned>(metrics.latency_histogram.begin(), metrics.latency_histogram.end()));

  // Log the context.
  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_chan(ctx);
}

void scheduler_cell_metrics_consumer_log::handle_metric(const app_services::metrics_set& metric)
{
  const scheduler_cell_metrics& metrics = static_cast<const scheduler_cell_metrics_impl&>(metric).get_metrics();

  fmt::memory_buffer buffer;

  // log cell-wide metrics
  fmt::format_to(buffer, "Cell Scheduler Metrics:");
  fmt::format_to(buffer,
                 " error_indications={} mean_latency={}usec latency_hist=[{}]",
                 metrics.nof_error_indications,
                 metrics.average_decision_latency.count(),
                 fmt::join(metrics.latency_histogram.begin(), metrics.latency_histogram.end(), ", "));
  if (not metrics.events.empty()) {
    fmt::format_to(buffer, " events=[");
    bool first = true;
    for (const auto& event : metrics.events) {
      fmt::format_to(buffer,
                     "{}{{rnti={} slot={} type={}}}",
                     first ? "" : ", ",
                     event.rnti,
                     event.slot,
                     event_to_string(event.type));
      first = false;
    }
    fmt::format_to(buffer, "]");
  }
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

void scheduler_cell_metrics_consumer_e2::handle_metric(const app_services::metrics_set& metric)
{
  notifier.report_metrics(static_cast<const scheduler_cell_metrics_impl&>(metric).get_metrics());
}
