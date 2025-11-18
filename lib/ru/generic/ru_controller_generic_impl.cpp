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
#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter_metadata.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_reader.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_writer.h"
#include "srsran/phy/lower/lower_phy_controller.h"
#include "srsran/phy/lower/processors/lower_phy_center_freq_controller.h"
#include "srsran/phy/lower/processors/lower_phy_cfo_controller.h"
#include "srsran/phy/lower/processors/lower_phy_tx_time_offset_controller.h"
#include "srsran/radio/radio_session.h"
#include "srsran/support/math/math_utils.h"
#include <thread>

using namespace srsran;

ru_controller_generic_impl::ru_controller_generic_impl(
    double                                               srate_MHz_,
    std::optional<std::chrono::system_clock::time_point> start_time_) :
  srate_MHz(srate_MHz_), start_time(start_time_)
{
}

void ru_controller_generic_impl::start()
{
  srsran_assert(radio, "Null radio");
  srsran_assert(!lower_phy_sectors.empty(), "Empty list of lower phy sectors");

  // Start streaming at the given time.
  if (start_time.has_value()) {
    // Sleep until the start time.
    std::this_thread::sleep_until(*start_time);

    // Get current radio timestamp.
    baseband_gateway_timestamp current_radio_ts = radio->read_current_time();

    // Calculate some constants that depend on the sampling rate.
    uint64_t nof_ticks_per_second  = static_cast<uint64_t>(srate_MHz * 1e6);
    uint64_t nof_ticks_subframe    = nof_ticks_per_second / 1000;
    uint64_t nof_ticks_super_frame = NOF_SUBFRAMES_PER_FRAME * NOF_SFNS * nof_ticks_subframe;
    uint64_t start_advance_10ms    = nof_ticks_per_second / 100;

    // Round time to the next 1PPS rising-edge.
    baseband_gateway_timestamp time_next_pps =
        divide_ceil(current_radio_ts, nof_ticks_per_second) * nof_ticks_per_second;

    // Calculate the SFN0 reference time for matching the SFN0 to the 1PPS rising edge.
    baseband_gateway_timestamp sfn0_ref_time = time_next_pps % nof_ticks_super_frame;

    // Start streaming 10-ms before the 1PPS rising edge.
    baseband_gateway_timestamp time_start = time_next_pps - start_advance_10ms;

    // Start radio and lower physical layer at the given timestamp.
    radio->start(time_start);
    for (auto& sector : lower_phy_sectors) {
      sector->get_controller().start(time_start, sfn0_ref_time);
    }

    return;
  }

  // Calculate starting time from the radio current time plus one hundred milliseconds.
  double                     delay_s      = 0.1;
  baseband_gateway_timestamp current_time = radio->read_current_time();
  baseband_gateway_timestamp start_ts     = current_time + static_cast<uint64_t>(delay_s * srate_MHz * 1e6);

  // Round start time to the next subframe.
  uint64_t sf_duration = static_cast<uint64_t>(srate_MHz * 1e3);
  start_ts             = divide_ceil(start_ts, sf_duration) * sf_duration;

  // Start radio and lower physical layer at the given timestamp.
  radio->start(start_ts);
  for (auto& sector : lower_phy_sectors) {
    sector->get_controller().start(start_ts);
  }
}

void ru_controller_generic_impl::stop()
{
  radio->stop();

  for (auto& sector : lower_phy_sectors) {
    sector->get_controller().stop();
  }
}

void ru_controller_generic_impl::set_lower_phy_sectors(std::vector<lower_phy_sector*> sectors)
{
  srsran_assert(!sectors.empty(), "Could not set empty sectors");

  lower_phy_sectors = std::move(sectors);

  gain_controller           = ru_gain_controller_generic_impl(&radio);
  cfo_controller            = ru_cfo_controller_generic_impl(lower_phy_sectors);
  center_freq_controller    = ru_center_frequency_controller_generic_impl(lower_phy_sectors, &radio);
  tx_time_offset_controller = ru_tx_time_offset_controller_generic_impl(lower_phy_sectors);
}

bool ru_gain_controller_generic_impl::set_tx_gain(unsigned port_id, double gain_dB)
{
  return (*radio)->get_management_plane().set_tx_gain(port_id, gain_dB);
}

bool ru_gain_controller_generic_impl::set_rx_gain(unsigned port_id, double gain_dB)
{
  return (*radio)->get_management_plane().set_rx_gain(port_id, gain_dB);
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

bool ru_center_frequency_controller_generic_impl::set_tx_center_frequency(unsigned sector_id, double center_freq_Hz)
{
  (*radio)->get_management_plane().set_tx_freq(sector_id, center_freq_Hz);
  if (sector_id < phy_sectors.size()) {
    return phy_sectors[sector_id]->get_tx_center_freq_control().set_carrier_center_frequency(center_freq_Hz);
  }
  return false;
}

bool ru_center_frequency_controller_generic_impl::set_rx_center_frequency(unsigned sector_id, double center_freq_Hz)
{
  (*radio)->get_management_plane().set_rx_freq(sector_id, center_freq_Hz);
  if (sector_id < phy_sectors.size()) {
    return phy_sectors[sector_id]->get_rx_center_freq_control().set_carrier_center_frequency(center_freq_Hz);
  }
  return false;
}

bool ru_tx_time_offset_controller_generic_impl::set_tx_time_offset(unsigned sector_id, phy_time_unit tx_time_offset)
{
  if (sector_id < phy_sectors.size()) {
    phy_sectors[sector_id]->get_tx_time_offset_control().set_tx_time_offset(tx_time_offset);
    return true;
  }
  return false;
}
