/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Baseband gateway transmit metadata structure declaration.

#pragma once

#include "srsran/gateways/baseband/baseband_gateway_timestamp.h"
#include <optional>

namespace srsran {

/// \brief Baseband transmitter metadata parameters.
///
/// Provides additional information about the contents of a baseband buffer that is meant to be transmitted.
struct baseband_gateway_transmitter_metadata {
  /// Baseband transmitter timestamp. Indicates the time the data needs to be transmitted at.
  baseband_gateway_timestamp ts;
  /// Empty baseband buffer flag. If set to \c true, the baseband buffer does not contain any transmit signal.
  bool is_empty;
  /// \brief Transmission start in samples.
  ///
  /// If present, the number of samples at the start of the baseband buffer that do not contain any baseband samples.
  /// Otherwise, the baseband buffer starts with transmit signal from the first sample.
  std::optional<unsigned> tx_start;
  /// \brief Transmission end in samples.
  ///
  /// If present, it is the sample index in which there is no signal until the end of the buffer. Otherwise, the
  /// baseband buffer contains transmit signal until the last sample.
  std::optional<unsigned> tx_end;
};

} // namespace srsran
