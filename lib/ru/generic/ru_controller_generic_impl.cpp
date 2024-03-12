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

#include "ru_controller_generic_impl.h"
#include "srsran/phy/lower/lower_phy_controller.h"
#include "srsran/radio/radio_session.h"
#include "srsran/support/math_utils.h"

using namespace srsran;

ru_controller_generic_impl::ru_controller_generic_impl(std::vector<lower_phy_controller*> low_phy_crtl_,
                                                       radio_session&                     radio_,
                                                       double                             srate_MHz_) :
  low_phy_crtl(std::move(low_phy_crtl_)), radio(radio_), srate_MHz(srate_MHz_)
{
}

void ru_controller_generic_impl::start()
{
  // Calculate starting time from the radio current time plus one hundred milliseconds and rounded to the next subframe.
  double                     delay_s      = 0.1;
  baseband_gateway_timestamp current_time = radio.read_current_time();
  baseband_gateway_timestamp start_time   = current_time + static_cast<uint64_t>(delay_s * srate_MHz * 1e6);
  start_time = divide_ceil(start_time, static_cast<uint64_t>(srate_MHz * 1e3)) * static_cast<uint64_t>(srate_MHz * 1e3);

  radio.start(start_time);

  for (auto& low_phy : low_phy_crtl) {
    low_phy->start(start_time);
  }
}

void ru_controller_generic_impl::stop()
{
  radio.stop();

  for (auto& low_phy : low_phy_crtl) {
    low_phy->stop();
  }
}

bool ru_controller_generic_impl::set_tx_gain(unsigned port_id, double gain_dB)
{
  return radio.get_management_plane().set_tx_gain(port_id, gain_dB);
}

bool ru_controller_generic_impl::set_rx_gain(unsigned port_id, double gain_dB)
{
  return radio.get_management_plane().set_rx_gain(port_id, gain_dB);
}
