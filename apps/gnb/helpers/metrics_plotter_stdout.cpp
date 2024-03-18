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

#include "metrics_plotter_stdout.h"
#include "srsran/support/math_utils.h"
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
    return fmt::format("{:>6}", num);
  }

  for (unsigned i = 1, e = max_nums.size() - 1; i != e; ++i) {
    if (num < max_nums[i + 1]) {
      return fmt::format("{:>5.3g}{}", num / static_cast<double>(max_nums[i]), suffixes[i]);
    }
  }

  return "Invalid number";
}

static std::string scaled_time(std::chrono::microseconds t)
{
  if (t.count() < 1000) {
    return fmt::format("{:>4}us", t.count());
  }
  return fmt::format("{:>4}ms", std::chrono::duration_cast<std::chrono::milliseconds>(t).count());
}

static void print_header()
{
  fmt::print("\n");
  fmt::print(
      "          |--------------------DL---------------------|-------------------UL------------------------------\n");
  fmt::print(
      " pci rnti | cqi  ri  mcs  brate   ok  nok  (%)  dl_bs | pusch  mcs  brate   ok  nok  (%)    bsr    ta  phr\n");
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

  os << std::setw(field_width) << std::fixed << std::setprecision(precision) << f;
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
  return " " + float_to_string(scaled, digits, 5 - factor.length()) + factor;
}

void metrics_plotter_stdout::report_metrics(span<const scheduler_ue_metrics> ue_metrics)
{
  if (!print_metrics) {
    return;
  }

  if (ue_metrics.size() > 10) {
    print_header();
  } else if (++nof_lines > 10 && !ue_metrics.empty()) {
    nof_lines = 0;
    print_header();
  }

  for (const auto& ue : ue_metrics) {
    fmt::print("{:>4}", ue.pci);
    fmt::print("{:>5x}", to_value(ue.rnti));
    if (!iszero(ue.cqi)) {
      fmt::print(" | {:>3}", int(ue.cqi));
    } else {
      fmt::print(" | {:>3.3}", "n/a");
    }

    fmt::print("  {:>2}", int(ue.ri));

    fmt::print("   {:>2}", int(ue.dl_mcs.to_uint()));
    if (ue.dl_brate_kbps > 0) {
      fmt::print(" {:>6.6}", float_to_eng_string(ue.dl_brate_kbps * 1e3, 1));
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
    fmt::print(" {}", scaled_fmt_integer(ue.dl_bs));

    fmt::print(" |");

    if (!std::isnan(ue.pusch_snr_db) && !iszero(ue.pusch_snr_db)) {
      fmt::print(" {:>5.1f}", clamp(ue.pusch_snr_db, -99.9f, 99.9f));
    } else {
      fmt::print(" {:>5.5}", "n/a");
    }

    fmt::print("   {:>2}", ue.ul_mcs.to_uint());
    if (ue.ul_brate_kbps > 0) {
      fmt::print(" {:>6.6}", float_to_eng_string(ue.ul_brate_kbps * 1e3, 1));
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
    fmt::print(" {}", scaled_fmt_integer(ue.bsr));
    if (ue.last_ta.has_value()) {
      fmt::print("{}", scaled_time(ue.last_ta.value()));
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

void metrics_plotter_stdout::enable_print()
{
  print_metrics = true;
}

void metrics_plotter_stdout::toggle_print()
{
  print_metrics = !print_metrics;
}
