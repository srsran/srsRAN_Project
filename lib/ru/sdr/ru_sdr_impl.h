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
#include "ru_controller_sdr_impl.h"
#include "ru_lower_phy_downlink_handler_impl.h"
#include "ru_lower_phy_error_adapter.h"
#include "ru_lower_phy_timing_adapter.h"
#include "ru_lower_phy_uplink_request_handler_impl.h"
#include "ru_metrics_collector_sdr_impl.h"
#include "ru_radio_metrics_collector.h"
#include "rx_symbol_adapter.h"
#include "srsran/radio/radio_session.h"
#include "srsran/ru/ru.h"
#include <memory>

namespace srsran {

/// SDR based Radio Unit implementation configuration.
struct ru_sdr_impl_config {
  double                                               srate_MHz;
  std::optional<std::chrono::system_clock::time_point> start_time;
  bool                                                 are_metrics_enabled;
};

/// SDR based Radio Unit implementation dependencies.
struct ru_sdr_impl_dependencies {
  ru_uplink_plane_rx_symbol_notifier& rx_symbol_handler;
  ru_timing_notifier&                 timing_handler;
  srslog::basic_logger&               logger;
  srslog::basic_logger&               radio_logger;
  ru_error_notifier&                  error_notifier;
};

/// SDR Radio Unit implementation.
class ru_sdr_impl : public radio_unit
{
public:
  ru_sdr_impl(const ru_sdr_impl_config& config, const ru_sdr_impl_dependencies& dependencies);

  // See interface for documentation.
  ru_controller& get_controller() override { return radio_unit_controller; }

  // See interface for documentation
  ru_downlink_plane_handler& get_downlink_plane_handler() override { return ru_downlink_hdlr; }

  // See interface for documentation.
  ru_uplink_plane_handler& get_uplink_plane_handler() override { return ru_uplink_request_hdlr; }

  // See interface for documentation.
  ru_metrics_collector* get_metrics_collector() override { return are_metrics_enabled ? &metrics_collector : nullptr; }

  /// Returns the lower PHY rx symbol notifier of this RU.
  lower_phy_rx_symbol_notifier& get_rx_symbol_notifier() { return rx_adapter; }

  /// Returns the lower PHY timing notifier of this RU.
  lower_phy_timing_notifier& get_timing_notifier() { return timing_adapter; }

  /// Returns the lower PHY error notifier of this RU.
  lower_phy_error_notifier& get_error_notifier() { return error_adapter; }

  /// Returns the radio event notifier of this RU.
  radio_event_notifier& get_radio_event_notifier() { return radio_event_dispatcher; }

  /// Sets the radio to the given one for this RU.
  void set_radio(std::unique_ptr<radio_session> radio_ptr)
  {
    radio = std::move(radio_ptr);
    srsran_assert(radio, "Invalid radio");
    radio_unit_controller.set_radio(*radio);
  }

  /// Sets the RU lower PHY sectors.
  void set_lower_phy_sectors(std::vector<std::unique_ptr<lower_phy_sector>> lower_phy_sectors);

  /// Returns the baseband gateway for the given sector.
  baseband_gateway& get_baseband_gateway(unsigned sector_id)
  {
    srsran_assert(radio, "Invalid radio");
    return radio->get_baseband_gateway(sector_id);
  }

private:
  const bool                                     are_metrics_enabled;
  ru_radio_metrics_collector                     radio_metrics_collector;
  ru_radio_logger_event_handler                  radio_event_logger;
  ru_radio_event_dispatcher                      radio_event_dispatcher;
  ru_lower_phy_error_adapter                     error_adapter;
  ru_lower_phy_rx_symbol_adapter                 rx_adapter;
  ru_lower_phy_timing_adapter                    timing_adapter;
  std::unique_ptr<radio_session>                 radio;
  std::vector<std::unique_ptr<lower_phy_sector>> phy_sectors;
  ru_metrics_collector_sdr_impl                  metrics_collector;
  ru_controller_sdr_impl                         radio_unit_controller;
  ru_lower_phy_downlink_handler_impl             ru_downlink_hdlr;
  ru_lower_phy_uplink_request_handler_impl       ru_uplink_request_hdlr;
};

} // namespace srsran
