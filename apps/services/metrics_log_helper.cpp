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

static std::string scaled_fmt_integer(uint64_t num)
{
  static constexpr std::array<const char*, 8> suffixes = {"", "k", "M", "G", "T", "P", "E", "Z"};
  static const std::array<uint64_t, 8>        max_nums = []() {
    std::array<uint64_t, 8> nums{0};
    for (unsigned i = 0, e = nums.size(); i != e; ++i) {
      nums[i] = (uint64_t)std::pow(10, i * 3);
    }
    return nums;
  }();

  if (num < max_nums[1]) {
    return fmt::format("{}", num);
  }

  for (unsigned i = 1, e = max_nums.size() - 1; i != e; ++i) {
    if (num < max_nums[i + 1]) {
      return fmt::format("{:.3g}{}", num / static_cast<double>(max_nums[i]), suffixes[i]);
    }
  }

  return "Invalid number";
}

static std::string float_to_string(float f, int digits, int field_width)
{
  std::ostringstream os;
  int                precision;

  if (std::isnan(f) || std::abs(f) < 0.0001f) {
    f         = 0.f;
    precision = digits - 1;
  } else {
    precision = digits - (int)(std::log10(std::abs(f + 0.0001f)) - 2 * DBL_EPSILON);
  }

  precision = std::max(precision, 0);

  os << std::fixed << std::setprecision(precision) << f;
  return os.str();
}

static std::string float_to_eng_string(float f, int digits)
{
  static char const* const prefixes[2][9] = {
      {
          "",
          "m",
          "u",
          "n",
          "p",
          "f",
          "a",
          "z",
          "y",
      },
      {
          "",
          "k",
          "M",
          "G",
          "T",
          "P",
          "E",
          "Z",
          "Y",
      },
  };

  const int degree = (f == 0.f) ? 0 : std::lrint(std::floor(std::log10(std::abs(f)) / 3));

  std::string factor;

  if (std::abs(degree) < 9) {
    factor = prefixes[(degree < 0) ? 0 : 1][std::abs(degree)];
  } else {
    return "failed";
  }

  const double scaled = f * std::pow(1000.0, -degree);
  if (degree != 0) {
    return float_to_string(scaled, digits, 5) + factor;
  }
  return float_to_string(scaled, digits, 5 - factor.length()) + factor;
}

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
      fmt::format_to(buffer, " dl_brate_kbps={}", float_to_eng_string(ue.dl_brate_kbps * 1e3, 1));
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
    fmt::format_to(buffer, " dl_bs={}", scaled_fmt_integer(ue.dl_bs));

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
      fmt::format_to(buffer, " ul_brate_kbps={}", float_to_eng_string(ue.ul_brate_kbps * 1e3, 1));
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
    fmt::format_to(buffer, " bsr={}", scaled_fmt_integer(ue.bsr));
    if (ue.last_ta.has_value()) {
      fmt::format_to(buffer, " last_ta={}s", float_to_eng_string(ue.last_ta->to_seconds<float>(), 0));
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
  fmt::format_to(buffer, " du_index={}", metrics.du_index);
  fmt::format_to(buffer, " ue_index={}", metrics.ue_index);
  fmt::format_to(buffer, " rb_id={}", metrics.rb_id);
  fmt::format_to(buffer, " TX=[{}]", metrics.tx);
  fmt::format_to(buffer, " RX=[{}]  ", metrics.rx);
  logger.debug("{}", to_c_str(buffer));
}
