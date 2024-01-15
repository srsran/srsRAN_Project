/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "ru_controller_generic_impl.h"
#include "ru_downlink_handler_generic_impl.h"
#include "ru_timing_adapter.h"
#include "ru_uplink_request_handler_generic_impl.h"
#include "rx_symbol_adapter.h"
#include "srsran/phy/adapters/phy_error_adapter.h"
#include "srsran/phy/lower/lower_phy.h"
#include "srsran/radio/radio_session.h"
#include "srsran/ru/ru.h"
#include <memory>

namespace srsran {

/// Radio Unit generic implementation configuration.
struct ru_generic_impl_config {
  /// Sampling rate in MHz.
  double srate_MHz;
  /// PHY error printer.
  std::vector<std::unique_ptr<phy_error_adapter>> phy_err_printer;
  /// Radio Unit received symbol adapter.
  std::unique_ptr<ru_rx_symbol_adapter> ru_rx_adapter;
  /// Radio Unit timing adapter.
  std::vector<std::unique_ptr<lower_phy_timing_notifier>> ru_time_adapter;
  /// Radio session instance.
  std::unique_ptr<radio_session> radio;
  /// Lower PHY instance.
  std::vector<std::unique_ptr<lower_phy>> low_phy;
};

/// Radio Unit generic implementation.
class ru_generic_impl : public radio_unit
{
public:
  explicit ru_generic_impl(ru_generic_impl_config&& config);

  // See interface for documentation.
  ru_controller& get_controller() override;

  // See interface for documentation
  ru_downlink_plane_handler& get_downlink_plane_handler() override;

  // See interface for documentation.
  ru_uplink_plane_handler& get_uplink_plane_handler() override;

private:
  std::vector<std::unique_ptr<phy_error_adapter>>         phy_err_printer;
  std::unique_ptr<ru_rx_symbol_adapter>                   ru_rx_adapter;
  std::vector<std::unique_ptr<lower_phy_timing_notifier>> ru_time_adapter;
  std::unique_ptr<radio_session>                          radio;
  std::vector<std::unique_ptr<lower_phy>>                 low_phy;
  ru_controller_generic_impl                              ru_ctrl;
  ru_downlink_handler_generic_impl                        ru_downlink_hdlr;
  ru_uplink_request_handler_generic_impl                  ru_uplink_request_hdlr;
};

} // namespace srsran
