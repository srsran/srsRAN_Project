/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "lower_phy_sector.h"

using namespace srsran;

lower_phy_uplink_request_handler& lower_phy_sector::get_uplink_request_handler()
{
  return lower_phy->get_uplink_request_handler();
}

lower_phy_downlink_handler& lower_phy_sector::get_downlink_handler()
{
  return lower_phy->get_downlink_handler();
}

lower_phy_controller& lower_phy_sector::get_controller()
{
  return lower_phy->get_controller();
}

lower_phy_cfo_controller& lower_phy_sector::get_tx_cfo_control()
{
  return lower_phy->get_tx_cfo_control();
}

lower_phy_cfo_controller& lower_phy_sector::get_rx_cfo_control()
{
  return lower_phy->get_rx_cfo_control();
}

lower_phy_center_freq_controller& lower_phy_sector::get_tx_center_freq_control()
{
  return lower_phy->get_tx_center_freq_control();
}

lower_phy_center_freq_controller& lower_phy_sector::get_rx_center_freq_control()
{
  return lower_phy->get_rx_center_freq_control();
}

lower_phy_tx_time_offset_controller& lower_phy_sector::get_tx_time_offset_control()
{
  return lower_phy->get_tx_time_offset_control();
}
