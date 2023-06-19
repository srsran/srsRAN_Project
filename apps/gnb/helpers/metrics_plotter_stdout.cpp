/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include <float.h>
#include <iomanip>
#include <sstream>

using namespace srsran;

void metrics_plotter_stdout::print_header()
{
  fmt::print("\n");
  fmt::print("           -----------------DL----------------|------------------UL--------------------\n");
  fmt::print(" pci rnti  cqi  ri  mcs  brate   ok  nok  (%) | pusch  mcs  brate   ok  nok  (%)    bsr\n");
}

void metrics_plotter_stdout::report_metrics(span<const scheduler_ue_metrics> ue_metrics)
{
  if (not print_metrics) {
    return;
  }

  if (++nof_lines > 10 && !ue_metrics.empty()) {
    nof_lines = 0;
    print_header();
  }

  for (const auto& ue : ue_metrics) {
    fmt::print("{:>4}", ue.pci);
    fmt::print("{:>5x}", ue.rnti);
    if (not iszero(ue.cqi)) {
      fmt::print("  {:>3}", int(ue.cqi));
    } else {
      fmt::print("  {:>3.3}", "n/a");
    }

    fmt::print("  {:>2}", int(ue.ri));

    if (not std::isnan(ue.dl_mcs.to_uint())) {
      fmt::print("   {:>2}", int(ue.dl_mcs.to_uint()));
    } else {
      fmt::print("   {:>2}", 0);
    }
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

    fmt::print(" |");

    if (not std::isnan(ue.pusch_snr_db) and not iszero(ue.pusch_snr_db)) {
      fmt::print(" {:>5.1f}", clamp(ue.pusch_snr_db, -99.9f, 99.9f));
    } else {
      fmt::print(" {:>5.5}", "n/a");
    }

    if (not std::isnan(ue.ul_mcs.to_uint())) {
      fmt::print("   {:>2}", ue.ul_mcs.to_uint());
    } else {
      fmt::print("   {:>2}", 0);
    }
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
    fmt::print(" {:>6.6}", float_to_eng_string(ue.bsr, 2));

    fmt::print("\n");
  }
}

void metrics_plotter_stdout::toggle_print()
{
  print_metrics = !print_metrics;
}

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

std::string metrics_plotter_stdout::float_to_string(float f, int digits, int field_width)
{
  std::ostringstream os;
  int                precision;
  if (std::isnan(f) or std::fabs(f) < 0.0001) {
    f         = 0.0;
    precision = digits - 1;
  } else {
    precision = digits - (int)(log10f(fabs(f + 0.0001)) - 2 * DBL_EPSILON);
  }
  if (precision == -1) {
    precision = 0;
  }
  os << std::setw(field_width) << std::fixed << std::setprecision(precision) << f;
  return os.str();
}

std::string metrics_plotter_stdout::float_to_eng_string(float f, int digits)
{
  const int degree = (f == 0.0) ? 0 : lrint(floor(log10f(fabs(f)) / 3));

  std::string factor;

  if (abs(degree) < 9) {
    if (degree < 0)
      factor = prefixes[0][abs(degree)];
    else
      factor = prefixes[1][abs(degree)];
  } else {
    return "failed";
  }

  const double scaled = f * pow(1000.0, -degree);
  if (degree != 0) {
    return float_to_string(scaled, digits, 5) + factor;
  } else {
    return " " + float_to_string(scaled, digits, 5 - factor.length()) + factor;
  }
}