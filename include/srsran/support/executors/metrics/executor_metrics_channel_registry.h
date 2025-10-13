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

#include <string>

namespace srsran {

class executor_metrics_channel;

/// Interface for adding a new executor metrics channel.
class executor_metrics_channel_registry
{
public:
  virtual ~executor_metrics_channel_registry() = default;

  /// Adds new metrics channel that a calling executor can use to send its metrics.
  virtual executor_metrics_channel& add_channel(const std::string&) = 0;
};

} // namespace srsran
