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

class lower_phy_controller;
class lower_phy_error_notifier;
class lower_phy_request_handler;
class lower_phy_rg_handler;
class lower_phy_rx_symbol_notifier;
class lower_phy_timing_notifier;
class lower_phy_cfo_controller;

/// \brief Lower PHY main interface.
///
/// Provides access to all the lower PHY components.
class lower_phy
{
public:
  /// Default destructor.
  virtual ~lower_phy() = default;

  /// \brief Returns a reference to the lower PHY request handler.
  virtual lower_phy_request_handler& get_request_handler() = 0;

  /// \brief Returns a reference to the lower PHY resource grid handler.
  virtual lower_phy_rg_handler& get_rg_handler() = 0;

  /// \brief Returns a reference to the lower PHY controller.
  virtual lower_phy_controller& get_controller() = 0;

  /// \brief Gets the transmit carrier frequency offset control.
  virtual lower_phy_cfo_controller& get_tx_cfo_control() = 0;

  /// \brief Gets the receive carrier frequency offset control.
  virtual lower_phy_cfo_controller& get_rx_cfo_control() = 0;
};

} // namespace srsran
