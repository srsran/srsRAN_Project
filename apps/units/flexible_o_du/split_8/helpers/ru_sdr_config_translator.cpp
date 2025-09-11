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

#include "ru_sdr_config_translator.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "ru_sdr_config.h"
#include "srsran/ran/band_helper.h"

using namespace srsran;

/// Fills the given low PHY configuration from the given gnb configuration.
static lower_phy_configuration generate_low_phy_config(const flexible_o_du_ru_config::cell_config& config,
                                                       const ru_sdr_unit_config&                   ru_cfg,
                                                       unsigned max_processing_delay_slot,
                                                       unsigned sector_id)
{
  /// Static configuration that the gnb supports.
  static constexpr cyclic_prefix cp = cyclic_prefix::NORMAL;

  const unsigned bandwidth_sc =
      NOF_SUBCARRIERS_PER_RB * band_helper::get_n_rbs_from_bw(config.bw, config.scs, config.freq_range);

  lower_phy_configuration out_cfg;

  out_cfg.scs                        = config.scs;
  out_cfg.cp                         = cp;
  out_cfg.bandwidth_rb               = band_helper::get_n_rbs_from_bw(config.bw, config.scs, config.freq_range);
  out_cfg.dl_freq_hz                 = band_helper::nr_arfcn_to_freq(config.dl_arfcn);
  out_cfg.ul_freq_hz                 = band_helper::nr_arfcn_to_freq(config.ul_arfcn);
  out_cfg.nof_rx_ports               = config.nof_rx_antennas;
  out_cfg.nof_tx_ports               = config.nof_tx_antennas;
  out_cfg.sector_id                  = sector_id;
  out_cfg.dft_window_offset          = 0.5F;
  out_cfg.max_processing_delay_slots = max_processing_delay_slot;

  out_cfg.srate = sampling_rate::from_MHz(ru_cfg.srate_MHz);

  out_cfg.ta_offset = band_helper::get_ta_offset(config.freq_range);
  if (ru_cfg.time_alignment_calibration.has_value()) {
    // Selects the user specific value.
    out_cfg.time_alignment_calibration = ru_cfg.time_alignment_calibration.value();
  } else {
    // Selects a default parameter that ensures a valid time alignment in the MSG1 (PRACH).
    out_cfg.time_alignment_calibration = 0;
  }

  // Select RX buffer size policy.
  if (ru_cfg.device_driver == "zmq") {
    out_cfg.baseband_rx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::slot;
  } else if (ru_cfg.expert_execution_cfg.threads.execution_profile == lower_phy_thread_profile::single) {
    // For single executor, the same executor processes uplink and downlink. In this case, the processing is blocked
    // by the signal reception. The buffers must be smaller than a slot duration considering the downlink baseband
    // samples must arrive to the baseband device before the transmission time passes.
    out_cfg.baseband_rx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::single_packet;
  } else {
    out_cfg.baseband_rx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::single_packet;
  }

  // Get lower PHY system time throttling.
  out_cfg.system_time_throttling = ru_cfg.expert_cfg.lphy_dl_throttling;

  // Currently, only one concurrent PRACH request is supported.
  out_cfg.max_nof_prach_concurrent_requests = 1;

  // Apply gain back-off to account for the PAPR of the signal and the DFT power normalization.
  out_cfg.amplitude_config.input_gain_dB =
      -convert_power_to_dB(static_cast<float>(bandwidth_sc)) - ru_cfg.amplitude_cfg.gain_backoff_dB;

  // If clipping is enabled, the amplitude controller will clip the IQ components when their amplitude comes within
  // 0.1 dB of the radio full scale value.
  out_cfg.amplitude_config.ceiling_dBFS = ru_cfg.amplitude_cfg.power_ceiling_dBFS;

  out_cfg.amplitude_config.enable_clipping = ru_cfg.amplitude_cfg.enable_clipping;

  // Set the full scale amplitude reference to 1.
  out_cfg.amplitude_config.full_scale_lin = 1.0F;

  if (!is_valid_lower_phy_config(out_cfg)) {
    report_error("Invalid lower PHY configuration.\n");
  }

  return out_cfg;
}

/// Slice the given string by the ',' limiter, and returns a vector with each position containing one slice of the
/// string.
static std::vector<std::string> split_rf_driver_args(const std::string& driver_args)
{
  std::stringstream        ss(driver_args);
  std::vector<std::string> result;

  while (ss.good()) {
    std::string str;
    getline(ss, str, ',');
    if (!str.empty()) {
      result.push_back(str);
    }
  }

  return result;
}

/// Finds the ZMQ ports within the given driver arguments. Returns a vector that contains with the ZMQ transmission or
/// reception ports.
static std::vector<std::string> extract_zmq_ports(const std::string& driver_args, const std::string& port_id)
{
  std::vector<std::string> ports;

  const std::vector<std::string>& splitted_args = split_rf_driver_args(driver_args);
  for (const auto& arg : splitted_args) {
    auto I = arg.find(port_id);

    if (I == std::string::npos) {
      continue;
    }

    I = arg.find("=");
    ports.push_back(arg.substr(++I));
  }

  return ports;
}

static double calibrate_center_freq_Hz(double center_freq_Hz, double freq_offset_Hz, double calibration_ppm)
{
  return (center_freq_Hz + freq_offset_Hz) * (1.0 + calibration_ppm * 1e-6);
}

static void generate_radio_config(radio_configuration::radio&                      out_cfg,
                                  const ru_sdr_unit_config&                        ru_cfg,
                                  span<const flexible_o_du_ru_config::cell_config> cells)
{
  out_cfg.args             = ru_cfg.device_arguments;
  out_cfg.log_level        = ru_cfg.loggers.radio_level;
  out_cfg.sampling_rate_Hz = ru_cfg.srate_MHz * 1e6;
  out_cfg.otw_format       = radio_configuration::to_otw_format(ru_cfg.otw_format);
  out_cfg.clock.clock      = radio_configuration::to_clock_source(ru_cfg.clock_source);
  out_cfg.clock.sync       = radio_configuration::to_clock_source(ru_cfg.synch_source);
  out_cfg.tx_mode          = radio_configuration::to_transmission_mode(ru_cfg.expert_cfg.transmission_mode);
  out_cfg.power_ramping_us = ru_cfg.expert_cfg.power_ramping_time_us;

  const std::vector<std::string>& zmq_tx_addr = extract_zmq_ports(ru_cfg.device_arguments, "tx_port");
  const std::vector<std::string>& zmq_rx_addr = extract_zmq_ports(ru_cfg.device_arguments, "rx_port");

  // For each sector...
  for (unsigned sector_id = 0, e = cells.size(); sector_id != e; ++sector_id) {
    // Select cell configuration.
    const flexible_o_du_ru_config::cell_config& cell = cells[sector_id];

    // Each cell is mapped to a different stream.
    radio_configuration::stream tx_stream_config;
    radio_configuration::stream rx_stream_config;

    // Deduce center frequencies.
    const double cell_tx_freq_Hz = band_helper::nr_arfcn_to_freq(cell.dl_arfcn);
    const double cell_rx_freq_Hz = band_helper::nr_arfcn_to_freq(cell.ul_arfcn);

    // Correct actual RF center frequencies considering offset and PPM calibration.
    double center_tx_freq_cal_Hz =
        calibrate_center_freq_Hz(cell_tx_freq_Hz, ru_cfg.center_freq_offset_Hz, ru_cfg.calibrate_clock_ppm);
    double center_rx_freq_cal_Hz =
        calibrate_center_freq_Hz(cell_rx_freq_Hz, ru_cfg.center_freq_offset_Hz, ru_cfg.calibrate_clock_ppm);

    // Calculate actual LO frequencies considering LO frequency offset and the frequency correction.
    double lo_tx_freq_cal_Hz = calibrate_center_freq_Hz(
        cell_tx_freq_Hz + ru_cfg.lo_offset_MHz * 1e6, ru_cfg.center_freq_offset_Hz, ru_cfg.calibrate_clock_ppm);
    double lo_rx_freq_cal_Hz = calibrate_center_freq_Hz(
        cell_rx_freq_Hz + ru_cfg.lo_offset_MHz * 1e6, ru_cfg.center_freq_offset_Hz, ru_cfg.calibrate_clock_ppm);

    // For each DL antenna port in the cell...
    for (unsigned port_id = 0; port_id != cell.nof_tx_antennas; ++port_id) {
      // Create channel configuration and append it to the previous ones.
      radio_configuration::channel tx_ch_config = {};
      tx_ch_config.freq.center_frequency_Hz     = center_tx_freq_cal_Hz;
      if (std::isnormal(ru_cfg.lo_offset_MHz)) {
        tx_ch_config.freq.lo_frequency_Hz = lo_tx_freq_cal_Hz;
      } else {
        tx_ch_config.freq.lo_frequency_Hz = 0.0;
      }
      tx_ch_config.gain_dB = ru_cfg.tx_gain_dB;

      // Add the TX ports.
      if (ru_cfg.device_driver == "zmq") {
        if (sector_id * cell.nof_tx_antennas + port_id >= zmq_tx_addr.size()) {
          report_error("ZMQ transmission channel arguments out of bounds\n");
        }

        tx_ch_config.args = zmq_tx_addr[sector_id * cell.nof_tx_antennas + port_id];
      }
      tx_stream_config.channels.emplace_back(tx_ch_config);
    }
    out_cfg.tx_streams.emplace_back(tx_stream_config);

    // For each UL antenna port in the cell...
    for (unsigned port_id = 0; port_id != cell.nof_rx_antennas; ++port_id) {
      // Create channel configuration and append it to the previous ones.
      radio_configuration::channel rx_ch_config = {};
      rx_ch_config.freq.center_frequency_Hz     = center_rx_freq_cal_Hz;
      if (std::isnormal(ru_cfg.lo_offset_MHz)) {
        rx_ch_config.freq.lo_frequency_Hz = lo_rx_freq_cal_Hz;
      } else {
        rx_ch_config.freq.lo_frequency_Hz = 0.0;
      }
      rx_ch_config.gain_dB = ru_cfg.rx_gain_dB;

      // Add the RX ports.
      if (ru_cfg.device_driver == "zmq") {
        if (sector_id * cell.nof_rx_antennas + port_id >= zmq_rx_addr.size()) {
          report_error("ZMQ reception channel arguments out of bounds\n");
        }

        rx_ch_config.args = zmq_rx_addr[sector_id * cell.nof_rx_antennas + port_id];
      }
      rx_stream_config.channels.emplace_back(rx_ch_config);
    }
    out_cfg.rx_streams.emplace_back(rx_stream_config);
  }
}

ru_generic_configuration srsran::generate_ru_sdr_config(const ru_sdr_unit_config&                        ru_cfg,
                                                        span<const flexible_o_du_ru_config::cell_config> cells,
                                                        unsigned max_processing_delay_slots)
{
  ru_generic_configuration out_cfg;
  out_cfg.are_metrics_enabled = ru_cfg.metrics_cfg.enable_ru_metrics;
  out_cfg.device_driver       = ru_cfg.device_driver;
  out_cfg.start_time          = ru_cfg.start_time;
  generate_radio_config(out_cfg.radio_cfg, ru_cfg, cells);

  unsigned sector_id = 0;
  for (const auto& cell : cells) {
    out_cfg.lower_phy_config.push_back(generate_low_phy_config(cell, ru_cfg, max_processing_delay_slots, sector_id++));
  }

  return out_cfg;
}

void srsran::fill_sdr_worker_manager_config(worker_manager_config& config, const ru_sdr_unit_config& ru_cfg)
{
  auto& sdr_cfg = config.ru_sdr_cfg.emplace();

  sdr_cfg.nof_cells = ru_cfg.expert_execution_cfg.cell_affinities.size();
  sdr_cfg.profile   = (ru_cfg.device_driver != "zmq")
                          ? static_cast<worker_manager_config::ru_sdr_config::lower_phy_thread_profile>(
                              ru_cfg.expert_execution_cfg.threads.execution_profile)
                          : worker_manager_config::ru_sdr_config::lower_phy_thread_profile::sequential;

  srsran_assert(config.config_affinities.size() == ru_cfg.expert_execution_cfg.cell_affinities.size(),
                "Invalid number of cell affinities");

  for (unsigned i = 0; i != sdr_cfg.nof_cells; ++i) {
    config.config_affinities[i].push_back(ru_cfg.expert_execution_cfg.cell_affinities[i].ru_cpu_cfg);
  }
}
