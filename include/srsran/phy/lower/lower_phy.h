/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
class lower_phy_center_freq_controller;
class lower_phy_tx_time_offset_controller;

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

  /// \brief Gets the transmit carrier center frequency control.
  virtual lower_phy_center_freq_controller& get_tx_center_freq_control() = 0;

  /// \brief Gets the receive carrier center frequency control.
  virtual lower_phy_center_freq_controller& get_rx_center_freq_control() = 0;

  /// Gets the transmit time offset control.
  virtual lower_phy_tx_time_offset_controller& get_tx_time_offset_control() = 0;
};

} // namespace srsran
