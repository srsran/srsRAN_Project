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

#include "lower_phy_impl.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"

using namespace srsran;

ue_lower_phy_impl::ue_lower_phy_impl(configuration& config) :
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

lower_phy_cfo_controller& ue_lower_phy_impl::get_tx_cfo_control()
{
  return downlink_proc->get_cfo_control();
}

lower_phy_cfo_controller& ue_lower_phy_impl::get_rx_cfo_control()
{
  return uplink_proc->get_cfo_control();
}

lower_phy_center_freq_controller& ue_lower_phy_impl::get_tx_center_freq_control()
{
  return downlink_proc->get_carrier_center_frequency_control();
}

lower_phy_center_freq_controller& ue_lower_phy_impl::get_rx_center_freq_control()
{
  return uplink_proc->get_carrier_center_frequency_control();
}

lower_phy_tx_time_offset_controller& ue_lower_phy_impl::get_tx_time_offset_control()
{
  return downlink_proc->get_tx_time_offset_control();
}
