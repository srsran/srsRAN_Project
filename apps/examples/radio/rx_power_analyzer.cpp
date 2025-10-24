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

/// \file
/// \brief Rx power analyzer application for SDR front-ends.
///
/// This application performs RX power measurements for a range of RX gain values. It can be configured to transmit
/// random IQ samples while conducting the RX power measurements, to emulate an incoming signal.
///
/// Its intended purpose is to aid in optimizing the RX gain as well as the target UL RX power of an SDR front-end. By
/// measuring the RX power with an RF termination plugged into the RX antenna port, the internal and thermal RF noise
/// components can be characterized for a range of gain values. Measuring with an antenna provides information about the
/// RF environment (i.e., RF noise and interference). Finally, measuring in the presence of a transmitted signal can be
/// useful to plot the actual RF gain curve.
///
/// The RX measurements are displayed upon completion of the sweep, and can optionally be written into a CSV file.

#include "radio_notifier_sample.h"
#include "srsran/gateways/baseband/baseband_gateway_receiver.h"
#include "srsran/gateways/baseband/baseband_gateway_transmitter.h"
#include "srsran/gateways/baseband/buffer/baseband_gateway_buffer_dynamic.h"
#include "srsran/radio/radio_factory.h"
#include "srsran/srsvec/conversion.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/file_vector.h"
#include "srsran/support/math/complex_normal_random.h"
#include "srsran/support/signal_handling.h"
#include <getopt.h>
#include <random>

using namespace srsran;

/// Describes the analysis configuration.
static std::string              results_filename;
static double                   step_time_s          = 1;
static srslog::basic_levels     log_level            = srslog::basic_levels::info;
static std::string              driver_name          = "uhd";
static std::string              device_arguments     = "type=b200";
static std::vector<std::string> tx_channel_arguments = {};
static std::vector<std::string> rx_channel_arguments = {};
static double                   sampling_rate_Hz     = 61.44e6;
static unsigned                 nof_channels         = 1;
static double                   tx_gain              = 20.0;
static double                   tx_rx_freq           = 3.5e9;
static double                   rx_gain_min          = 10.0;
static double                   rx_gain_max          = 75.0;
static double                   tx_rx_delay_s        = 0.002;
static bool                     tx_active            = false;
static float                    avg_tx_power_dBFS    = -12.0F;

static radio_configuration::over_the_wire_format otw_format = radio_configuration::over_the_wire_format::DEFAULT;

/// Measurement point information, containing the Rx gain, RF port number and the mean square value of the samples
/// relative to full scale (in dBFS).
using result_type = std::tuple<float, unsigned, float>;

static std::vector<result_type> measurement_results;

/// Set to true to stop.
static std::atomic<bool>              stop  = {true};
static std::unique_ptr<radio_session> radio = nullptr;

/// Function to call when the application is interrupted.
static void interrupt_signal_handler(int signal)
{
  if (radio != nullptr) {
    radio->stop();
  }
  stop = true;
}

/// Function to call when the application is going to be forcefully shutdown.
static void cleanup_signal_handler(int signal)
{
  srslog::flush();
}

static void usage(std::string_view prog)
{
  fmt::print("Usage: {} [-D time] [-v level] [-o file name]\n", prog);
  fmt::print("\t-d Driver name. [Default {}]\n", driver_name);
  fmt::print("\t-a Device arguments. [Default {}]\n", device_arguments);
  fmt::print("\t-p Number of radio ports. [Default {}]\n", nof_channels);
  fmt::print("\t-s Sampling rate. [Default {}]\n", sampling_rate_Hz);
  fmt::print("\t-f Tx/Rx frequency. [Default {}]\n", tx_rx_freq);
  fmt::print("\t-t Step time in seconds. [Default {}]\n", step_time_s);
  fmt::print("\t-m Minimum Rx gain. [Default {}]\n", rx_gain_min);
  fmt::print("\t-M Maximum Rx gain. [Default {}]\n", rx_gain_max);
  fmt::print("\t-g Tx gain. [Default {}]\n", tx_gain);
  fmt::print("\t-v Logging level. [Default {}]\n", srslog::basic_level_to_string(log_level));
  fmt::print("\t-r Saves measurement results in a file. Ignored if none. [Default {}]\n",
             results_filename.empty() ? "none" : results_filename);
  fmt::print("\t-T Transmit random data while measuring. [Default {}]\n", tx_active);
  fmt::print("\t-h Print this message.\n");
}

static void parse_args(int argc, char** argv)
{
  std::string profile_name;

  int opt = 0;
  while ((opt = getopt(argc, argv, "d:a:p:s:f:r:t:m:M:g:v:Th")) != -1) {
    switch (opt) {
      case 'd':
        driver_name = std::string(optarg);
        break;
      case 'a':
        device_arguments = std::string(optarg);
        break;
      case 'p':
        if (optarg != nullptr) {
          nof_channels = std::strtol(optarg, nullptr, 10);
        }
        break;
      case 's':
        if (optarg != nullptr) {
          sampling_rate_Hz = std::strtod(optarg, nullptr);
        }
        break;
      case 'f':
        if (optarg != nullptr) {
          tx_rx_freq = std::strtod(optarg, nullptr);
        }
        break;
      case 'r':
        results_filename = std::string(optarg);
        break;
      case 't':
        if (optarg != nullptr) {
          step_time_s = std::strtod(optarg, nullptr);
        }
        break;
      case 'm':
        if (optarg != nullptr) {
          rx_gain_min = std::strtol(optarg, nullptr, 10);
        }
        break;
      case 'M':
        if (optarg != nullptr) {
          rx_gain_max = std::strtol(optarg, nullptr, 10);
        }
        break;
      case 'g':
        if (optarg != nullptr) {
          tx_gain = std::strtol(optarg, nullptr, 10);
        }
        break;
      case 'v': {
        auto level = srslog::str_to_basic_level(std::string(optarg));
        log_level  = level.has_value() ? level.value() : srslog::basic_levels::info;
        break;
      }
      case 'T':
        tx_active = true;
        break;
      case 'h':
      default:
        usage(argv[0]);
        std::exit(-1);
    }
  }
}

static void print_results(const std::vector<result_type>& results)
{
  fmt::print("rx_gain\trx_port\tpower_dBFS\n");
  for (const auto& res : results) {
    fmt::print("{}\t{}\t{}\n", std::get<0>(res), std::get<1>(res), std::get<2>(res));
  }
}

static void write_results_csv(const std::vector<result_type>& results)
{
  // Open file sink if not empty.
  if (results_filename.empty()) {
    fmt::print("Result filename is not set. \n");
  }

  file_vector<char> results_file(results_filename.c_str(), openmode::write_only);

  fmt::print("Writing to CSV file {}...\n", results_filename);
  fmt::print("CSV columns: <rx_gain>,<rx_port>,<rx_power_dBFS>\n");

  for (const auto& res : results) {
    std::string res_str;
    fmt::format_to(std::back_inserter(res_str), "{},{},{}\n", std::get<0>(res), std::get<1>(res), std::get<2>(res));
    results_file.write(res_str);
  }
}

/// Resizes the TX and RX buffers. It also fills the TX buffer with random data samples.
static void resize_buffers(baseband_gateway_buffer_dynamic& tx_baseband_buffer,
                           baseband_gateway_buffer_dynamic& rx_baseband_buffer,
                           baseband_gateway_buffer_dynamic& rx_measurement_buffer,
                           unsigned                         block_size,
                           unsigned                         total_nof_samples)
{
  // Create a normal distribution for complex numbers with some power back-off.
  std::mt19937                      rgen(0);
  complex_normal_distribution<cf_t> dist({0, 0}, convert_dB_to_amplitude(avg_tx_power_dBFS));

  tx_baseband_buffer.resize(block_size);

  // Generate random data in Tx buffer.
  for (unsigned channel_idx = 0; channel_idx != nof_channels; ++channel_idx) {
    span<ci16_t> data = tx_baseband_buffer[channel_idx];
    for (ci16_t& iq : data) {
      cf_t value = dist(rgen);
      iq         = to_ci16(cf_t(value.real() * INT16_MAX, value.imag() * INT16_MAX));
    }
  }
  rx_baseband_buffer.resize(block_size);
  rx_measurement_buffer.resize(total_nof_samples + block_size);
}

int main(int argc, char** argv)
{
  // Set interrupt and cleanup signal handlers.
  register_interrupt_signal_handler(interrupt_signal_handler);
  register_cleanup_signal_handler(cleanup_signal_handler);

  // Parse arguments.
  parse_args(argc, argv);

  unsigned tx_rx_delay_samples = static_cast<unsigned>(sampling_rate_Hz * tx_rx_delay_s);

  // Asynchronous task executor.
  task_worker                    async_task_worker("async_thread", RADIO_MAX_NOF_PORTS);
  std::unique_ptr<task_executor> async_task_executor = make_task_executor_ptr(async_task_worker);

  // Create radio factory.
  std::unique_ptr<radio_factory> factory = create_radio_factory(driver_name);
  report_fatal_error_if_not(factory, "Driver {} is not available.", driver_name.c_str());

  // Create radio configuration.
  radio_configuration::radio config;
  config.clock.sync       = radio_configuration::clock_sources::source::DEFAULT;
  config.clock.clock      = radio_configuration::clock_sources::source::DEFAULT;
  config.sampling_rate_Hz = sampling_rate_Hz;
  config.otw_format       = otw_format;
  config.tx_mode          = radio_configuration::transmission_mode::continuous;
  config.power_ramping_us = 0;
  config.args             = device_arguments;
  config.log_level        = log_level;

  // Create transmit buffer for the stream.
  baseband_gateway_buffer_dynamic tx_baseband_buffer(nof_channels, 0);

  // For each channel in the stream...
  radio_configuration::stream tx_stream_config;
  for (unsigned channel_idx = 0; channel_idx != nof_channels; ++channel_idx) {
    // Create channel configuration and append it to the previous ones.
    radio_configuration::channel ch_config;
    ch_config.freq.center_frequency_Hz = tx_rx_freq;
    ch_config.gain_dB                  = tx_gain;
    if (!tx_channel_arguments.empty()) {
      ch_config.args = tx_channel_arguments[channel_idx];
    }
    tx_stream_config.channels.emplace_back(ch_config);
  }
  config.tx_streams.emplace_back(tx_stream_config);

  // Compute the total number of samples to receive for each sweep point.
  uint64_t total_nof_samples = static_cast<uint64_t>(step_time_s * sampling_rate_Hz);

  // Create receive baseband buffer for the stream.
  baseband_gateway_buffer_dynamic rx_baseband_buffer(nof_channels, 0);

  // Create a measurement buffer that holds all received samples for a sweep point.
  baseband_gateway_buffer_dynamic rx_measurement_buffer(nof_channels, total_nof_samples);

  // Create a buffer to hold floating-point based complex samples.
  std::vector<cf_t> cf_buffer;
  cf_buffer.resize(total_nof_samples);

  // For each channel in the stream...
  radio_configuration::stream rx_stream_config;
  for (unsigned channel_idx = 0; channel_idx != nof_channels; ++channel_idx) {
    radio_configuration::channel ch_config;
    ch_config.freq.center_frequency_Hz = tx_rx_freq;
    ch_config.gain_dB                  = rx_gain_min;
    if (!rx_channel_arguments.empty()) {
      ch_config.args = rx_channel_arguments[channel_idx];
    }
    rx_stream_config.channels.emplace_back(ch_config);
  }
  config.rx_streams.emplace_back(rx_stream_config);

  // Create notification handler.
  radio_notifier_spy notification_handler(log_level);

  // Sweep all RX gains in the range.
  for (float rx_gain = rx_gain_min; rx_gain <= rx_gain_max; ++rx_gain) {
    // Received sample counter.
    uint64_t sample_count = 0;

    // Set RX gain.
    for (unsigned channel_idx = 0; channel_idx != nof_channels; ++channel_idx) {
      config.rx_streams.front().channels[channel_idx].gain_dB = rx_gain;
    }

    // Create radio.
    radio = factory->create(config, *async_task_executor, notification_handler);
    report_fatal_error_if_not(radio, "Failed to create radio.");

    // Determine the optimal block size.
    unsigned block_size = radio->get_baseband_gateway(0).get_receiver_optimal_buffer_size();

    // Resize the baseband buffers if necessary.
    if (block_size != rx_baseband_buffer.get_nof_samples()) {
      resize_buffers(tx_baseband_buffer, rx_baseband_buffer, rx_measurement_buffer, block_size, total_nof_samples);
    }

    // Calculate starting time.
    double                     delay_s      = 0.1;
    baseband_gateway_timestamp current_time = radio->read_current_time();
    baseband_gateway_timestamp start_time   = current_time + static_cast<uint64_t>(delay_s * sampling_rate_Hz);

    // Start processing.
    {
      bool is_stopped = stop.exchange(false);
      srsran_assert(is_stopped, "Radio must be stopped before starting to receive samples");
    }
    radio->start(start_time);

    // Receive and transmit per block basis.
    while (!stop && sample_count < total_nof_samples) {
      // Get transmitter data plane.
      baseband_gateway_transmitter& transmitter = radio->get_baseband_gateway(0).get_transmitter();

      // Get receiver data plane.
      baseband_gateway_receiver& receiver = radio->get_baseband_gateway(0).get_receiver();

      // Receive baseband.
      baseband_gateway_receiver::metadata rx_metadata = receiver.receive(rx_baseband_buffer.get_writer());

      // Copy the received samples into the measurement buffer.
      for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
        span<ci16_t> dest = rx_measurement_buffer.get_writer()[i_channel].subspan(sample_count, block_size);
        srsvec::copy(dest, rx_baseband_buffer.get_reader()[i_channel]);
      }

      if (tx_active) {
        // Prepare transmit metadata.
        baseband_gateway_transmitter_metadata tx_metadata;
        tx_metadata.ts = rx_metadata.ts + tx_rx_delay_samples;

        // Transmit baseband.
        transmitter.transmit(tx_baseband_buffer.get_reader(), tx_metadata);
      }

      // Increment sample counter.
      sample_count += block_size;
    }

    if (!stop.load()) {
      // Request stop streaming asynchronously. As TX and RX operations run in the main thread, it avoids deadlock.
      std::thread stop_thread([]() {
        radio->stop();
        bool is_stopped = stop.exchange(true);
        srsran_assert(!is_stopped, "Radio must be running before attempting to stop.");
      });

      // Keep transmitting and receiving until the radio stops.
      while (!stop.load()) {
        baseband_gateway_transmitter&       transmitter = radio->get_baseband_gateway(0).get_transmitter();
        baseband_gateway_receiver&          receiver    = radio->get_baseband_gateway(0).get_receiver();
        baseband_gateway_receiver::metadata rx_metadata = receiver.receive(rx_baseband_buffer.get_writer());

        if (tx_active) {
          baseband_gateway_transmitter_metadata tx_metadata;
          tx_metadata.ts = rx_metadata.ts + tx_rx_delay_samples;

          transmitter.transmit(tx_baseband_buffer.get_reader(), tx_metadata);
        }
      }

      stop_thread.join();
    }

    for (unsigned i_channel = 0; i_channel != nof_channels; ++i_channel) {
      span<const ci16_t> rx_samples = rx_measurement_buffer.get_reader()[i_channel].first(sample_count);
      srsvec::convert(cf_buffer, rx_samples, INT16_MAX);

      // Compute average power relative to the full scale value.
      float avg_power_dBFS = convert_power_to_dB(srsvec::average_power(cf_buffer));

      // Store the measurement results for the current sweep point.
      measurement_results.emplace_back(rx_gain, i_channel, avg_power_dBFS);
    }
  }

  // Print the measurement results.
  print_results(measurement_results);

  // Write the measurement results into a CSV file.
  if (!results_filename.empty()) {
    write_results_csv(measurement_results);
  }

  // Stop asynchronous thread.
  async_task_worker.stop();

  notification_handler.print();

  return 0;
}
