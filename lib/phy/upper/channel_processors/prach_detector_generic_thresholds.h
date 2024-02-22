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

/// \file
/// \brief List of thresholds for the PRACH detector.

#pragma once

#include "srsran/adt/span.h"
#include "srsran/adt/to_array.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/prach_subcarrier_spacing.h"

namespace srsran {
namespace detail {

/// Parameters that affect the detection threshold value.
struct threshold_params {
  /// Number of receive antenna ports.
  unsigned nof_rx_ports = 0;
  /// Subcarrier spacing.
  prach_subcarrier_spacing scs = prach_subcarrier_spacing::invalid;
  /// PRACH format type.
  prach_format_type format = prach_format_type::invalid;
  /// Zero correlation zone.
  unsigned zero_correlation_zone = 0;
  /// Flag to activate non-coherent combining of PRACH symbols.
  bool combine_symbols = false;
};

/// Overloaded less-than comparison operator.
inline bool operator<(const threshold_params& lhs, const threshold_params& rhs)
{
  if (lhs.nof_rx_ports < rhs.nof_rx_ports) {
    return true;
  }
  if (lhs.nof_rx_ports > rhs.nof_rx_ports) {
    return false;
  }

  // If we are here, nof_rx_ports is equal.
  if (lhs.scs < rhs.scs) {
    return true;
  }
  if (lhs.scs > rhs.scs) {
    return false;
  }

  // If we are here, nof_rx_ports and scs are equal.
  if (lhs.format < rhs.format) {
    return true;
  }
  if (lhs.format > rhs.format) {
    return false;
  }

  // If we are here, nof_rx_ports, scs and format are equal.
  if (lhs.zero_correlation_zone < rhs.zero_correlation_zone) {
    return true;
  }
  if (lhs.zero_correlation_zone > rhs.zero_correlation_zone) {
    return false;
  }

  // If we are here, nof_rx_ports, scs, format and zero_correlation_zone are equal.
  if (!lhs.combine_symbols && rhs.combine_symbols) {
    return true;
  }
  return false;
}

/// Overloaded equal-to comparison operator.
inline bool operator==(const threshold_params& lhs, const threshold_params& rhs)
{
  if (lhs.nof_rx_ports != rhs.nof_rx_ports) {
    return false;
  }

  // If we are here, nof_rx_ports is equal.
  if (lhs.scs != rhs.scs) {
    return false;
  }

  // If we are here, nof_rx_ports and scs are equal.
  if (lhs.format != rhs.format) {
    return false;
  }

  // If we are here, nof_rx_ports, scs and format are equal.
  if (lhs.zero_correlation_zone != rhs.zero_correlation_zone) {
    return false;
  }

  // If we are here, nof_rx_ports, scs, format and zero_correlation_zone are equal.
  if (lhs.combine_symbols != rhs.combine_symbols) {
    return false;
  }
  return true;
}

/// Manages the mapping between PRACH configuration and the (threshold, margin) pairs.
class threshold_and_margin_finder
{
public:
  /// Flag applied to each threshold entry.
  enum class threshold_flag {
    /// Threshold entry not suitable for proper PRACH detection.
    red,
    /// Threshold entry with suboptimal PRACH detection performance.
    orange,
    /// Threshold entry meeting PRACH detection requirements.
    green
  };

  using threshold_and_margin_type = std::pair<float, unsigned>;

  struct threshold_entry {
    threshold_params          configuration;
    threshold_and_margin_type threshold_and_margin;
    threshold_flag            flag;
  };

  /// Constructor: receives the list of pairings and ensures it is sorted.
  explicit threshold_and_margin_finder(span<const threshold_entry> in) :
    sorted_thresholds_and_margins(in.begin(), in.end())
  {
    std::sort(sorted_thresholds_and_margins.begin(),
              sorted_thresholds_and_margins.end(),
              [](const threshold_entry& a, const threshold_entry& b) { return (a.configuration < b.configuration); });
  }

  /// Retrieves the (threshold, margin) pair corresponding to the given configuration.
  threshold_and_margin_type get(const threshold_params& params) const
  {
    auto it =
        std::lower_bound(sorted_thresholds_and_margins.begin(),
                         sorted_thresholds_and_margins.end(),
                         params,
                         [](const threshold_entry& a, const threshold_params& b) { return (a.configuration < b); });

    if (it != sorted_thresholds_and_margins.end()) {
      return it->threshold_and_margin;
    }

    // todo(david): once all cases are covered, replace this by a srsran_assert.
    if (is_long_preamble(params.format)) {
      return {/* threshold */ 2.0F, /* margin */ 5};
    }
    return {/* threshold */ 0.3F, /* margin */ 12};
  }

  threshold_flag check_flag(const threshold_params& params) const
  {
    auto it =
        std::lower_bound(sorted_thresholds_and_margins.begin(),
                         sorted_thresholds_and_margins.end(),
                         params,
                         [](const threshold_entry& a, const threshold_params& b) { return (a.configuration < b); });

    if ((it != sorted_thresholds_and_margins.end()) && (it->configuration == params)) {
      return it->flag;
    }
    return threshold_flag::red;
  }

private:
  std::vector<threshold_entry> sorted_thresholds_and_margins;
};

using th_flag                               = threshold_and_margin_finder::threshold_flag;
static const auto all_threshold_and_margins = to_array<threshold_and_margin_finder::threshold_entry>({
    // clang-format off
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 0, /* combine symbols */ true}, {0.147F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 1, /* combine symbols */ true}, {1.000F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 2, /* combine symbols */ true}, {0.874F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 3, /* combine symbols */ true}, {0.774F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 4, /* combine symbols */ true}, {0.640F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 5, /* combine symbols */ true}, {0.551F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 6, /* combine symbols */ true}, {0.449F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 7, /* combine symbols */ true}, {0.388F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 8, /* combine symbols */ true}, {0.321F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 9, /* combine symbols */ true}, {0.255F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 10, /* combine symbols */ true}, {0.205F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 11, /* combine symbols */ true}, {0.167F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 12, /* combine symbols */ true}, {0.147F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 13, /* combine symbols */ true}, {0.148F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 14, /* combine symbols */ true}, {0.148F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 15, /* combine symbols */ true}, {0.148F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 0, /* combine symbols */ true}, {0.085F, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 1, /* combine symbols */ true}, {0.517F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 2, /* combine symbols */ true}, {0.451F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 3, /* combine symbols */ true}, {0.404F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 4, /* combine symbols */ true}, {0.344F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 5, /* combine symbols */ true}, {0.301F, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 6, /* combine symbols */ true}, {0.249F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 7, /* combine symbols */ true}, {0.219F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 8, /* combine symbols */ true}, {0.180F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 9, /* combine symbols */ true}, {0.146F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 10, /* combine symbols */ true}, {0.118F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 11, /* combine symbols */ true}, {0.098F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 12, /* combine symbols */ true}, {0.085F, 5}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 13, /* combine symbols */ true}, {0.086F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 14, /* combine symbols */ true}, {0.085F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 15, /* combine symbols */ true}, {0.085F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 0, /* combine symbols */ true}, {0.053F, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 1, /* combine symbols */ true}, {0.303F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 2, /* combine symbols */ true}, {0.269F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 3, /* combine symbols */ true}, {0.240F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 4, /* combine symbols */ true}, {0.207F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 5, /* combine symbols */ true}, {0.180F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 6, /* combine symbols */ true}, {0.149F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 7, /* combine symbols */ true}, {0.131F, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 8, /* combine symbols */ true}, {0.112F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 9, /* combine symbols */ true}, {0.090F, 5}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 10, /* combine symbols */ true}, {0.072F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 11, /* combine symbols */ true}, {0.060F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 12, /* combine symbols */ true}, {0.052F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 13, /* combine symbols */ true}, {0.052F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 14, /* combine symbols */ true}, {0.052F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::zero, /* ZCZ */ 15, /* combine symbols */ true}, {0.053F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 0, /* combine symbols */ true}, {0.025F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 1, /* combine symbols */ true}, {1.000F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 2, /* combine symbols */ true}, {0.869F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 3, /* combine symbols */ true}, {0.781F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 4, /* combine symbols */ true}, {0.636F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 5, /* combine symbols */ true}, {0.548F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 6, /* combine symbols */ true}, {0.455F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 7, /* combine symbols */ true}, {0.387F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 8, /* combine symbols */ true}, {0.328F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 9, /* combine symbols */ true}, {0.256F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 10, /* combine symbols */ true}, {0.205F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 11, /* combine symbols */ true}, {0.169F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 12, /* combine symbols */ true}, {0.134F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 13, /* combine symbols */ true}, {0.097F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 14, /* combine symbols */ true}, {0.060F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 15, /* combine symbols */ true}, {0.041F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 0, /* combine symbols */ true}, {0.014F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 1, /* combine symbols */ true}, {0.510F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 2, /* combine symbols */ true}, {0.459F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 3, /* combine symbols */ true}, {0.409F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 4, /* combine symbols */ true}, {0.341F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 5, /* combine symbols */ true}, {0.299F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 6, /* combine symbols */ true}, {0.250F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 7, /* combine symbols */ true}, {0.213F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 8, /* combine symbols */ true}, {0.183F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 9, /* combine symbols */ true}, {0.145F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 10, /* combine symbols */ true}, {0.118F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 11, /* combine symbols */ true}, {0.097F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 12, /* combine symbols */ true}, {0.078F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 13, /* combine symbols */ true}, {0.057F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 14, /* combine symbols */ true}, {0.035F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 15, /* combine symbols */ true}, {0.024F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 0, /* combine symbols */ true}, {0.009F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 1, /* combine symbols */ true}, {0.304F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 2, /* combine symbols */ true}, {0.269F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 3, /* combine symbols */ true}, {0.242F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 4, /* combine symbols */ true}, {0.206F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 5, /* combine symbols */ true}, {0.180F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 6, /* combine symbols */ true}, {0.150F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 7, /* combine symbols */ true}, {0.132F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 8, /* combine symbols */ true}, {0.112F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 9, /* combine symbols */ true}, {0.090F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 10, /* combine symbols */ true}, {0.072F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 11, /* combine symbols */ true}, {0.060F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 12, /* combine symbols */ true}, {0.048F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 13, /* combine symbols */ true}, {0.035F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 14, /* combine symbols */ true}, {0.022F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::one, /* ZCZ */ 15, /* combine symbols */ true}, {0.015F, 5}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 0, /* combine symbols */ true}, {0.100F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 1, /* combine symbols */ true}, {0.996F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 2, /* combine symbols */ true}, {0.882F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 3, /* combine symbols */ true}, {0.772F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 4, /* combine symbols */ true}, {0.658F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 5, /* combine symbols */ true}, {0.551F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 6, /* combine symbols */ true}, {0.450F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 7, /* combine symbols */ true}, {0.387F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 8, /* combine symbols */ true}, {0.329F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 9, /* combine symbols */ true}, {0.258F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 10, /* combine symbols */ true}, {0.207F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 11, /* combine symbols */ true}, {0.169F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 12, /* combine symbols */ true}, {0.134F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 13, /* combine symbols */ true}, {0.101F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 14, /* combine symbols */ true}, {0.101F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 15, /* combine symbols */ true}, {0.101F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 0, /* combine symbols */ true}, {0.059F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 1, /* combine symbols */ true}, {0.511F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 2, /* combine symbols */ true}, {0.452F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 3, /* combine symbols */ true}, {0.407F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 4, /* combine symbols */ true}, {0.345F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 5, /* combine symbols */ true}, {0.302F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 6, /* combine symbols */ true}, {0.246F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 7, /* combine symbols */ true}, {0.216F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 8, /* combine symbols */ true}, {0.180F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 9, /* combine symbols */ true}, {0.145F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 10, /* combine symbols */ true}, {0.116F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 11, /* combine symbols */ true}, {0.097F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 12, /* combine symbols */ true}, {0.077F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 13, /* combine symbols */ true}, {0.059F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 14, /* combine symbols */ true}, {0.058F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 15, /* combine symbols */ true}, {0.059F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 0, /* combine symbols */ true}, {0.036F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 1, /* combine symbols */ true}, {0.301F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 2, /* combine symbols */ true}, {0.270F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 3, /* combine symbols */ true}, {0.243F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 4, /* combine symbols */ true}, {0.208F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 5, /* combine symbols */ true}, {0.181F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 6, /* combine symbols */ true}, {0.150F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 7, /* combine symbols */ true}, {0.132F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 8, /* combine symbols */ true}, {0.111F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 9, /* combine symbols */ true}, {0.090F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 10, /* combine symbols */ true}, {0.072F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 11, /* combine symbols */ true}, {0.060F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 12, /* combine symbols */ true}, {0.048F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 13, /* combine symbols */ true}, {0.037F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 14, /* combine symbols */ true}, {0.036F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz1_25, prach_format_type::two, /* ZCZ */ 15, /* combine symbols */ true}, {0.036F, 5}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 0, /* combine symbols */ true}, {0.610F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 1, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 2, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 3, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 4, /* combine symbols */ true}, {0.995F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 5, /* combine symbols */ true}, {0.876F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 6, /* combine symbols */ true}, {0.787F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 7, /* combine symbols */ true}, {0.769F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 8, /* combine symbols */ true}, {0.690F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 9, /* combine symbols */ true}, {0.637F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 10, /* combine symbols */ true}, {0.600F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 11, /* combine symbols */ true}, {0.607F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 12, /* combine symbols */ true}, {0.604F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 13, /* combine symbols */ true}, {0.597F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 14, /* combine symbols */ true}, {0.603F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 15, /* combine symbols */ true}, {0.598F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 0, /* combine symbols */ true}, {0.328F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 1, /* combine symbols */ true}, {0.692F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 2, /* combine symbols */ true}, {0.625F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 3, /* combine symbols */ true}, {0.555F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 4, /* combine symbols */ true}, {0.507F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 5, /* combine symbols */ true}, {0.461F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 6, /* combine symbols */ true}, {0.423F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 7, /* combine symbols */ true}, {0.409F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 8, /* combine symbols */ true}, {0.371F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 9, /* combine symbols */ true}, {0.344F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 10, /* combine symbols */ true}, {0.323F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 11, /* combine symbols */ true}, {0.326F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 12, /* combine symbols */ true}, {0.323F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 13, /* combine symbols */ true}, {0.324F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 14, /* combine symbols */ true}, {0.326F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 15, /* combine symbols */ true}, {0.326F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 0, /* combine symbols */ true}, {0.195F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 1, /* combine symbols */ true}, {0.406F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 2, /* combine symbols */ true}, {0.367F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 3, /* combine symbols */ true}, {0.328F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 4, /* combine symbols */ true}, {0.303F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 5, /* combine symbols */ true}, {0.271F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 6, /* combine symbols */ true}, {0.246F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 7, /* combine symbols */ true}, {0.243F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 8, /* combine symbols */ true}, {0.224F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 9, /* combine symbols */ true}, {0.206F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 10, /* combine symbols */ true}, {0.196F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 11, /* combine symbols */ true}, {0.197F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 12, /* combine symbols */ true}, {0.196F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 13, /* combine symbols */ true}, {0.196F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 14, /* combine symbols */ true}, {0.198F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A1, /* ZCZ */ 15, /* combine symbols */ true}, {0.196F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 0, /* combine symbols */ true}, {0.605F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 1, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 2, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 3, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 4, /* combine symbols */ true}, {0.980F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 5, /* combine symbols */ true}, {0.879F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 6, /* combine symbols */ true}, {0.785F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 7, /* combine symbols */ true}, {0.760F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 8, /* combine symbols */ true}, {0.692F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 9, /* combine symbols */ true}, {0.631F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 10, /* combine symbols */ true}, {0.598F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 11, /* combine symbols */ true}, {0.602F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 12, /* combine symbols */ true}, {0.611F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 13, /* combine symbols */ true}, {0.607F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 14, /* combine symbols */ true}, {0.605F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 15, /* combine symbols */ true}, {0.607F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 0, /* combine symbols */ true}, {0.322F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 1, /* combine symbols */ true}, {0.686F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 2, /* combine symbols */ true}, {0.617F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 3, /* combine symbols */ true}, {0.554F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 4, /* combine symbols */ true}, {0.516F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 5, /* combine symbols */ true}, {0.460F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 6, /* combine symbols */ true}, {0.416F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 7, /* combine symbols */ true}, {0.406F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 8, /* combine symbols */ true}, {0.374F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 9, /* combine symbols */ true}, {0.342F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 10, /* combine symbols */ true}, {0.323F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 11, /* combine symbols */ true}, {0.325F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 12, /* combine symbols */ true}, {0.326F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 13, /* combine symbols */ true}, {0.326F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 14, /* combine symbols */ true}, {0.324F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 15, /* combine symbols */ true}, {0.321F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 0, /* combine symbols */ true}, {0.195F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 1, /* combine symbols */ true}, {0.401F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 2, /* combine symbols */ true}, {0.358F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 3, /* combine symbols */ true}, {0.325F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 4, /* combine symbols */ true}, {0.302F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 5, /* combine symbols */ true}, {0.271F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 6, /* combine symbols */ true}, {0.244F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 7, /* combine symbols */ true}, {0.241F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 8, /* combine symbols */ true}, {0.222F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 9, /* combine symbols */ true}, {0.204F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 10, /* combine symbols */ true}, {0.194F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 11, /* combine symbols */ true}, {0.195F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 12, /* combine symbols */ true}, {0.195F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 13, /* combine symbols */ true}, {0.195F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 14, /* combine symbols */ true}, {0.195F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A1, /* ZCZ */ 15, /* combine symbols */ true}, {0.195F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 0, /* combine symbols */ true}, {0.352F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 1, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 2, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 3, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 4, /* combine symbols */ true}, {0.987F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 5, /* combine symbols */ true}, {0.868F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 6, /* combine symbols */ true}, {0.789F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 7, /* combine symbols */ true}, {0.762F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 8, /* combine symbols */ true}, {0.696F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 9, /* combine symbols */ true}, {0.633F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 10, /* combine symbols */ true}, {0.602F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 11, /* combine symbols */ true}, {0.516F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 12, /* combine symbols */ true}, {0.461F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 13, /* combine symbols */ true}, {0.388F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 14, /* combine symbols */ true}, {0.350F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 15, /* combine symbols */ true}, {0.353F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 0, /* combine symbols */ true}, {0.192F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 1, /* combine symbols */ true}, {0.688F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 2, /* combine symbols */ true}, {0.625F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 3, /* combine symbols */ true}, {0.568F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 4, /* combine symbols */ true}, {0.516F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 5, /* combine symbols */ true}, {0.460F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 6, /* combine symbols */ true}, {0.416F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 7, /* combine symbols */ true}, {0.407F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 8, /* combine symbols */ true}, {0.372F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 9, /* combine symbols */ true}, {0.342F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 10, /* combine symbols */ true}, {0.324F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 11, /* combine symbols */ true}, {0.281F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 12, /* combine symbols */ true}, {0.255F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 13, /* combine symbols */ true}, {0.218F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 14, /* combine symbols */ true}, {0.193F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 15, /* combine symbols */ true}, {0.192F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 0, /* combine symbols */ true}, {0.119F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 1, /* combine symbols */ true}, {0.402F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 2, /* combine symbols */ true}, {0.363F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 3, /* combine symbols */ true}, {0.326F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 4, /* combine symbols */ true}, {0.304F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 5, /* combine symbols */ true}, {0.273F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 6, /* combine symbols */ true}, {0.248F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 7, /* combine symbols */ true}, {0.243F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 8, /* combine symbols */ true}, {0.224F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 9, /* combine symbols */ true}, {0.206F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 10, /* combine symbols */ true}, {0.195F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 11, /* combine symbols */ true}, {0.172F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 12, /* combine symbols */ true}, {0.154F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 13, /* combine symbols */ true}, {0.131F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 14, /* combine symbols */ true}, {0.118F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::A2, /* ZCZ */ 15, /* combine symbols */ true}, {0.119F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 0, /* combine symbols */ true}, {0.352F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 1, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 2, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 3, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 4, /* combine symbols */ true}, {0.993F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 5, /* combine symbols */ true}, {0.863F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 6, /* combine symbols */ true}, {0.786F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 7, /* combine symbols */ true}, {0.776F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 8, /* combine symbols */ true}, {0.699F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 9, /* combine symbols */ true}, {0.633F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 10, /* combine symbols */ true}, {0.607F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 11, /* combine symbols */ true}, {0.518F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 12, /* combine symbols */ true}, {0.464F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 13, /* combine symbols */ true}, {0.392F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 14, /* combine symbols */ true}, {0.349F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 15, /* combine symbols */ true}, {0.353F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 0, /* combine symbols */ true}, {0.194F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 1, /* combine symbols */ true}, {0.694F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 2, /* combine symbols */ true}, {0.611F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 3, /* combine symbols */ true}, {0.550F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 4, /* combine symbols */ true}, {0.510F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 5, /* combine symbols */ true}, {0.452F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 6, /* combine symbols */ true}, {0.411F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 7, /* combine symbols */ true}, {0.400F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 8, /* combine symbols */ true}, {0.367F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 9, /* combine symbols */ true}, {0.340F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 10, /* combine symbols */ true}, {0.323F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 11, /* combine symbols */ true}, {0.282F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 12, /* combine symbols */ true}, {0.256F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 13, /* combine symbols */ true}, {0.212F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 14, /* combine symbols */ true}, {0.193F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 15, /* combine symbols */ true}, {0.193F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 0, /* combine symbols */ true}, {0.118F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 1, /* combine symbols */ true}, {0.404F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 2, /* combine symbols */ true}, {0.362F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 3, /* combine symbols */ true}, {0.328F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 4, /* combine symbols */ true}, {0.301F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 5, /* combine symbols */ true}, {0.272F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 6, /* combine symbols */ true}, {0.247F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 7, /* combine symbols */ true}, {0.242F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 8, /* combine symbols */ true}, {0.222F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 9, /* combine symbols */ true}, {0.205F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 10, /* combine symbols */ true}, {0.195F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 11, /* combine symbols */ true}, {0.170F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 12, /* combine symbols */ true}, {0.155F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 13, /* combine symbols */ true}, {0.131F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 14, /* combine symbols */ true}, {0.119F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::A2, /* ZCZ */ 15, /* combine symbols */ true}, {0.119F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 0, /* combine symbols */ true}, {0.234F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 1, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 2, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 3, /* combine symbols */ true}, {1.000F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 4, /* combine symbols */ true}, {0.995F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 5, /* combine symbols */ true}, {0.876F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 6, /* combine symbols */ true}, {0.789F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 7, /* combine symbols */ true}, {0.763F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 8, /* combine symbols */ true}, {0.707F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 9, /* combine symbols */ true}, {0.646F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 10, /* combine symbols */ true}, {0.603F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 11, /* combine symbols */ true}, {0.517F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 12, /* combine symbols */ true}, {0.465F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 13, /* combine symbols */ true}, {0.388F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 14, /* combine symbols */ true}, {0.304F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 15, /* combine symbols */ true}, {0.232F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 0, /* combine symbols */ true}, {0.134F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 1, /* combine symbols */ true}, {0.687F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 2, /* combine symbols */ true}, {0.616F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 3, /* combine symbols */ true}, {0.553F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 4, /* combine symbols */ true}, {0.510F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 5, /* combine symbols */ true}, {0.456F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 6, /* combine symbols */ true}, {0.408F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 7, /* combine symbols */ true}, {0.402F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 8, /* combine symbols */ true}, {0.370F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 9, /* combine symbols */ true}, {0.348F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 10, /* combine symbols */ true}, {0.326F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 11, /* combine symbols */ true}, {0.283F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 12, /* combine symbols */ true}, {0.252F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 13, /* combine symbols */ true}, {0.217F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 14, /* combine symbols */ true}, {0.173F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 15, /* combine symbols */ true}, {0.133F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 0, /* combine symbols */ true}, {0.080F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 1, /* combine symbols */ true}, {0.401F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 2, /* combine symbols */ true}, {0.360F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 3, /* combine symbols */ true}, {0.324F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 4, /* combine symbols */ true}, {0.298F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 5, /* combine symbols */ true}, {0.270F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 6, /* combine symbols */ true}, {0.246F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 7, /* combine symbols */ true}, {0.242F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 8, /* combine symbols */ true}, {0.223F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 9, /* combine symbols */ true}, {0.205F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 10, /* combine symbols */ true}, {0.193F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 11, /* combine symbols */ true}, {0.171F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 12, /* combine symbols */ true}, {0.153F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 13, /* combine symbols */ true}, {0.130F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 14, /* combine symbols */ true}, {0.104F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz15, prach_format_type::B4, /* ZCZ */ 15, /* combine symbols */ true}, {0.080F, 12}, th_flag::orange},
    // The following commented value is the calibrated one - however, we noticed that in synthetic environments with no
    // noise/interference, the resulting false-alarm probability is a bit too high. We increase a bit the threshold.
    // {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 0, /* combine symbols */ true}, {0.229F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 0, /* combine symbols */ true}, {0.458F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 1, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 2, /* combine symbols */ true}, {1.000F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 3, /* combine symbols */ true}, {1.000F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 4, /* combine symbols */ true}, {0.986F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 5, /* combine symbols */ true}, {0.883F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 6, /* combine symbols */ true}, {0.783F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 7, /* combine symbols */ true}, {0.770F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 8, /* combine symbols */ true}, {0.700F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 9, /* combine symbols */ true}, {0.637F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 10, /* combine symbols */ true}, {0.599F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 11, /* combine symbols */ true}, {0.516F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 12, /* combine symbols */ true}, {0.464F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 13, /* combine symbols */ true}, {0.385F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 14, /* combine symbols */ true}, {0.304F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 1, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 15, /* combine symbols */ true}, {0.232F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 0, /* combine symbols */ true}, {0.131F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 1, /* combine symbols */ true}, {0.677F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 2, /* combine symbols */ true}, {0.622F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 3, /* combine symbols */ true}, {0.553F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 4, /* combine symbols */ true}, {0.511F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 5, /* combine symbols */ true}, {0.458F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 6, /* combine symbols */ true}, {0.414F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 7, /* combine symbols */ true}, {0.409F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 8, /* combine symbols */ true}, {0.374F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 9, /* combine symbols */ true}, {0.345F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 10, /* combine symbols */ true}, {0.325F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 11, /* combine symbols */ true}, {0.283F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 12, /* combine symbols */ true}, {0.255F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 13, /* combine symbols */ true}, {0.213F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 14, /* combine symbols */ true}, {0.171F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 2, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 15, /* combine symbols */ true}, {0.132F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 0, /* combine symbols */ true}, {0.081F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 1, /* combine symbols */ true}, {0.404F, 12}, th_flag::red},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 2, /* combine symbols */ true}, {0.363F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 3, /* combine symbols */ true}, {0.322F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 4, /* combine symbols */ true}, {0.298F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 5, /* combine symbols */ true}, {0.268F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 6, /* combine symbols */ true}, {0.246F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 7, /* combine symbols */ true}, {0.241F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 8, /* combine symbols */ true}, {0.219F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 9, /* combine symbols */ true}, {0.205F, 12}, th_flag::orange},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 10, /* combine symbols */ true}, {0.197F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 11, /* combine symbols */ true}, {0.171F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 12, /* combine symbols */ true}, {0.155F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 13, /* combine symbols */ true}, {0.131F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 14, /* combine symbols */ true}, {0.105F, 12}, th_flag::green},
    {{/* nof_rx_ports */ 4, prach_subcarrier_spacing::kHz30, prach_format_type::B4, /* ZCZ */ 15, /* combine symbols */ true}, {0.081F, 12}, th_flag::green},
    // clang-format on
});

} // namespace detail
} // namespace srsran
