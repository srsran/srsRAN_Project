/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {

/// Labels for the UCI decoding/detection status, common for both PUCCH and PUSCH UCI.
enum class uci_status {
  /// Unknown status.
  unknown,
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

} // namespace srsgnb
