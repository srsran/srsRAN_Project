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

namespace srsran {

/// Labels for the UCI decoding/detection status, common for both PUCCH and PUSCH UCI.
enum class uci_status {
  /// Unknown status.
  unknown = 0,
  /// \brief The UCI message is valid.
  ///
  /// Messages longer than 11 bits are considered as \c valid when the CRC has passed. For shorter messages, the \c
  /// valid state means that the detection metric is above the threshold.
  valid,
  /// \brief The UCI message is invalid.
  ///
  /// Messages longer than 11 bits are considered as \c invalid when the CRC failed. For shorter messages, the \c
  /// invalid state means that all detection metrics are below the threshold.
  invalid
};

/// Gets a string representing a \c uci_status value.
inline const char* to_string(uci_status status)
{
  switch (status) {
    case uci_status::valid:
      return "valid";
    case uci_status::unknown:
      return "unknown";
    case uci_status::invalid:
    default:
      return "invalid";
  }
}

} // namespace srsran
