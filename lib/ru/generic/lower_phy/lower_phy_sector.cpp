/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lower_phy_sector.h"

using namespace srsran;

lower_phy_request_handler& lower_phy_sector::get_request_handler()
{
  return low_phy->get_request_handler();
}

lower_phy_rg_handler& lower_phy_sector::get_rg_handler()
{
  return low_phy->get_rg_handler();
}

lower_phy_controller& lower_phy_sector::get_controller()
{
  return low_phy->get_controller();
}

lower_phy_cfo_controller& lower_phy_sector::get_tx_cfo_control()
{
  return low_phy->get_tx_cfo_control();
}

lower_phy_cfo_controller& lower_phy_sector::get_rx_cfo_control()
{
  return low_phy->get_rx_cfo_control();
}
