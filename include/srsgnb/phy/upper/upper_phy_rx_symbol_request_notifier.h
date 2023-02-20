/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Interface of the upper-PHY notifier in charge of requesting symbol captures.

#pragma once

namespace srsgnb {

class prach_buffer;
struct prach_buffer_context;
class resource_grid;
struct resource_grid_context;

/// \brief Interface of the upper-PHY notifier in charge of requesting symbol captures.
class upper_phy_rx_symbol_request_notifier
{
public:
  virtual ~upper_phy_rx_symbol_request_notifier() = default;

  /// \brief Notifies the event of a PRACH capture request.
  ///
  /// \param[in] context PRACH window context.
  /// \param[in] buffer  Buffer to be used by the request handler to write the PRACH window.
  virtual void on_prach_capture_request(const prach_buffer_context& context, prach_buffer& buffer) = 0;

  /// \brief Notifies the event of an uplink slot request.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid    Grid in which to store the captured data.
  virtual void on_uplink_slot_request(const resource_grid_context& context, resource_grid& grid) = 0;
};

} // namespace srsgnb
