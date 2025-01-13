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

/// \file
/// \brief List of thresholds for the PRACH detector.

#pragma once

#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/prach_subcarrier_spacing.h"

namespace srsran::detail {

/// Flag applied to each threshold entry.
enum class threshold_flag {
  /// Threshold entry not suitable for proper PRACH detection.
  red,
  /// Threshold entry with suboptimal PRACH detection performance.
  orange,
  /// Threshold entry meeting PRACH detection requirements.
  green
};

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

/// Pairs the PRACH detector threshold and margin.
using threshold_and_margin_type = std::pair<float, unsigned>;

/// Checks the quality flag of the threshold for the given configurations.
threshold_flag get_threshold_flag(const threshold_params& params);

/// Retrieves the (threshold, margin) pair corresponding to the given configuration.
threshold_and_margin_type get_threshold_and_margin(const threshold_params& params);

} // namespace srsran::detail
