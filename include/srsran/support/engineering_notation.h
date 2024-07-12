
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

#pragma once

#include "fmt/core.h"
#include <array>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <string>

namespace srsran {

/// \file
/// \brief Helper functions to print numbers in engineering notation, for pretty printing.

inline std::string scaled_fmt_integer(uint64_t num, bool right_align)
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
    return right_align ? fmt::format("{:>6}", num) : fmt::format("{}", num);
  }

  for (unsigned i = 1, e = max_nums.size() - 1; i != e; ++i) {
    if (num < max_nums[i + 1]) {
      if (right_align) {
        return fmt::format("{:>5.3g}{}", num / static_cast<double>(max_nums[i]), suffixes[i]);
      }
      return fmt::format("{:.3g}{}", num / static_cast<double>(max_nums[i]), suffixes[i]);
    }
  }

  return "Invalid number";
}

inline std::string float_to_string(float f, int digits, int field_width, bool align_right)
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

  if (align_right) {
    os << std::setw(field_width) << std::fixed << std::setprecision(precision) << f;
  } else {
    os << std::fixed << std::setprecision(precision) << f;
  }
  return os.str();
}

inline std::string float_to_eng_string(float f, int digits, bool align_right)
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
    return float_to_string(scaled, digits, 5, align_right) + factor;
  }
  return " " + float_to_string(scaled, digits, 5 - factor.length(), align_right) + factor;
}
} // namespace srsran
