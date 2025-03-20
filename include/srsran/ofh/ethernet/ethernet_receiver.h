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

namespace srsran {
namespace ether {

class receiver_metrics_collector;
class receiver_operation_controller;

/// \brief Describes an Ethernet receiver.
///
/// The receiver notifies incoming Ethernet frames through the \c frame_notifier interface.
class receiver
{
public:
  /// Default destructor.
  virtual ~receiver() = default;

  /// Returns the operation controller of this Ethernet receiver.
  virtual receiver_operation_controller& get_operation_controller() = 0;

  /// Returns the metrics collector of this Ethernet receiver or nullptr if metrics are disabled.
  virtual receiver_metrics_collector* get_metrics_collector() = 0;
};

} // namespace ether
} // namespace srsran
