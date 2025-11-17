/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lower_phy_impl.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"

using namespace srsran;

lower_phy_impl::lower_phy_impl(dependencies deps) :
  notification_adaptor(deps.error_notifier, deps.metrics_notifier, deps.rx_symbol_notifier, deps.timing_notifier),
  downlink_proc(std::move(deps.downlink_proc)),
  uplink_proc(std::move(deps.uplink_proc)),
  handler_adaptor(downlink_proc->get_downlink_request_handler(),
                  uplink_proc->get_prach_request_handler(),
                  uplink_proc->get_puxch_request_handler()),
  controller(std::move(deps.controller))
{
  srsran_assert(downlink_proc != nullptr, "Invalid downlink processor.");
  srsran_assert(uplink_proc != nullptr, "Invalid uplink processor.");
  srsran_assert(controller != nullptr, "Invalid controller.");

  // Connect internal adaptors.
  downlink_proc->connect(notification_adaptor.get_downlink_notifier(), notification_adaptor.get_pdxch_notifier());
  uplink_proc->connect(notification_adaptor.get_uplink_notifier(),
                       notification_adaptor.get_prach_notifier(),
                       notification_adaptor.get_puxch_notifier());
}

lower_phy_cfo_controller& lower_phy_impl::get_tx_cfo_control()
{
  return downlink_proc->get_cfo_control();
}

lower_phy_cfo_controller& lower_phy_impl::get_rx_cfo_control()
{
  return uplink_proc->get_cfo_control();
}

lower_phy_center_freq_controller& lower_phy_impl::get_tx_center_freq_control()
{
  return downlink_proc->get_carrier_center_frequency_control();
}

lower_phy_center_freq_controller& lower_phy_impl::get_rx_center_freq_control()
{
  return uplink_proc->get_carrier_center_frequency_control();
}

lower_phy_tx_time_offset_controller& lower_phy_impl::get_tx_time_offset_control()
{
  return downlink_proc->get_tx_time_offset_control();
}
