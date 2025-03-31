/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"

namespace srsran {
namespace ether {

class transmitter_metrics_collector;

/// Describes an Ethernet transmitter.
class transmitter
{
public:
  /// Default destructor.
  virtual ~transmitter() = default;

  /// Sends the given list of frames through the underlying Ethernet link.
  virtual void send(span<span<const uint8_t>> frames) = 0;

  /// Returns the metrics collector of this Ethernet transmitter or nullptr if metrics are disabled.
  virtual transmitter_metrics_collector* get_metrics_collector() = 0;
};

} // namespace ether
} // namespace srsran
