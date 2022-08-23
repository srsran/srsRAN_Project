/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
#include "srsgnb/phy/adapters/phy_error_adapter.h"
#include "srsgnb/phy/adapters/phy_rg_gateway_adapter.h"
#include "srsgnb/phy/adapters/phy_rx_symbol_adapter.h"
#include "srsgnb/phy/adapters/phy_timing_adapter.h"
#include "srsgnb/phy/lower/lower_phy.h"
#include "srsgnb/radio/radio_factory.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/math_utils.h"
#include "upper_phy_ssb_example.h"
#include <atomic>
#include <csignal>
#include <getopt.h>
#include <string>

struct configuration_profile {
  std::string           name;
  std::string           description;
  std::function<void()> function;
};

using namespace srsgnb;

static std::string log_level = "warning";

// Program parameters.
static subcarrier_spacing                        scs                        = subcarrier_spacing::kHz15;
static unsigned                                  max_processing_delay_slots = 4;
static unsigned                                  ul_to_dl_subframe_offset   = 1;
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
static radio_configuration::over_the_wire_format otw_format     = radio_configuration::over_the_wire_format::SC16;
static unsigned                                  duration_slots = 60000;
static bool                                      zmq_loopback   = true;
static ssb_pattern_case                          ssb_pattern    = ssb_pattern_case::A;

// Amplitude control args.
static float baseband_gain_dB       = -3.0F;
static bool  enable_clipping        = false;
static float full_scale_amplitude   = 1.0F;
static float amplitude_ceiling_dBFS = -0.1F;

/// Defines a set of configuration profiles.
static const std::vector<configuration_profile> profiles = {
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
           fmt::format_to(buffer, "inproc://#{}", channel_id);
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
           fmt::format_to(buffer, "inproc://#{}", channel_id);
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

  // Stop the timing handler. It stops blocking notifier and allows the PHY to free run.
  upper_phy->stop();

  // Stop PHY.
  if (lower_phy_instance != nullptr) {
    lower_phy_instance->get_controller().stop();
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
  fmt::print("\t-v Logging level. [Default {}]\n", log_level);
  fmt::print("\t-c Enable amplitude clipping. [Default {}]\n", enable_clipping);
  fmt::print("\t-b Baseband gain prior to clipping (in dB). [Default {}]\n", baseband_gain_dB);
  fmt::print("\t-h Print this message.\n");
}

static void parse_args(int argc, char** argv)
{
  std::string profile_name;

  int opt = 0;
  while ((opt = getopt(argc, argv, "D:P:L:v:b:ch")) != -1) {
    switch (opt) {
      case 'P':
        if (optarg != nullptr) {
          profile_name = std::string(optarg);
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
      case 'c':
        enable_clipping = true;
        break;
      case 'b':
        if (optarg != nullptr) {
          baseband_gain_dB = std::strtof(optarg, nullptr);
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

lower_phy_configuration create_lower_phy_configuration(float                         tx_scale,
                                                       lower_phy_error_notifier*     error_notifier,
                                                       lower_phy_rx_symbol_notifier* rx_symbol_notifier,
                                                       lower_phy_timing_notifier*    timing_notifier)
{
  lower_phy_configuration phy_config;
  phy_config.srate                      = srate;
  phy_config.scs                        = scs;
  phy_config.max_processing_delay_slots = max_processing_delay_slots;
  phy_config.ul_to_dl_subframe_offset   = ul_to_dl_subframe_offset;
  phy_config.time_advance_calibration   = phy_time_unit::from_seconds(0);
  phy_config.ta_offset                  = lower_phy_ta_offset::n0;
  phy_config.tx_scale                   = tx_scale;
  phy_config.cp                         = cp;
  phy_config.bb_gateway                 = &radio->get_baseband_gateway();
  phy_config.error_notifier             = error_notifier;
  phy_config.rx_symbol_notifier         = rx_symbol_notifier;
  phy_config.timing_notifier            = timing_notifier;

  // Amplitude controller configuration.
  phy_config.amplitude_config.full_scale_lin  = full_scale_amplitude;
  phy_config.amplitude_config.ceiling_dBFS    = amplitude_ceiling_dBFS;
  phy_config.amplitude_config.enable_clipping = enable_clipping;
  phy_config.amplitude_config.input_gain_dB   = baseband_gain_dB;

  for (unsigned sector_id = 0; sector_id != nof_sectors; ++sector_id) {
    lower_phy_sector_description sector_config;
    sector_config.bandwidth_rb = bw_rb;
    sector_config.dl_freq_hz   = dl_center_freq;
    sector_config.ul_freq_hz   = rx_freq;
    for (unsigned port_id = 0; port_id < nof_ports; ++port_id) {
      lower_phy_sector_port_mapping port_mapping;
      port_mapping.stream_id  = sector_id;
      port_mapping.channel_id = port_id;
      sector_config.port_mapping.push_back(port_mapping);
    }
    phy_config.sectors.push_back(sector_config);
    phy_config.nof_channels_per_stream.push_back(nof_ports);
  }
  phy_config.log_level = log_level;

  return phy_config;
}

int main(int argc, char** argv)
{
  // Parse arguments.
  parse_args(argc, argv);

  // Make sure thread pool logging matches the test level.
  srslog::fetch_basic_logger("POOL").set_level(srslog::str_to_basic_level(log_level));

  // Derived parameters.
  float tx_scale = 1.0F / std::sqrt(NRE * bw_rb);

  // Make sure parameters are valid.
  report_fatal_error_if_not(
      srate.is_valid(scs), "Sampling rate ({}) must be multiple of {}kHz.", srate, scs_to_khz(scs));
  report_fatal_error_if_not(cp.is_valid(scs, srate.get_dft_size(scs)),
                            "The cyclic prefix ({}) numerology ({}) and sampling rate ({}) combination is invalid .",
                            cp.to_string(),
                            to_numerology_value(scs),
                            srate);

  // Radio asynchronous task executor.
  task_worker                    async_task_worker("async_thread", nof_sectors + 1);
  std::unique_ptr<task_executor> async_task_executor = make_task_executor(async_task_worker);

  // Lower PHY RT task executor.
  task_worker                    rt_task_worker("phy_rt_thread", 1, false, os_thread_realtime_priority::MAX_PRIO);
  std::unique_ptr<task_executor> rt_task_executor = make_task_executor(rt_task_worker);

  // Create radio factory.
  std::unique_ptr<radio_factory> factory = create_radio_factory(driver_name);
  report_fatal_error_if_not(factory, "Driver {} is not available.", driver_name.c_str());

  // Create radio configuration. Assume 1 sector per stream.
  radio_configuration::radio radio_config = create_radio_configuration();

  // Create notification handler
  radio_notifier_spy notification_handler(log_level);

  // Create radio.
  radio = factory->create(radio_config, *async_task_executor, notification_handler);
  srsgnb_assert(radio, "Failed to create radio.");

  // Create symbol handler.
  rx_symbol_handler_example rx_symbol_handler(log_level);

  // Create adapters.
  phy_error_adapter      error_adapter(log_level);
  phy_rx_symbol_adapter  rx_symbol_adapter;
  phy_rg_gateway_adapter rg_gateway_adapter;
  phy_timing_adapter     timing_adapter;

  // Create lower physical layer.
  {
    lower_phy_configuration phy_config =
        create_lower_phy_configuration(tx_scale, &error_adapter, &rx_symbol_adapter, &timing_adapter);
    lower_phy_instance = create_lower_phy(phy_config);
    srsgnb_assert(lower_phy_instance, "Failed to create lower physical layer.");
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
  srsgnb_assert(ssb_offset_pointA_subc_15kHz % ratio_scs_over_15kHz == 0,
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
  upper_phy_sample_config.ssb_config.phys_cell_id      = 500;
  upper_phy_sample_config.ssb_config.cp                = cyclic_prefix::NORMAL;
  upper_phy_sample_config.ssb_config.period_ms         = 5;
  upper_phy_sample_config.ssb_config.beta_pss_dB       = 0.0;
  upper_phy_sample_config.ssb_config.ssb_idx           = {0};
  upper_phy_sample_config.ssb_config.L_max             = 8;
  upper_phy_sample_config.ssb_config.subcarrier_offset = subcarrier_offset;
  upper_phy_sample_config.ssb_config.offset_pointA     = ssb_offset_pointA_subc_rb;
  upper_phy_sample_config.ssb_config.pattern_case      = ssb_pattern;
  upper_phy                                            = upper_phy_ssb_example::create(upper_phy_sample_config);
  srsgnb_assert(upper_phy, "Failed to create upper physical layer.");

  // Connect adapters.
  rx_symbol_adapter.connect(&rx_symbol_handler);
  timing_adapter.connect(upper_phy.get());
  rg_gateway_adapter.connect(&lower_phy_instance->get_rg_handler());

  // Set signal handler.
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGHUP, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGKILL, signal_handler);

  // Start processing.
  lower_phy_instance->get_controller().start(*rt_task_executor);

  // Receive and transmit per block basis.
  for (unsigned slot_count = 0; slot_count != duration_slots && !stop; ++slot_count) {
    // Wait for PHY to detect a TTI boundary.
    upper_phy->wait_tti_boundary();
  }

  // Stop execution.
  stop_execution();

  // Stop workers.
  async_task_worker.stop();
  rt_task_worker.stop();

  // Prints radio notification summary (number of overflow, underflow and other events).
  notification_handler.print();

  // Avoids pending log messages before destruction starts.
  srslog::flush();

  return 0;
}

/// \endcond
