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

/// \file
/// \brief List of thresholds for the PRACH detector.

#pragma once

#include "srsran/adt/to_array.h"
#include "srsran/ran/prach/prach_format_type.h"

namespace srsran {
namespace detail {

/// Parameters that affect the detection threshold value.
struct threshold_params {
  /// Number of receive antenna ports.
  unsigned nof_rx_ports = {};
  /// PRACH format type.
  prach_format_type format = {};
  /// Zero correlation zone.
  unsigned zero_correlation_zone = {};
  /// Flag to activate non-coherent combining of PRACH symbols.
  bool combine_symbols = {};
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
  if (lhs.format < rhs.format) {
    return true;
  }
  if (lhs.format > rhs.format) {
    return false;
  }

  // If we are here, nof_rx_ports and format are equal.
  if (lhs.zero_correlation_zone < rhs.zero_correlation_zone) {
    return true;
  }
  if (lhs.zero_correlation_zone > rhs.zero_correlation_zone) {
    return false;
  }

  // If we are here, nof_rx_ports, format and zero_correlation_zone are equal.
  if (!lhs.combine_symbols && rhs.combine_symbols) {
    return true;
  }
  return false;
}

/// Manages the mapping between PRACH configuration and the (threshold, margin) pairs.
class threshold_and_margin_finder
{
public:
  using threshold_and_margin = std::pair<float, unsigned>;
  using threshold_entry      = std::pair<threshold_params, threshold_and_margin>;

  /// Constructor: receives the list of pairings and ensures it is sorted.
  explicit threshold_and_margin_finder(span<const threshold_entry> in) :
    sorted_thresholds_and_margins(in.begin(), in.end())
  {
    std::sort(sorted_thresholds_and_margins.begin(),
              sorted_thresholds_and_margins.end(),
              [](const threshold_entry& a, const threshold_entry& b) { return (a.first < b.first); });
  }

  /// Retrieves the (threshold, margin) pair corresponding to the given configuration.
  threshold_and_margin get(const threshold_params& params) const
  {
    auto it = std::lower_bound(sorted_thresholds_and_margins.begin(),
                               sorted_thresholds_and_margins.end(),
                               params,
                               [](const threshold_entry& a, const threshold_params& b) { return (a.first < b); });

    if (it != sorted_thresholds_and_margins.end()) {
      return it->second;
    }

    // todo(david): once all cases are covered, replace this by a srsran_assert.
    if (is_long_preamble(params.format)) {
      return {/* threshold */ 2.0F, /* margin */ 5};
    }
    return {/* threshold */ 0.3F, /* margin */ 12};
  }

private:
  std::vector<threshold_entry> sorted_thresholds_and_margins;
};

static const auto all_threshold_and_margins = to_array<threshold_and_margin_finder::threshold_entry>({
    {{/* nof_rx_ports */ 1, prach_format_type::zero, /* ZCZ */ 0, /* combine symbols */ true}, {0.15F, 5}},
    {{/* nof_rx_ports */ 1, prach_format_type::zero, /* ZCZ */ 1, /* combine symbols */ true}, {1.00F, 5}},
    {{/* nof_rx_ports */ 1, prach_format_type::B4, /* ZCZ */ 0, /* combine symbols */ true}, {0.39F, 12}},
    {{/* nof_rx_ports */ 1, prach_format_type::B4, /* ZCZ */ 14, /* combine symbols */ true}, {0.39F, 12}},
    {{/* nof_rx_ports */ 2, prach_format_type::zero, /* ZCZ */ 0, /* combine symbols */ true}, {0.09F, 5}},
    {{/* nof_rx_ports */ 2, prach_format_type::zero, /* ZCZ */ 1, /* combine symbols */ true}, {0.45F, 5}},
    {{/* nof_rx_ports */ 2, prach_format_type::B4, /* ZCZ */ 0, /* combine symbols */ true}, {0.14F, 12}},
    {{/* nof_rx_ports */ 2, prach_format_type::B4, /* ZCZ */ 14, /* combine symbols */ true}, {0.18F, 12}},
    {{/* nof_rx_ports */ 4, prach_format_type::zero, /* ZCZ */ 0, /* combine symbols */ true}, {0.06F, 5}},
    {{/* nof_rx_ports */ 4, prach_format_type::zero, /* ZCZ */ 1, /* combine symbols */ true}, {0.32F, 5}},
    {{/* nof_rx_ports */ 4, prach_format_type::B4, /* ZCZ */ 0, /* combine symbols */ true}, {0.09F, 12}},
    {{/* nof_rx_ports */ 4, prach_format_type::B4, /* ZCZ */ 14, /* combine symbols */ true}, {0.11F, 12}},
});

} // namespace detail
} // namespace srsran