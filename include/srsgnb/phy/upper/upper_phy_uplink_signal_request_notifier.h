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

struct prach_buffer;
struct prach_buffer_context;

/// \brief Interface that notifies uplink signal capture requests of the upper PHY.
class upper_phy_uplink_signal_request_notifier
{
public:
  virtual ~upper_phy_uplink_signal_request_notifier() = default;

  /// \brief Notifies the event of requesting to capture a new PRACH window.
  ///
  /// \param[in] context Provides the PRACH window context.
  /// \param[in] buffer  Provides the PRACH buffer used to write the PRACH window.
  virtual void on_prach_window_request(const prach_buffer_context& context, prach_buffer& buffer) = 0;
};

} // namespace srsgnb
