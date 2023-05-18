/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "adaptors/ru_timing_adaptor.h"
#include "adaptors/rx_symbol_adaptor.h"
#include "ru_controller_generic_impl.h"
#include "ru_downlink_handler_generic_impl.h"
#include "ru_radio_notifier_sample.h"
#include "ru_uplink_request_handler_generic_impl.h"
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
  /// Radio notification handler counter.
  std::unique_ptr<ru_radio_notification_handler_counter> radio_event_counter;
  /// PHY error printer.
  std::unique_ptr<phy_error_adapter> phy_err_printer;
  /// Radio Unit received symbol adaptor.
  std::unique_ptr<ru_rx_symbol_adaptor> ru_rx_adaptor;
  /// Radio Unit timing adaptor.
  std::unique_ptr<ru_timing_adaptor> ru_time_adaptor;
  /// Radio session instance.
  std::unique_ptr<radio_session> radio;
  /// Lower PHY instance.
  std::unique_ptr<lower_phy> low_phy;
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
  std::unique_ptr<ru_radio_notification_handler_counter> radio_event_counter;
  std::unique_ptr<phy_error_adapter>                     phy_err_printer;
  std::unique_ptr<ru_rx_symbol_adaptor>                  ru_rx_adaptor;
  std::unique_ptr<ru_timing_adaptor>                     ru_time_adaptor;
  std::unique_ptr<radio_session>                         radio;
  std::unique_ptr<lower_phy>                             low_phy;
  ru_controller_generic_impl                             ru_ctrl;
  ru_downlink_handler_generic_impl                       ru_downlink_hdlr;
  ru_uplink_request_handler_generic_impl                 ru_uplink_request_hdlr;
};

} // namespace srsran
