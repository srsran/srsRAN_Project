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

#include "ru_controller_generic_impl.h"
#include "ru_downlink_handler_generic_impl.h"
#include "ru_timing_adapter.h"
#include "ru_uplink_request_handler_generic_impl.h"
#include "rx_symbol_adapter.h"
#include "srsran/phy/adapters/phy_error_adapter.h"
#include "srsran/phy/adapters/phy_metrics_adapter.h"
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
  /// PHY metrics printer.
  std::vector<std::unique_ptr<phy_metrics_adapter>> phy_metrics_printer;
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

  // See interface for documentation.
  ru_metrics_collector* get_metrics_collector() override { return nullptr; }

private:
  std::vector<std::unique_ptr<phy_error_adapter>>         phy_err_printer;
  std::vector<std::unique_ptr<phy_metrics_adapter>>       phy_metric_printer;
  std::unique_ptr<ru_rx_symbol_adapter>                   ru_rx_adapter;
  std::vector<std::unique_ptr<lower_phy_timing_notifier>> ru_time_adapter;
  std::unique_ptr<radio_session>                          radio;
  std::vector<std::unique_ptr<lower_phy>>                 low_phy;
  ru_controller_generic_impl                              ru_ctrl;
  ru_downlink_handler_generic_impl                        ru_downlink_hdlr;
  ru_uplink_request_handler_generic_impl                  ru_uplink_request_hdlr;
};

} // namespace srsran
