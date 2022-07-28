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
struct resource_grid_reader;
struct upper_phy_rx_symbol_context;

/// \brief Interface used to notify uplink symbol capture requests by the upper PHY.
class upper_phy_rx_symbol_request_notifier
{
public:
  virtual ~upper_phy_rx_symbol_request_notifier() = default;

  /// \brief Notifies the event of a PRACH capture request.
  ///
  /// \param[in] context Provides the PRACH window context.
  /// \param[in] buffer  Provides the PRACH buffer used to write the PRACH window.
  virtual void on_prach_capture_request(const prach_buffer_context& context, prach_buffer& buffer) = 0;

  /// \brief Notifies the event of an uplink slot request.
  ///
  /// \param context Context of the capture request.
  /// \param grid Grid that will store the data of the capture.
  virtual void on_uplink_slot_request(const upper_phy_rx_symbol_context& context, const resource_grid_reader& grid) = 0;
};

} // namespace srsgnb
