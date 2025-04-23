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

#include "ru_controller_generic_impl.h"
#include "lower_phy/lower_phy_sector.h"
#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter_metadata.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "srsran/phy/lower/lower_phy_controller.h"
#include "srsran/phy/lower/processors/lower_phy_cfo_controller.h"
#include "srsran/radio/radio_session.h"
#include "srsran/support/math/math_utils.h"

using namespace srsran;

namespace {

class radio_management_plane_dummy : public radio_management_plane
{
public:
  bool set_tx_gain(unsigned port_id, double gain_dB) override { return false; }
  bool set_rx_gain(unsigned port_id, double gain_dB) override { return false; }
};

class baseband_gateway_transmitter_dummy : public baseband_gateway_transmitter
{
public:
  void transmit(const baseband_gateway_buffer_reader&        data,
                const baseband_gateway_transmitter_metadata& metadata) override
  {
  }
};

class baseband_gateway_receiver_dummy : public baseband_gateway_receiver
{
public:
  metadata receive(baseband_gateway_buffer_writer& data) override { return {}; }
};

class baseband_gateway_dummy : public baseband_gateway
{
  baseband_gateway_transmitter_dummy tx;
  baseband_gateway_receiver_dummy    rx;

public:
  baseband_gateway_transmitter& get_transmitter() override { return tx; }
  baseband_gateway_receiver&    get_receiver() override { return rx; }
};

class radio_session_dummy : public radio_session
{
  radio_management_plane_dummy management;
  baseband_gateway_dummy       gateway;

public:
  radio_management_plane&    get_management_plane() override { return management; }
  baseband_gateway&          get_baseband_gateway(unsigned stream_id) override { return gateway; }
  baseband_gateway_timestamp read_current_time() override { return 0; }
  void                       start(baseband_gateway_timestamp init_time) override {}
  void                       stop() override {}
};

} // namespace

static radio_session_dummy dummy_radio;

ru_controller_generic_impl::ru_controller_generic_impl(double srate_MHz_) :
  srate_MHz(srate_MHz_), radio(&dummy_radio), gain_controller(radio)
{
}

void ru_controller_generic_impl::start()
{
  // Calculate starting time from the radio current time plus one hundred milliseconds.
  double                     delay_s      = 0.1;
  baseband_gateway_timestamp current_time = radio->read_current_time();
  baseband_gateway_timestamp start_time   = current_time + static_cast<uint64_t>(delay_s * srate_MHz * 1e6);

  // Round start time to the next subframe.
  uint64_t sf_duration = static_cast<uint64_t>(srate_MHz * 1e3);
  start_time           = divide_ceil(start_time, sf_duration) * sf_duration;

  radio->start(start_time);

  for (auto& low_phy : low_phy_crtl) {
    low_phy->get_controller().start(start_time);
  }
}

void ru_controller_generic_impl::stop()
{
  radio->stop();

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
  return radio->get_management_plane().set_tx_gain(port_id, gain_dB);
}

bool ru_gain_controller_generic_impl::set_rx_gain(unsigned port_id, double gain_dB)
{
  return radio->get_management_plane().set_rx_gain(port_id, gain_dB);
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
