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

class frame_notifier;

/// \brief Ethernet operation control interface.
///
/// Allows starting and stopping the Ethernet receiver.
class receiver_operation_controller
{
public:
  virtual ~receiver_operation_controller() = default;

  /// \brief Starts the Ethernet receiver operation.
  ///
  /// \param[in] notifier Ethernet frames reception notifier.
  /// \note Caller will be blocked until the receiver is fully started.
  virtual void start(frame_notifier& notifier) = 0;

  /// \brief Stops the Ethernet receiver operation.
  /// \note Caller will be blocked until the receiver is fully stopped.
  virtual void stop() = 0;
};

} // namespace ether
} // namespace srsran
