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

#pragma once

#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/upper/channel_processors/prach_detection_result.h"
#include "srsran/ran/prach/prach_format_type.h"
#include "srsran/ran/prach/prach_subcarrier_spacing.h"
#include "srsran/ran/prach/restricted_set_config.h"

namespace srsran {

/// \brief Describes a PRACH detector interface.
///
/// The PRACH sequence generation to detect is described in TS38.211 Section 6.3.3.
class prach_detector
{
public:
  /// Configuration for PRACH detection.
  struct configuration {
    /// Root sequence index. Possibles values are {0, ..., 837} for long preambles and {0, ..., 137} for short
    /// preambles.
    unsigned root_sequence_index;
    /// Preamble format.
    prach_format_type format;
    /// Restricted set configuration.
    restricted_set_config restricted_set;
    /// Zero-correlation zone configuration index to calculate \f$N_{CS}\f$ as per TS38.211 Section 6.3.3.1.
    unsigned zero_correlation_zone;
    /// Start preamble index to monitor. Possible values are {0, ..., 63}.
    unsigned start_preamble_index;
    /// \brief Number of preamble indices to monitor. Possible values are {1, ..., 64}.
    /// \note The sum <tt>start_preamble_index + nof_preamble_indices</tt> should be not larger than 64.
    unsigned nof_preamble_indices;
    /// Random access subcarrier spacing.
    prach_subcarrier_spacing ra_scs;
    /// Number of receive ports.
    unsigned nof_rx_ports;
  };

  /// Default destructor.
  virtual ~prach_detector() = default;

  /// \brief Detects PRACH transmissions according to the given configuration.
  ///
  /// The PRACH detector assumes that \c signal is captured at the start of the PRACH reception window and it is at
  /// least as long as the PRACH preamble sequence.
  ///
  /// An assertion is triggered if the sum of start preamble index and number of preambles exceeds 64.
  ///
  /// \param[in] signal Baseband signal buffer.
  /// \param[in] config PRACH configuration for the slot.
  /// \return The detection result.
  virtual prach_detection_result detect(const prach_buffer& input, const configuration& config) = 0;
};

/// \brief Describes the PRACH detector validator interface.
class prach_detector_validator
{
public:
  /// Default destructor.
  virtual ~prach_detector_validator() = default;

  /// \brief Validates PRACH detector configuration parameters.
  /// \return True if the parameters contained in \c config are supported, false otherwise.
  virtual bool is_valid(const prach_detector::configuration& config) const = 0;
};

} // namespace srsran
