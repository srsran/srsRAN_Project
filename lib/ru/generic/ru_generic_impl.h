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

#include "lower_phy/lower_phy_sector.h"
#include "ru_controller_generic_impl.h"
#include "ru_downlink_handler_generic_impl.h"
#include "ru_generic_error_adapter.h"
#include "ru_metrics_collector_generic_impl.h"
#include "ru_timing_adapter.h"
#include "ru_uplink_request_handler_generic_impl.h"
#include "rx_symbol_adapter.h"
#include "srsran/radio/radio_session.h"
#include "srsran/ru/ru.h"
#include <memory>

namespace srsran {

/// Radio Unit generic implementation configuration.
struct ru_generic_impl_config {
  double srate_MHz;
  bool   are_metrics_enabled;
};

/// Radio Unit generic implementation dependencies.
struct ru_generic_impl_dependencies {
  ru_uplink_plane_rx_symbol_notifier&                    rx_symbol_handler;
  ru_timing_notifier&                                    timing_handler;
  srslog::basic_logger&                                  logger;
  ru_error_notifier&                                     error_notifier;
  std::unique_ptr<ru_radio_notification_handler_counter> radio_event_counter;
  std::unique_ptr<radio_session>                         radio;
};

/// Radio Unit generic implementation.
class ru_generic_impl : public radio_unit
{
public:
  ru_generic_impl(const ru_generic_impl_config& config, ru_generic_impl_dependencies&& dependencies);

  // See interface for documentation.
  ru_controller& get_controller() override;

  // See interface for documentation
  ru_downlink_plane_handler& get_downlink_plane_handler() override;

  // See interface for documentation.
  ru_uplink_plane_handler& get_uplink_plane_handler() override;

  // See interface for documentation.
  ru_metrics_collector* get_metrics_collector() override { return are_metrics_enabled ? &metrics_collector : nullptr; }

  /// Returns the lower PHY rx symbol notifier of this RU.
  lower_phy_rx_symbol_notifier& get_rx_symbol_notifier() { return rx_adapter; }

  /// Returns the lower PHY timing notifier of this RU.
  lower_phy_timing_notifier& get_timing_notifier() { return timing_adapter; }

  /// Returns the lowwer PHY error notifier of this RU.
  lower_phy_error_notifier& get_error_notifier() { return error_adapter; }

  /// Sets the RU lower PHY sectors.
  void set_lower_phy_sectors(std::vector<std::unique_ptr<lower_phy_sector>> sectors);

  /// Returns the baseband gateway for the given sector.
  baseband_gateway& get_baseband_gateway(unsigned sector_id) { return radio->get_baseband_gateway(sector_id); }

private:
  const bool                                     are_metrics_enabled;
  ru_generic_error_adapter                       error_adapter;
  ru_rx_symbol_adapter                           rx_adapter;
  ru_timing_adapter                              timing_adapter;
  std::unique_ptr<radio_session>                 radio;
  std::vector<std::unique_ptr<lower_phy_sector>> phy_sectors;
  ru_metrics_collector_generic_impl              metrics_collector;
  ru_controller_generic_impl                     ru_ctrl;
  ru_downlink_handler_generic_impl               ru_downlink_hdlr;
  ru_uplink_request_handler_generic_impl         ru_uplink_request_hdlr;
};

} // namespace srsran
