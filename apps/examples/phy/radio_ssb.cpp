/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
/// \brief Example application of SSB transmitted over a radio interface.
///
/// This proof of concept illustrates the integration of the upper and lower physical layer functionalities
/// together with a real-time radio interface (either real, e.g. UHD, or virtual, e.g. ZMQ) by generating and
/// transmitting the Synchronization Signal Block (SSB).
///
/// The application supports different working profiles, run <tt> radio_ssb -h </tt> for usage details.
/// \cond

#include "../radio/radio_notifier_sample.h"
#include "lower_phy_example_factory.h"
#include "rx_symbol_handler_example.h"
#include "upper_phy_ssb_example.h"
#include "srsran/phy/adapters/phy_error_adapter.h"
#include "srsran/phy/adapters/phy_rg_gateway_adapter.h"
#include "srsran/phy/adapters/phy_rx_symbol_adapter.h"
#include "srsran/phy/adapters/phy_rx_symbol_request_adapter.h"
#include "srsran/phy/adapters/phy_timing_adapter.h"
#include "srsran/phy/lower/lower_phy.h"
#include "srsran/phy/lower/lower_phy_controller.h"
#include "srsran/phy/lower/lower_phy_rx_symbol_context.h"
#include "srsran/radio/radio_factory.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/math_utils.h"
#include <atomic>
#include <csignal>
#include <getopt.h>
#include <string>
#include <unistd.h>
#include <unordered_map>

struct configuration_profile {
  std::string           name;
  std::string           description;
  std::function<void()> function;
};

using namespace srsran;

// List of allowed data modulations.
static std::vector<std::string> modulations = {to_string(modulation_scheme::PI_2_BPSK),
                                               to_string(modulation_scheme::BPSK),
                                               to_string(modulation_scheme::QPSK),
                                               to_string(modulation_scheme::QAM16),
                                               to_string(modulation_scheme::QAM64),
                                               to_string(modulation_scheme::QAM256)};

static std::string log_level = "warning";

// Program parameters.
static subcarrier_spacing                        scs                        = subcarrier_spacing::kHz15;
static unsigned                                  max_processing_delay_slots = 2;
static cyclic_prefix                             cp                         = cyclic_prefix::NORMAL;
static double                                    dl_center_freq             = 3489.42e6;
static double                                    ssb_center_freq            = 3488.16e6;
static double                                    tx_gain                    = 60.0;
static double                                    rx_freq                    = 3.5e9;
static double                                    rx_gain                    = 60.0;
static unsigned                                  nof_ports                  = 1;
static unsigned                                  nof_sectors                = 1;
static std::string                               driver_name                = "uhd";
static std::string                               device_arguments           = "type=b200";
static std::vector<std::string>                  tx_channel_args            = {};
static std::vector<std::string>                  rx_channel_args            = {};
static sampling_rate                             srate                      = sampling_rate::from_MHz(23.04);
static unsigned                                  bw_rb                      = 52;
static radio_configuration::over_the_wire_format otw_format           = radio_configuration::over_the_wire_format::SC16;
static unsigned                                  duration_slots       = 60000;
static bool                                      zmq_loopback         = true;
static ssb_pattern_case                          ssb_pattern          = ssb_pattern_case::A;
static bool                                      enable_random_data   = false;
static bool                                      enable_ul_processing = false;
static bool                                      enable_prach_processing = false;
static modulation_scheme                         data_mod_scheme         = modulation_scheme::QPSK;
static std::string                               thread_profile_name     = "single";

// Amplitude control args.
static float baseband_backoff_dB    = 12.0F;
static bool  enable_clipping        = false;
static float full_scale_amplitude   = 1.0F;
static float amplitude_ceiling_dBFS = -0.1F;

/// Defines a set of configuration profiles.
static const std::vector<configuration_profile> profiles = {
    {"b200_10MHz",
     "Single channel B200 USRP 10MHz bandwidth.",
     []() {
       device_arguments = "type=b200";
       srate            = sampling_rate::from_MHz(11.52);
       bw_rb            = 52;
     }},
    {"b200_20MHz",
     "Single channel B200 USRP 20MHz bandwidth.",
     []() {
       // Do nothing.
     }},
    {"b200_50MHz",
     "Single channel B200 USRP 50MHz bandwidth.",
     []() {
       device_arguments = "type=b200";
       srate            = sampling_rate::from_MHz(61.44);
       bw_rb            = 270;
       otw_format       = radio_configuration::over_the_wire_format::SC12;
     }},
    {"x300_50MHz",
     "Single channel X3x0 USRP 50MHz bandwidth.",
     []() {
       device_arguments = "type=x300";
       srate            = sampling_rate::from_MHz(92.16);
       tx_gain          = 10;
       rx_gain          = 10;
     }},
    {"n310_50MHz",
     "Single channel N310 USRP 50MHz bandwidth.",
     []() {
       device_arguments = "type=n3xx,ip_addr=192.168.20.2";
       srate            = sampling_rate::from_MHz(61.44);
       bw_rb            = 270;
       tx_gain          = 5;
       rx_gain          = 5;
     }},
    {"n310_100MHz",
     "Single channel N310 USRP 50MHz bandwidth.",
     []() {
       device_arguments = "type=n3xx,ip_addr=192.168.20.2";
       srate            = sampling_rate::from_MHz(122.88);
       bw_rb            = 270;
       tx_gain          = 5;
       rx_gain          = 5;
       scs              = subcarrier_spacing::kHz30;
     }},
    {"zmq_20MHz_n7",
     "Single 20MHz FDD in band n7 using ZMQ.",
     []() {
       driver_name      = "zmq";
       device_arguments = "";
       scs              = subcarrier_spacing::kHz15;
       srate            = sampling_rate::from_MHz(61.44);
       bw_rb            = 106;
       otw_format       = radio_configuration::over_the_wire_format::DEFAULT;
       dl_center_freq   = 2680.1e6;
       ssb_center_freq  = 2679.65e6;
       ssb_pattern      = ssb_pattern_case::A;

       if (zmq_loopback) {
         // Prepare ZMQ addresses using in-process communication instead of TCP. It avoids port collision and allows
         // parallel execution.
         for (unsigned channel_id = 0; channel_id != nof_ports * nof_sectors; ++channel_id) {
           fmt::memory_buffer buffer;
           fmt::format_to(buffer, "inproc://{}#{}", getpid(), channel_id);
           tx_channel_args.emplace_back(to_string(buffer));
           rx_channel_args.emplace_back(to_string(buffer));
         }
       } else {
         // Prepare ZMQ addresses using TCP. Transmitter ports start at 5000+channel_id while receivers connect to
         // 6000+channel_id.
         unsigned port_base   = 5000;
         unsigned port_offset = 1000;
         for (unsigned channel_id = 0; channel_id != nof_ports * nof_sectors; ++channel_id) {
           tx_channel_args.emplace_back("tcp://*:" + std::to_string(port_base + channel_id));
           rx_channel_args.emplace_back("tcp://localhost:" + std::to_string(port_base + channel_id + port_offset));
         }
       }
     }},
    {"zmq_20MHz_n78",
     "Single 20MHz TDD in band n78 using ZMQ.",
     []() {
       driver_name      = "zmq";
       device_arguments = "";
       scs              = subcarrier_spacing::kHz30;
       srate            = sampling_rate::from_MHz(61.44);
       bw_rb            = 52;
       otw_format       = radio_configuration::over_the_wire_format::DEFAULT;
       dl_center_freq   = 3489.42e6;
       ssb_center_freq  = 3488.16e6;
       ssb_pattern      = ssb_pattern_case::C;

       if (zmq_loopback) {
         // Prepare ZMQ addresses using in-process communication instead of TCP. It avoids port collision and allows
         // parallel execution.
         for (unsigned channel_id = 0; channel_id != nof_ports * nof_sectors; ++channel_id) {
           fmt::memory_buffer buffer;
           fmt::format_to(buffer, "inproc://{}#{}", getpid(), channel_id);
           tx_channel_args.emplace_back(to_string(buffer));
           rx_channel_args.emplace_back(to_string(buffer));
         }
       } else {
         // Prepare ZMQ addresses using TCP. Transmitter ports start at 5000+channel_id while receivers connect to
         // 6000+channel_id.
         unsigned port_base   = 5000;
         unsigned port_offset = 1000;
         for (unsigned channel_id = 0; channel_id != nof_ports * nof_sectors; ++channel_id) {
           tx_channel_args.emplace_back("tcp://*:" + std::to_string(port_base + channel_id));
           rx_channel_args.emplace_back("tcp://localhost:" + std::to_string(port_base + channel_id + port_offset));
         }
       }
     }},
};

// Global instances.
static std::mutex                             stop_execution_mutex;
static std::atomic<bool>                      stop               = {false};
static std::unique_ptr<lower_phy>             lower_phy_instance = nullptr;
static std::unique_ptr<radio_session>         radio              = nullptr;
static std::unique_ptr<upper_phy_ssb_example> upper_phy          = nullptr;

static void stop_execution()
{
  // Make sure this function is not executed simultaneously.
  std::unique_lock<std::mutex> lock(stop_execution_mutex);

  // Skip if stop has already been signaled.
  if (stop) {
    return;
  }

  // Signal program to stop.
  stop = true;

  // Stop radio. It stops blocking the radio transmit and receive operations. The timing handler prevents the PHY from
  // free running.
  if (radio != nullptr) {
    radio->stop();
  }
}

static void signal_handler(int sig)
{
  stop_execution();
}

static void usage(std::string prog)
{
  fmt::print("Usage: {} [-P profile] [-D duration] [-v level] [-o file name]\n", prog);
  fmt::print("\t-P Profile. [Default {}]\n", profiles.front().name);
  for (const configuration_profile& profile : profiles) {
    fmt::print("\t\t {:<30}{}\n", profile.name, profile.description);
  }
  fmt::print("\t-D Duration in slots. [Default {}]\n", duration_slots);
  fmt::print("\t-L Set ZMQ loopback. Set to 0 to disable, otherwise true. [Default {}]\n", zmq_loopback);
  fmt::print("\t-T Set thread profile (single, dual, quad). [Default {}]\n", thread_profile_name);
  fmt::print("\t-v Logging level. [Default {}]\n", log_level);
  fmt::print("\t-c Enable amplitude clipping. [Default {}]\n", enable_clipping);
  fmt::print("\t-b Baseband gain back-off prior to clipping (in dB). [Default {}]\n", baseband_backoff_dB);
  fmt::print("\t-d Fill the resource grid with random data [Default {}]\n", enable_random_data);
  fmt::print("\t-u Enable uplink processing [Default {}]\n", enable_ul_processing);
  fmt::print("\t-p Enable PRACH processing [Default {}]\n", enable_prach_processing);
  fmt::print("\t-a Number of antenna ports [Default {}]\n", nof_ports);
  fmt::print(
      "\t-m Data modulation scheme ({}). [Default {}]\n", span<std::string>(modulations), to_string(data_mod_scheme));
  fmt::print("\t-h Print this message.\n");
}

static void parse_args(int argc, char** argv)
{
  std::string profile_name;

  int opt = 0;
  while ((opt = getopt(argc, argv, "D:P:T:L:v:b:m:a:cduph")) != -1) {
    switch (opt) {
      case 'P':
        if (optarg != nullptr) {
          profile_name = std::string(optarg);
        }
        break;
      case 'T':
        if (optarg != nullptr) {
          thread_profile_name = std::string(optarg);
        }
        break;
      case 'D':
        if (optarg != nullptr) {
          duration_slots = std::strtol(optarg, nullptr, 10);
        }
        break;
      case 'L':
        if (optarg != nullptr) {
          zmq_loopback = (std::strtol(optarg, nullptr, 10) > 0);
        }
        break;
      case 'v':
        log_level = std::string(optarg);
        break;
      case 'd':
        enable_random_data = true;
        break;
      case 'u':
        enable_ul_processing = true;
        break;
      case 'p':
        enable_prach_processing = true;
        break;
      case 'a':
        if (optarg != nullptr) {
          nof_ports = std::strtol(optarg, nullptr, 10);
        }
        break;
      case 'm':
        if (optarg != nullptr) {
          data_mod_scheme = modulation_scheme_from_string(std::string(optarg));
        }
        break;
      case 'c':
        enable_clipping = true;
        break;
      case 'b':
        if (optarg != nullptr) {
          baseband_backoff_dB = std::strtof(optarg, nullptr);
        }
        break;
      case 'h':
      default:
        usage(argv[0]);
        exit(-1);
    }
  }

  // Search profile if set.
  if (!profile_name.empty()) {
    bool found = false;
    for (const configuration_profile& profile : profiles) {
      if (strcmp(profile_name.c_str(), profile.name.c_str()) == 0) {
        profile.function();
        found = true;
        break;
      }
    }
    if (!found) {
      usage(argv[0]);
      fmt::print("Invalid profile {}.\n", profile_name);
      std::exit(0);
    }
  }
}

static radio_configuration::radio create_radio_configuration()
{
  radio_configuration::radio radio_config = {};
  radio_config.sampling_rate_hz           = srate.to_Hz<double>();
  radio_config.otw_format                 = otw_format;
  radio_config.args                       = device_arguments;
  radio_config.log_level                  = log_level;
  for (unsigned sector_id = 0; sector_id != nof_sectors; ++sector_id) {
    // For each channel in the streams...
    radio_configuration::stream tx_stream_config;
    radio_configuration::stream rx_stream_config;
    for (unsigned port_id = 0; port_id != nof_ports; ++port_id) {
      // Create channel configuration and append it to the previous ones.
      radio_configuration::channel tx_ch_config;
      tx_ch_config.freq.center_frequency_hz = dl_center_freq;
      tx_ch_config.gain_dB                  = tx_gain;
      if (!tx_channel_args.empty()) {
        tx_ch_config.args = tx_channel_args[sector_id * nof_ports + port_id];
      }
      tx_stream_config.channels.emplace_back(tx_ch_config);

      radio_configuration::channel rx_ch_config;
      rx_ch_config.freq.center_frequency_hz = rx_freq;
      rx_ch_config.gain_dB                  = rx_gain;
      if (!rx_channel_args.empty()) {
        rx_ch_config.args = rx_channel_args[sector_id * nof_ports + port_id];
      }
      rx_stream_config.channels.emplace_back(rx_ch_config);
    }
    radio_config.tx_streams.emplace_back(tx_stream_config);
    radio_config.rx_streams.emplace_back(rx_stream_config);
  }
  return radio_config;
}

lower_phy_configuration create_lower_phy_configuration(task_executor*                rx_task_executor,
                                                       task_executor*                tx_task_executor,
                                                       task_executor*                ul_task_executor,
                                                       task_executor*                dl_task_executor,
                                                       task_executor*                prach_task_executor,
                                                       lower_phy_error_notifier*     error_notifier,
                                                       lower_phy_rx_symbol_notifier* rx_symbol_notifier,
                                                       lower_phy_timing_notifier*    timing_notifier,
                                                       srslog::basic_logger*         logger)
{
  lower_phy_configuration phy_config;
  phy_config.srate                          = srate;
  phy_config.scs                            = scs;
  phy_config.max_processing_delay_slots     = max_processing_delay_slots;
  phy_config.time_alignment_calibration     = 0;
  phy_config.system_time_throttling         = 0.0F;
  phy_config.ta_offset                      = n_ta_offset::n0;
  phy_config.cp                             = cp;
  phy_config.dft_window_offset              = 0.5F;
  phy_config.bb_gateway                     = &radio->get_baseband_gateway(0);
  phy_config.rx_symbol_notifier             = rx_symbol_notifier;
  phy_config.timing_notifier                = timing_notifier;
  phy_config.error_notifier                 = error_notifier;
  phy_config.rx_task_executor               = rx_task_executor;
  phy_config.tx_task_executor               = tx_task_executor;
  phy_config.ul_task_executor               = ul_task_executor;
  phy_config.dl_task_executor               = dl_task_executor;
  phy_config.prach_async_executor           = prach_task_executor;
  phy_config.baseband_rx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::half_slot;
  phy_config.baseband_tx_buffer_size_policy = lower_phy_baseband_buffer_size_policy::half_slot;

  // Amplitude controller configuration.
  phy_config.amplitude_config.full_scale_lin  = full_scale_amplitude;
  phy_config.amplitude_config.ceiling_dBFS    = amplitude_ceiling_dBFS;
  phy_config.amplitude_config.enable_clipping = enable_clipping;

  // Baseband gain includes normalization to unitary power (according to the number of subcarriers) and the additional
  // back-off to account for signal PAPR.
  phy_config.amplitude_config.input_gain_dB = -convert_power_to_dB(bw_rb * NRE) - baseband_backoff_dB;

  for (unsigned sector_id = 0; sector_id != nof_sectors; ++sector_id) {
    lower_phy_sector_description sector_config;
    sector_config.bandwidth_rb = bw_rb;
    sector_config.dl_freq_hz   = dl_center_freq;
    sector_config.ul_freq_hz   = rx_freq;
    sector_config.nof_tx_ports = nof_ports;
    sector_config.nof_rx_ports = nof_ports;
    phy_config.sectors.push_back(sector_config);
  }

  // Logger for amplitude control metrics.
  phy_config.logger = logger;

  return phy_config;
}

int main(int argc, char** argv)
{
  // Parse arguments.
  parse_args(argc, argv);

  // Make sure thread pool logging matches the test level.
  srslog::fetch_basic_logger("POOL").set_level(srslog::str_to_basic_level(log_level));

  // Make sure parameters are valid.
  report_fatal_error_if_not(
      srate.is_valid(scs), "Sampling rate ({}) must be multiple of {}kHz.", srate, scs_to_khz(scs));
  report_fatal_error_if_not(cp.is_valid(scs, srate.get_dft_size(scs)),
                            "The cyclic prefix ({}) numerology ({}) and sampling rate ({}) combination is invalid .",
                            cp.to_string(),
                            to_numerology_value(scs),
                            srate);

  std::unordered_map<std::string, std::unique_ptr<task_worker>> workers;
  std::unique_ptr<task_executor>                                async_task_executor;
  std::unique_ptr<task_executor>                                rx_task_executor;
  std::unique_ptr<task_executor>                                tx_task_executor;
  std::unique_ptr<task_executor>                                ul_task_executor;
  std::unique_ptr<task_executor>                                dl_task_executor;
  std::unique_ptr<task_executor>                                prach_task_executor;
  if (thread_profile_name == "single") {
    workers.emplace(
        std::make_pair("async_thread", std::make_unique<task_worker>("async_thread", 2 * nof_sectors * nof_ports)));
    workers.emplace(std::make_pair("low_phy", std::make_unique<task_worker>("low_phy", 4)));

    async_task_executor = make_task_executor(*workers["async_thread"]);
    rx_task_executor    = make_task_executor(*workers["low_phy"]);
    tx_task_executor    = make_task_executor(*workers["low_phy"]);
    ul_task_executor    = make_task_executor(*workers["low_phy"]);
    dl_task_executor    = make_task_executor(*workers["low_phy"]);
  } else if (thread_profile_name == "dual") {
    os_thread_realtime_priority low_dl_priority = os_thread_realtime_priority::max();
    os_thread_realtime_priority low_ul_priority = os_thread_realtime_priority::max() - 1;
    os_sched_affinity_bitmask   low_ul_affinity;
    os_sched_affinity_bitmask   low_dl_affinity;
    low_ul_affinity.set(0);
    low_dl_affinity.set(1);

    workers.emplace(
        std::make_pair("async_thread", std::make_unique<task_worker>("async_thread", 2 * nof_sectors * nof_ports)));
    workers.emplace(std::make_pair("low_phy_ul",
                                   std::make_unique<task_worker>("low_phy_ul", 128, low_ul_priority, low_ul_affinity)));
    workers.emplace(std::make_pair("low_phy_dl",
                                   std::make_unique<task_worker>("low_phy_dl", 128, low_dl_priority, low_dl_affinity)));

    async_task_executor = make_task_executor(*workers["async_thread"]);
    rx_task_executor    = make_task_executor(*workers["low_phy_ul"]);
    tx_task_executor    = make_task_executor(*workers["low_phy_dl"]);
    ul_task_executor    = make_task_executor(*workers["low_phy_ul"]);
    dl_task_executor    = make_task_executor(*workers["low_phy_dl"]);
  } else if (thread_profile_name == "quad") {
    os_thread_realtime_priority low_rx_priority = os_thread_realtime_priority::max() - 2;
    os_thread_realtime_priority low_tx_priority = os_thread_realtime_priority::max();
    os_thread_realtime_priority low_dl_priority = os_thread_realtime_priority::max() - 1;
    os_thread_realtime_priority low_ul_priority = os_thread_realtime_priority::max() - 3;
    os_sched_affinity_bitmask   low_rx_affinity;
    os_sched_affinity_bitmask   low_tx_affinity;
    os_sched_affinity_bitmask   low_ul_affinity;
    os_sched_affinity_bitmask   low_dl_affinity;
    low_rx_affinity.set(0);
    low_tx_affinity.set(1);
    low_ul_affinity.set(2);
    low_dl_affinity.set(3);
    workers.emplace(
        std::make_pair("async_thread", std::make_unique<task_worker>("async_thread", 2 * nof_sectors * nof_ports)));
    workers.emplace(
        std::make_pair("low_rx", std::make_unique<task_worker>("low_rx", 1, low_rx_priority, low_rx_affinity)));
    workers.emplace(
        std::make_pair("low_tx", std::make_unique<task_worker>("low_tx", 128, low_tx_priority, low_tx_affinity)));
    workers.emplace(
        std::make_pair("low_dl", std::make_unique<task_worker>("low_dl", 128, low_dl_priority, low_dl_affinity)));
    workers.emplace(
        std::make_pair("low_ul", std::make_unique<task_worker>("low_ul", 128, low_ul_priority, low_ul_affinity)));

    async_task_executor = make_task_executor(*workers["async_thread"]);
    rx_task_executor    = make_task_executor(*workers["low_rx"]);
    tx_task_executor    = make_task_executor(*workers["low_tx"]);
    ul_task_executor    = make_task_executor(*workers["low_ul"]);
    dl_task_executor    = make_task_executor(*workers["low_dl"]);
  } else {
    report_error("Invalid thread profile '{}'.\n", thread_profile_name);
  }
  workers.emplace(std::make_pair("low_phy_prach", std::make_unique<task_worker>("low_phy_prach", 4)));
  prach_task_executor = make_task_executor(*workers["low_phy_prach"]);

  // Create radio factory.
  std::unique_ptr<radio_factory> factory = create_radio_factory(driver_name);
  report_fatal_error_if_not(factory, "Driver {} is not available.", driver_name.c_str());

  // Create radio configuration. Assume 1 sector per stream.
  radio_configuration::radio radio_config = create_radio_configuration();

  // Create notification handler
  radio_notifier_spy notification_handler(log_level);

  // Create radio.
  radio = factory->create(radio_config, *async_task_executor, notification_handler);
  srsran_assert(radio, "Failed to create radio.");

  // Create symbol handler.
  rx_symbol_handler_example rx_symbol_handler(log_level);

  // Create lower PHY error adapter logger.
  srslog::basic_logger& logger = srslog::fetch_basic_logger("Low-PHY");
  logger.set_level(srslog::str_to_basic_level(log_level));

  // Create adapters.
  phy_error_adapter             error_adapter(logger);
  phy_rx_symbol_adapter         rx_symbol_adapter;
  phy_rg_gateway_adapter        rg_gateway_adapter;
  phy_timing_adapter            timing_adapter;
  phy_rx_symbol_request_adapter phy_rx_symbol_req_adapter;

  // Create lower physical layer.
  {
    // Prepare lower physical layer configuration.
    lower_phy_configuration phy_config = create_lower_phy_configuration(rx_task_executor.get(),
                                                                        tx_task_executor.get(),
                                                                        ul_task_executor.get(),
                                                                        dl_task_executor.get(),
                                                                        prach_task_executor.get(),
                                                                        &error_adapter,
                                                                        &rx_symbol_adapter,
                                                                        &timing_adapter,
                                                                        &logger);
    lower_phy_instance                 = create_lower_phy(phy_config);
    srsran_assert(lower_phy_instance, "Failed to create lower physical layer.");
  }

  double scs_Hz = static_cast<double>(1000U * scs_to_khz(scs));

  // Ratio between the resource grid SCS and 15kHz SCS.
  unsigned ratio_scs_over_15kHz = pow2(to_numerology_value(scs));
  // Frequency of Point A in Hz.
  double dl_pointA_freq_Hz = dl_center_freq - scs_Hz * NRE * bw_rb / 2;
  // Frequency of the lowest SS/PBCH block subcarrier.
  double ssb_lowest_freq_Hz = ssb_center_freq - (scs_Hz * NRE * SSB_BW_RB / 2);
  // Frequency offset from Point A to the lowest SS/PBCH block subcarrier in Hz.
  double ssb_offset_pointA_Hz = ssb_lowest_freq_Hz - dl_pointA_freq_Hz;
  // Frequency offset from Point A to the lowest SS/PBCH block subcarrier in 15kHz subcarriers (only valid for FR1).
  unsigned ssb_offset_pointA_subc_15kHz = static_cast<unsigned>(ssb_offset_pointA_Hz / 15e3);
  // Make sure it is possible to map the SS/PBCH block in the grid.
  srsran_assert(ssb_offset_pointA_subc_15kHz % ratio_scs_over_15kHz == 0,
                "The combination of DL center frequency {} MHz and SSB center frequency {} MHz results in a fractional "
                "offset of {}kHz SCS between Point A and the lowest SS/PBCH block lowest subcarrier.",
                dl_center_freq,
                ssb_center_freq,
                scs_to_khz(scs));
  // SSB frequency offset to Point A as a number of RBs.
  ssb_offset_to_pointA ssb_offset_pointA_subc_rb = ssb_offset_pointA_subc_15kHz / NRE;
  // Round down the offset to Point A to match CRB boundaries.
  ssb_offset_pointA_subc_rb = (ssb_offset_pointA_subc_rb.to_uint() / ratio_scs_over_15kHz) * ratio_scs_over_15kHz;
  // Remainder SSB frequency offset from Point A after rounding.
  ssb_subcarrier_offset subcarrier_offset = ssb_offset_pointA_subc_15kHz - ssb_offset_pointA_subc_rb.to_uint() * NRE;

  upper_phy_ssb_example::configuration upper_phy_sample_config;
  upper_phy_sample_config.log_level                    = log_level;
  upper_phy_sample_config.max_nof_prb                  = bw_rb;
  upper_phy_sample_config.max_nof_ports                = nof_ports;
  upper_phy_sample_config.rg_pool_size                 = 2 * max_processing_delay_slots;
  upper_phy_sample_config.ldpc_encoder_type            = "generic";
  upper_phy_sample_config.gateway                      = &rg_gateway_adapter;
  upper_phy_sample_config.rx_symb_req_notifier         = &phy_rx_symbol_req_adapter;
  upper_phy_sample_config.ssb_config.phys_cell_id      = 500;
  upper_phy_sample_config.ssb_config.cp                = cyclic_prefix::NORMAL;
  upper_phy_sample_config.ssb_config.period_ms         = 5;
  upper_phy_sample_config.ssb_config.beta_pss_dB       = 0.0;
  upper_phy_sample_config.ssb_config.ssb_idx           = {0};
  upper_phy_sample_config.ssb_config.L_max             = 8;
  upper_phy_sample_config.ssb_config.subcarrier_offset = subcarrier_offset;
  upper_phy_sample_config.ssb_config.offset_pointA     = ssb_offset_pointA_subc_rb;
  upper_phy_sample_config.ssb_config.pattern_case      = ssb_pattern;
  upper_phy_sample_config.enable_random_data           = enable_random_data;
  upper_phy_sample_config.enable_ul_processing         = enable_ul_processing;
  upper_phy_sample_config.enable_prach_processing      = enable_prach_processing;
  upper_phy_sample_config.data_modulation              = data_mod_scheme;
  upper_phy                                            = upper_phy_ssb_example::create(upper_phy_sample_config);
  srsran_assert(upper_phy, "Failed to create upper physical layer.");

  // Connect adapters.
  rx_symbol_adapter.connect(&rx_symbol_handler);
  timing_adapter.connect(upper_phy.get());
  rg_gateway_adapter.connect(&lower_phy_instance->get_rg_handler());
  phy_rx_symbol_req_adapter.connect(&lower_phy_instance->get_request_handler());

  // Set signal handler.
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGHUP, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGKILL, signal_handler);

  // Calculate starting time.
  double                     delay_s      = 0.1;
  baseband_gateway_timestamp current_time = radio->read_current_time();
  baseband_gateway_timestamp start_time = current_time + static_cast<uint64_t>(delay_s * radio_config.sampling_rate_hz);

  // Start processing.
  radio->start(start_time);
  lower_phy_instance->get_controller().start(start_time);

  // Receive and transmit per block basis.
  for (unsigned slot_count = 0; slot_count != duration_slots && !stop; ++slot_count) {
    // Wait for PHY to detect a TTI boundary.
    upper_phy->wait_tti_boundary();
  }

  // Stop execution.
  stop_execution();

  // Stop the timing handler. It stops blocking notifier and allows the PHY to free run.
  upper_phy->stop();

  // Stop lower PHY.
  if (lower_phy_instance != nullptr) {
    lower_phy_instance->get_controller().stop();
  }

  // Stop workers.
  for (auto& worker : workers) {
    worker.second->stop();
  }

  // Prints radio notification summary (number of overflow, underflow and other events).
  notification_handler.print();

  // Avoids pending log messages before destruction starts.
  srslog::flush();

  return 0;
}

/// \endcond
