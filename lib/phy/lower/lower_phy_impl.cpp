/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lower_phy_impl.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"
#include "srsran/phy/support/resource_grid_reader_empty.h"

using namespace srsran;

lower_phy_impl::lower_phy_impl(configuration& config) :
  downlink_proc(std::move(config.downlink_proc)),
  uplink_proc(std::move(config.uplink_proc)),
  handler_adaptor(downlink_proc->get_downlink_request_handler(),
                  uplink_proc->get_prach_request_handler(),
                  uplink_proc->get_puxch_request_handler()),
  controller(std::move(config.controller))
{
  srsran_assert(downlink_proc != nullptr, "Invalid downlink processor.");
  srsran_assert(uplink_proc != nullptr, "Invalid uplink processor.");
  srsran_assert(controller != nullptr, "Invalid controller.");
  srsran_assert(config.timing_notifier != nullptr, "Invalid timing notifier.");
  srsran_assert(config.rx_symbol_notifier != nullptr, "Invalid Rx symbol notifier.");
  srsran_assert(config.error_notifier != nullptr, "Invalid error notifier.");
  srsran_assert(config.metrics_notifier != nullptr, "Invalid metrics notifier.");

  // Connect external interfaces.
  notification_adaptor.connect_timing_notifier(*config.timing_notifier);
  notification_adaptor.connect_rx_symbol_notifier(*config.rx_symbol_notifier);
  notification_adaptor.connect_error_notifier(*config.error_notifier);
  notification_adaptor.connect_metrics_notifier(*config.metrics_notifier);

  // Connect internal adaptors.
  downlink_proc->connect(notification_adaptor.get_downlink_notifier(), notification_adaptor.get_pdxch_notifier());
  uplink_proc->connect(notification_adaptor.get_uplink_notifier(),
                       notification_adaptor.get_prach_notifier(),
                       notification_adaptor.get_puxch_notifier());
}
