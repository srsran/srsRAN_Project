/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_controller_generic_impl.h"
#include "lower_phy/lower_phy_sector.h"
#include "srsran/phy/lower/lower_phy_controller.h"
#include "srsran/phy/lower/processors/lower_phy_cfo_controller.h"
#include "srsran/radio/radio_session.h"
#include "srsran/support/math/math_utils.h"

using namespace srsran;

ru_controller_generic_impl::ru_controller_generic_impl(radio_session& radio_, double srate_MHz_) :
  srate_MHz(srate_MHz_), radio(radio_), gain_controller(radio)
{
}

void ru_controller_generic_impl::start()
{
  // Calculate starting time from the radio current time plus one hundred milliseconds.
  double                     delay_s      = 0.1;
  baseband_gateway_timestamp current_time = radio.read_current_time();
  baseband_gateway_timestamp start_time   = current_time + static_cast<uint64_t>(delay_s * srate_MHz * 1e6);

  // Round start time to the next subframe.
  uint64_t sf_duration = static_cast<uint64_t>(srate_MHz * 1e3);
  start_time           = divide_ceil(start_time, sf_duration) * sf_duration;

  radio.start(start_time);

  for (auto& low_phy : low_phy_crtl) {
    low_phy->get_controller().start(start_time);
  }
}

void ru_controller_generic_impl::stop()
{
  radio.stop();

  for (auto& low_phy : low_phy_crtl) {
    low_phy->get_controller().stop();
  }
}

void ru_controller_generic_impl::set_lower_phy_sectors(std::vector<lower_phy_sector*> sectors)
{
  srsran_assert(!sectors.empty(), "Could not set empty sectors");

  low_phy_crtl   = std::move(sectors);
  cfo_controller = ru_cfo_controller_generic_impl(low_phy_crtl);
}

bool ru_gain_controller_generic_impl::set_tx_gain(unsigned port_id, double gain_dB)
{
  return radio.get_management_plane().set_tx_gain(port_id, gain_dB);
}

bool ru_gain_controller_generic_impl::set_rx_gain(unsigned port_id, double gain_dB)
{
  return radio.get_management_plane().set_rx_gain(port_id, gain_dB);
}

bool ru_cfo_controller_generic_impl::set_tx_cfo(unsigned sector_id, const cfo_compensation_request& cfo_request)
{
  if (sector_id < phy_sectors.size()) {
    return phy_sectors[sector_id]->get_tx_cfo_control().schedule_cfo_command(
        cfo_request.start_timestamp.value_or(std::chrono::system_clock::now()),
        cfo_request.cfo_hz,
        cfo_request.cfo_drift_hz_s);
  }
  return false;
}

bool ru_cfo_controller_generic_impl::set_rx_cfo(unsigned sector_id, const cfo_compensation_request& cfo_request)
{
  if (sector_id < phy_sectors.size()) {
    return phy_sectors[sector_id]->get_rx_cfo_control().schedule_cfo_command(
        cfo_request.start_timestamp.value_or(std::chrono::system_clock::now()),
        cfo_request.cfo_hz,
        cfo_request.cfo_drift_hz_s);
  }
  return false;
}
