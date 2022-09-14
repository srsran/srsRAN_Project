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

/// Determines UCI decoding or detection status, it is common for PUCCH and PUSCH.
enum class uci_status {
  /// Unknown status.
  unknown,
  /// \brief The UCI message is valid.
  ///
  /// If the message is longer than 12 bits, it means that CRC has passed. Otherwise, detector correlation exceeded the
  /// threshold.
  valid,
  /// \brief The UCI message is invalid.
  ///
  /// If the message is longer than 12 bits, it means that CRC has not passed. Otherwise, detector correlation did not
  /// reach the threshold.
  invalid
};

} // namespace srsgnb
