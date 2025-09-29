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

#include "scheduler_metrics_consumers.h"
#include "srsran/support/engineering_notation.h"
#include "srsran/support/format/custom_formattable.h"
#include "srsran/support/format/fmt_to_c_str.h"

using namespace srsran;

static void print_sched_header()
{
  fmt::print("\n");
  fmt::print(
      "          "
      "|--------------------DL---------------------|-------------------------UL----------------------------------\n");
  fmt::print(" pci rnti | cqi  ri  mcs  brate   ok  nok  (%)  dl_bs | pusch  rsrp  ri  mcs  brate   ok  nok  (%)    "
             "bsr     ta  phr\n");
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

void scheduler_cell_metrics_consumer_stdout::handle_metric(const std::optional<scheduler_metrics_report>& report)
{
  if (!report) {
    return;
  }

  for (const auto& cell : report->cells) {
    if (cell.ue_metrics.empty()) {
      continue;
    }

    if (cell.ue_metrics.size() > MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER) {
      print_sched_header();
    } else if (++nof_lines > MAX_NOF_STDOUT_METRIC_LINES_WITHOUT_HEADER && !cell.ue_metrics.empty()) {
      nof_lines = 0;
      print_sched_header();
    }

    for (const auto& ue : cell.ue_metrics) {
      fmt::print("{:>4}", ue.pci);
      fmt::print("{:>5x}", to_value(ue.rnti));

      if (ue.cqi_stats.get_nof_observations() > 0) {
        fmt::print(" | {:>3}", static_cast<unsigned>(std::round(ue.cqi_stats.get_mean())));
      } else {
        fmt::print(" | {:>3.3}", "n/a");
      }

      if (ue.dl_ri_stats.get_nof_observations() > 0) {
        fmt::print(" {:>3.1f}", ue.dl_ri_stats.get_mean());
      } else {
        fmt::print(" {:>3}", "n/a");
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

      if (ue.ul_ri_stats.get_nof_observations() > 0) {
        fmt::print(" {:>3.1f}", ue.ul_ri_stats.get_mean());
      } else {
        fmt::print("   1");
      }

      fmt::print("   {:>2}", ue.ul_mcs.to_uint());
      if (ue.ul_brate_kbps > 0) {
        fmt::print(" {:>6.6}", float_to_eng_string(ue.ul_brate_kbps * 1e3, 2, true));
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
      if (ue.srs_ta_stats.get_nof_observations() > 0) {
        fmt::print(" {}", float_to_eng_string(ue.srs_ta_stats.get_mean(), 0, true));
      } else if (ue.pusch_ta_stats.get_nof_observations() > 0) {
        fmt::print(" {}", float_to_eng_string(ue.pusch_ta_stats.get_mean(), 0, true));
      } else if (ue.pucch_ta_stats.get_nof_observations() > 0) {
        fmt::print(" {}", float_to_eng_string(ue.pucch_ta_stats.get_mean(), 0, true));
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
}

template <typename ResultType>
static ResultType to_percentage(unsigned numerator, unsigned denominator)
{
  return static_cast<ResultType>(100.0 * static_cast<double>(numerator) / static_cast<double>(denominator));
}

void scheduler_cell_metrics_consumer_log::handle_metric(const std::optional<scheduler_metrics_report>& report)
{
  if (!report) {
    return;
  }

  for (const auto& cell : report->cells) {
    fmt::memory_buffer buffer;

    unsigned sum_dl_bitrate_kbps = 0;
    unsigned sum_ul_bitrate_kbps = 0;
    unsigned sum_pdsch_rbs       = 0;
    unsigned sum_pusch_rbs       = 0;
    float    max_crc_delay       = std::numeric_limits<float>::min();
    float    max_ce_delay        = std::numeric_limits<float>::min();
    float    max_pucch_delay     = std::numeric_limits<float>::min();
    float    max_pusch_delay     = std::numeric_limits<float>::min();
    for (const auto& ue : cell.ue_metrics) {
      sum_dl_bitrate_kbps += ue.dl_brate_kbps;
      sum_ul_bitrate_kbps += ue.ul_brate_kbps;
      sum_pdsch_rbs += ue.tot_pdsch_prbs_used;
      sum_pusch_rbs += ue.tot_pusch_prbs_used;
      if (ue.max_crc_delay_ms.has_value()) {
        max_crc_delay = std::max(max_crc_delay, ue.max_crc_delay_ms.value());
      }
      if (ue.max_ce_delay_ms.has_value()) {
        max_ce_delay = std::max(max_ce_delay, ue.max_ce_delay_ms.value());
      }
      if (ue.max_pucch_harq_delay_ms.has_value()) {
        max_pucch_delay = std::max(max_pucch_delay, ue.max_pucch_harq_delay_ms.value());
      }
      if (ue.max_pusch_harq_delay_ms.has_value()) {
        max_pusch_delay = std::max(max_pusch_delay, ue.max_pusch_harq_delay_ms.value());
      }
    }

    // log cell-wide metrics
    fmt::format_to(
        std::back_inserter(buffer),
        "Scheduler cell pci={} metrics:"
        " total_dl_brate={}bps total_ul_brate={}bps nof_prbs={} nof_dl_slots={} nof_ul_slots={} nof_prach_preambles={} "
        "error_indications={} pdsch_rbs_per_slot={} pusch_rbs_per_slot={} pdschs_per_slot={:.3} puschs_per_slot={:.3} "
        "failed_pdcch={} failed_uci={} nof_ues={} mean_latency={}usec max_latency={}usec max_latency_slot={} "
        "latency_hist=[{}] msg3_ok={} msg3_nok={} late_dl_harqs={} late_ul_harqs={} pucch_tot_rb_usage_avg={:.3}",
        cell.pci,
        float_to_eng_string(sum_dl_bitrate_kbps * 1e3, 2, false),
        float_to_eng_string(sum_ul_bitrate_kbps * 1e3, 2, false),
        cell.nof_prbs,
        cell.nof_dl_slots,
        cell.nof_ul_slots,
        cell.nof_prach_preambles,
        cell.nof_error_indications,
        cell.nof_dl_slots > 0 ? sum_pdsch_rbs / cell.nof_dl_slots : 0,
        cell.nof_ul_slots > 0 ? sum_pusch_rbs / cell.nof_ul_slots : 0,
        cell.nof_dl_slots > 0 ? cell.dl_grants_count / (float)cell.nof_dl_slots : 0,
        cell.nof_ul_slots > 0 ? cell.ul_grants_count / (float)cell.nof_ul_slots : 0,
        cell.nof_failed_pdcch_allocs,
        cell.nof_failed_uci_allocs,
        cell.ue_metrics.size(),
        cell.average_decision_latency.count(),
        cell.max_decision_latency.count(),
        cell.max_decision_latency_slot,
        fmt::join(cell.latency_histogram.begin(), cell.latency_histogram.end(), ", "),
        cell.nof_msg3_ok,
        cell.nof_msg3_nok,
        cell.nof_failed_pdsch_allocs_late_harqs,
        cell.nof_failed_pusch_allocs_late_harqs,
        cell.pucch_tot_rb_usage_avg);
    if (max_crc_delay != std::numeric_limits<float>::min()) {
      fmt::format_to(std::back_inserter(buffer), " max_crc_delay={}ms", max_crc_delay);
    }
    if (max_ce_delay != std::numeric_limits<float>::min()) {
      fmt::format_to(std::back_inserter(buffer), " max_ce_delay={}ms", max_ce_delay);
    }
    if (max_pucch_delay != std::numeric_limits<float>::min()) {
      fmt::format_to(std::back_inserter(buffer), " max_pucch_harq_delay={}ms", max_pucch_delay);
    }
    if (max_pusch_delay != std::numeric_limits<float>::min()) {
      fmt::format_to(std::back_inserter(buffer), " max_pusch_harq_delay={}ms", max_pusch_delay);
    }
    fmt::format_to(
        std::back_inserter(buffer), " avg_prach_delay={:.3}", format_value_or(cell.avg_prach_delay_slots, "n/a"));
    if (not cell.events.empty()) {
      fmt::format_to(std::back_inserter(buffer), " events=[");
      bool first = true;
      for (const auto& event : cell.events) {
        fmt::format_to(std::back_inserter(buffer),
                       "{}{{rnti={} slot={} type={}}}",
                       first ? "" : ", ",
                       event.rnti,
                       event.slot,
                       event_to_string(event.type));
        first = false;
      }
      if (cell.nof_filtered_events > 0) {
        fmt::format_to(std::back_inserter(buffer), ", ... ({} remaining events) ...", cell.nof_filtered_events);
      }
      fmt::format_to(std::back_inserter(buffer), "]");
    }
    log_chan("{}", to_c_str(buffer));
    buffer.clear();

    // log ue-specific metrics
    for (const auto& ue : cell.ue_metrics) {
      fmt::format_to(std::back_inserter(buffer), "Scheduler UE pci={} rnti={} metrics:", ue.pci, ue.rnti);
      if (ue.cqi_stats.get_nof_observations() > 0) {
        fmt::format_to(
            std::back_inserter(buffer), " cqi={}", static_cast<unsigned>(std::round(ue.cqi_stats.get_mean())));
      } else {
        fmt::format_to(std::back_inserter(buffer), " cqi=n/a");
      }

      if (ue.dl_ri_stats.get_nof_observations() > 0) {
        fmt::format_to(std::back_inserter(buffer), " dl_ri={:.1f}", ue.dl_ri_stats.get_mean());
      } else {
        fmt::format_to(std::back_inserter(buffer), " dl_ri=n/a");
      }

      fmt::format_to(std::back_inserter(buffer), " dl_mcs={}", int(ue.dl_mcs.to_uint()));
      if (ue.dl_brate_kbps > 0) {
        fmt::format_to(
            std::back_inserter(buffer), " dl_brate={}bps", float_to_eng_string(ue.dl_brate_kbps * 1e3, 2, false));
      } else {
        fmt::format_to(std::back_inserter(buffer), " dl_brate={}bps", 0);
      }
      fmt::format_to(std::back_inserter(buffer), " dl_nof_ok={}", ue.dl_nof_ok);
      fmt::format_to(std::back_inserter(buffer), " dl_nof_nok={}", ue.dl_nof_nok);
      unsigned dl_total = ue.dl_nof_ok + ue.dl_nof_nok;
      fmt::format_to(std::back_inserter(buffer),
                     " dl_error_rate={}%",
                     dl_total > 0 ? to_percentage<int>(ue.dl_nof_nok, dl_total) : 0);
      fmt::format_to(std::back_inserter(buffer), " dl_bs={}", scaled_fmt_integer(ue.dl_bs, false));
      fmt::format_to(std::back_inserter(buffer), " dl_nof_prbs={}", ue.tot_pdsch_prbs_used);
      if (ue.last_dl_olla.has_value()) {
        fmt::format_to(std::back_inserter(buffer), " dl_olla={:.4}", ue.last_dl_olla.value());
      }

      if (!std::isnan(ue.pusch_snr_db) && !iszero(ue.pusch_snr_db)) {
        fmt::format_to(std::back_inserter(buffer), " pusch_snr_db={:.1f}", std::clamp(ue.pusch_snr_db, -99.9f, 99.9f));
      } else {
        fmt::format_to(std::back_inserter(buffer), " pusch_snr_db=n/a");
      }

      if (!std::isinf(ue.pusch_rsrp_db) && !std::isnan(ue.pusch_rsrp_db)) {
        if (ue.pusch_rsrp_db >= 0.0F) {
          fmt::format_to(std::back_inserter(buffer), " pusch_rsrp_db=ovl");
        } else {
          fmt::format_to(
              std::back_inserter(buffer), " pusch_rsrp_db={:.1f}", std::clamp(ue.pusch_rsrp_db, -99.9F, 0.0F));
        }
      } else {
        fmt::format_to(std::back_inserter(buffer), " pusch_rsrp_db=n/a");
      }

      if (ue.ul_ri_stats.get_nof_observations() > 0) {
        fmt::format_to(std::back_inserter(buffer), " ul_ri={:.1f}", ue.ul_ri_stats.get_mean());
      } else {
        fmt::format_to(std::back_inserter(buffer), " ul_ri=1");
      }

      fmt::format_to(std::back_inserter(buffer), " ul_mcs={}", ue.ul_mcs.to_uint());
      if (ue.ul_brate_kbps > 0) {
        fmt::format_to(
            std::back_inserter(buffer), " ul_brate={}bps", float_to_eng_string(ue.ul_brate_kbps * 1e3, 2, false));
      } else {
        fmt::format_to(std::back_inserter(buffer), " ul_brate={}bps", 0);
      }
      fmt::format_to(std::back_inserter(buffer), " ul_nof_ok={}", ue.ul_nof_ok);
      fmt::format_to(std::back_inserter(buffer), " ul_nof_nok={}", ue.ul_nof_nok);

      unsigned ul_total = ue.ul_nof_ok + ue.ul_nof_nok;
      auto     out_it   = std::back_inserter(buffer);
      fmt::format_to(out_it, " ul_error_rate={}%", ul_total > 0 ? to_percentage<int>(ue.ul_nof_nok, ul_total) : 0);
      fmt::format_to(out_it, " ul_nof_prbs={}", ue.tot_pusch_prbs_used);
      fmt::format_to(out_it, " bsr={}", scaled_fmt_integer(ue.bsr, false));
      fmt::format_to(out_it, " sr_count={}", ue.sr_count);
      fmt::format_to(out_it, " f0f1_invalid_harqs={}", ue.nof_pucch_f0f1_invalid_harqs);
      fmt::format_to(out_it, " f2f3f4_invalid_harqs={}", ue.nof_pucch_f2f3f4_invalid_harqs);
      fmt::format_to(out_it, " f2f3f4_invalid_csis={}", ue.nof_pucch_f2f3f4_invalid_csis);
      fmt::format_to(out_it, " pusch_invalid_harqs={}", ue.nof_pusch_invalid_harqs);
      fmt::format_to(out_it, " pusch_invalid_csis={}", ue.nof_pusch_invalid_csis);
      if (ue.last_ul_olla.has_value()) {
        fmt::format_to(out_it, " ul_olla={:.4}", ue.last_ul_olla.value());
      }
      if (ue.ta_stats.get_nof_observations() > 0) {
        fmt::format_to(out_it, " ta={}s", float_to_eng_string(ue.ta_stats.get_mean(), 0, false));
      } else {
        fmt::format_to(out_it, " ta=n/a");
      }
      if (ue.srs_ta_stats.get_nof_observations() > 0) {
        fmt::format_to(out_it, " srs_ta={}s", float_to_eng_string(ue.srs_ta_stats.get_mean(), 0, false));
      } else {
        fmt::format_to(out_it, " srs_ta=n/a");
      }
      fmt::format_to(out_it, " last_phr={}", format_value_or(ue.last_phr, "n/a"));
      fmt::format_to(out_it, " avg_ul_ce_delay={:.3}", format_unit_or(ue.avg_ce_delay_ms, "ms", "n/a"));
      fmt::format_to(out_it, " max_ul_ce_delay={:.3}", format_unit_or(ue.max_ce_delay_ms, "ms", "n/a"));
      fmt::format_to(out_it, " avg_crc_delay={:.3}", format_unit_or(ue.avg_crc_delay_ms, "ms", "n/a"));
      fmt::format_to(out_it, " max_crc_delay={:.3}", format_unit_or(ue.max_crc_delay_ms, "ms", "n/a"));
      fmt::format_to(out_it, " avg_pusch_harq_delay={:.3}", format_unit_or(ue.avg_pusch_harq_delay_ms, "ms", "n/a"));
      fmt::format_to(out_it, " max_pusch_harq_delay={:.3}", format_unit_or(ue.max_pusch_harq_delay_ms, "ms", "n/a"));
      fmt::format_to(out_it, " avg_pucch_harq_delay={:.3}", format_unit_or(ue.avg_pucch_harq_delay_ms, "ms", "n/a"));
      fmt::format_to(out_it, " max_pucch_harq_delay={:.3}", format_unit_or(ue.max_pucch_harq_delay_ms, "ms", "n/a"));

      log_chan("{}", to_c_str(buffer));
      buffer.clear();
    }
  }
}

void scheduler_cell_metrics_consumer_e2::handle_metric(const std::optional<scheduler_metrics_report>& report)
{
  if (!report) {
    return;
  }

  for (const auto& cell : report->cells)
    notifier.report_metrics(cell);
}
