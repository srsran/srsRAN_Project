/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../radio/radio_notifier_sample.h"
#include "rx_symbol_handler_sample.h"
#include "srsgnb/phy/adapters/phy_rg_gateway_adapter.h"
#include "srsgnb/phy/adapters/phy_rx_symbol_adapter.h"
#include "srsgnb/phy/adapters/phy_timing_adapter.h"
#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/phy/lower/lower_phy_factory.h"
#include "srsgnb/radio/radio_factory.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/math_utils.h"
#include "timing_handler_sample.h"
#include <atomic>
#include <csignal>
#include <getopt.h>
#include <random>
#include <string>

namespace srsgnb {
struct dft_processor_factory_fftw_config {
  bool        avoid_wisdom;
  std::string wisdom_filename;
};
std::unique_ptr<dft_processor_factory>
create_dft_processor_factory_fftw(const dft_processor_factory_fftw_config& factory_config);
struct ofdm_modulator_factory_config {
  dft_processor_factory& dft_factory;
};
std::unique_ptr<ofdm_modulator_factory> create_ofdm_modulator_factory(ofdm_modulator_factory_config& config);

} // namespace srsgnb

/// Describes a benchmark configuration profile.
struct benchmark_configuration_profile {
  std::string           name;
  std::string           description;
  std::function<void()> function;
};

using namespace srsgnb;

static std::string log_level = "warning";

// Program parameters.
static unsigned                                  numerology                 = 0;
static unsigned                                  max_processing_delay_slots = 4;
static unsigned                                  ul_to_dl_slot_offset       = 1;
static cyclic_prefix                             cp                         = cyclic_prefix::NORMAL;
static double                                    tx_freq                    = 3.5e9;
static double                                    tx_gain                    = 60.0;
static double                                    rx_freq                    = 3.5e9;
static double                                    rx_gain                    = 60.0;
static unsigned                                  nof_ports                  = 1;
static unsigned                                  nof_sectors                = 1;
static std::string                               driver_name                = "uhd";
static std::string                               device_arguments           = "type=b200";
static std::vector<std::string>                  tx_channel_args            = {};
static std::vector<std::string>                  rx_channel_args            = {};
static double                                    sampling_rate_hz           = 23.04e6;
static unsigned                                  bw_rb                      = 52;
static radio_configuration::over_the_wire_format otw_format     = radio_configuration::over_the_wire_format::SC16;
static unsigned                                  duration_slots = 60000;
static bool                                      zmq_loopback   = true;

/// Defines a set of configuration profiles.
static const std::vector<benchmark_configuration_profile> profiles = {
    {"b200_20MHz",
     "Single channel B200 USRP 20MHz bandwidth.",
     []() {
       // Do nothing.
     }},
    {"b200_50MHz",
     "Single channel B200 USRP 50MHz bandwidth.",
     []() {
       device_arguments = "type=b200";
       sampling_rate_hz = 61.44e6;
       bw_rb            = 270;
       otw_format       = radio_configuration::over_the_wire_format::SC12;
     }},
    {"x300_50MHz",
     "Single channel X3x0 USRP 50MHz bandwidth.",
     []() {
       device_arguments = "type=x300";
       sampling_rate_hz = 92.16e6;
       tx_gain          = 10;
       rx_gain          = 10;
     }},
    {"zmq_50MHz",
     "Single channel using ZMQ.",
     []() {
       driver_name      = "zmq";
       device_arguments = "";
       sampling_rate_hz = 61.44e6;
       bw_rb            = 270;
       otw_format       = radio_configuration::over_the_wire_format::DEFAULT;

       unsigned port_base   = 5000;
       unsigned port_offset = zmq_loopback ? 0 : 1000;
       for (unsigned channel_id = 0; channel_id != nof_ports * nof_sectors; ++channel_id) {
         tx_channel_args.emplace_back("tcp://*:" + std::to_string(port_base + channel_id));
         rx_channel_args.emplace_back("tcp://localhost:" + std::to_string(port_base + channel_id + port_offset));
       }
     }},
};

// Global instances.
static std::mutex                     stop_execution_mutex;
static std::atomic<bool>              stop  = {false};
static std::unique_ptr<lower_phy>     phy   = nullptr;
static std::unique_ptr<radio_session> radio = nullptr;
static timing_handler_sample          timing_handler;

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
  timing_handler.stop();

  // Stop PHY.
  if (phy != nullptr) {
    phy->stop();
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
  for (benchmark_configuration_profile profile : profiles) {
    fmt::print("\t\t {:<30}{}\n", profile.name, profile.description);
  }
  fmt::print("\t-D Duration in slots. [Default {}]\n", duration_slots);
  fmt::print("\t-L Set ZMQ loopback. Set to 0 to disable, otherwise true [Default {}].\n", zmq_loopback);
  fmt::print("\t-v Logging level. [Default {}]\n", log_level);
  fmt::print("\t-h print this message.\n");
}

static void parse_args(int argc, char** argv)
{
  std::string profile_name;

  int opt = 0;
  while ((opt = getopt(argc, argv, "D:P:L:v:h")) != -1) {
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
      case 'h':
      default:
        usage(argv[0]);
        exit(-1);
    }
  }

  // Search profile if set.
  if (!profile_name.empty()) {
    bool found = false;
    for (const benchmark_configuration_profile& profile : profiles) {
      if (strcmp(profile_name.c_str(), profile.name.c_str()) == 0) {
        profile.function();
        found = true;
        break;
      }
    }
    if (!found) {
      usage(argv[0]);
      srsran_terminate("Invalid profile {}.", profile_name);
    }
  }
}

int main(int argc, char** argv)
{
  // Parse arguments.
  parse_args(argc, argv);

  // Make sure thread pool logging matches the test level.
  srslog::fetch_basic_logger("POOL").set_level(srslog::str_to_basic_level(log_level));

  // Derived parameters.
  unsigned dft_size_15kHz = static_cast<unsigned>(sampling_rate_hz / 15e3);
  float    tx_scale       = 1.0F / std::sqrt(NRE * bw_rb);
  double   rx_to_tx_delay = static_cast<double>(ul_to_dl_slot_offset * 1e-3) / pow2(numerology);

  // Make sure parameters are valid.
  srsran_always_assert(std::remainder(sampling_rate_hz, 15e3) < 1e-3,
                       "Sampling rate ({:.3f} MHz) must be multiple of 15kHz.",
                       sampling_rate_hz / 1e6);
  srsran_always_assert(cp.is_valid(numerology, dft_size_15kHz / pow2(numerology)),
                       "The cyclic prefix ({}) numerology ({}) and sampling rate ({:.3f}) combination is invalid .",
                       cp.to_string(),
                       numerology,
                       sampling_rate_hz);
  // Initialize random generator for data.
  std::mt19937                          rgen(1234);
  std::uniform_real_distribution<float> dist(-1.0, +1.0);

  // Radio asynchronous task executor.
  task_worker                    async_task_worker("async_thread", nof_sectors + 1);
  std::unique_ptr<task_executor> async_task_executor = make_task_executor(async_task_worker);

  // Lower PHY RT task executor.
  task_worker                    rt_task_worker("phy_rt_thread", 1, false, os_thread_realtime_priority::MAX_PRIO);
  std::unique_ptr<task_executor> rt_task_executor = make_task_executor(rt_task_worker);

  // Create radio factory.
  std::unique_ptr<radio_factory> factory = create_radio_factory(driver_name);
  srsran_always_assert(factory, "Driver %s is not available.", driver_name.c_str());

  // Create radio configuration. Assume 1 sector per stream.
  radio_configuration::radio radio_config = {};
  radio_config.sampling_rate_hz           = sampling_rate_hz;
  radio_config.otw_format                 = otw_format;
  radio_config.args                       = device_arguments;
  radio_config.log_level                  = log_level;
  for (unsigned sector_id = 0; sector_id != nof_sectors; ++sector_id) {
    // For each channel in the streams...
    radio_configuration::stream tx_stream_config;
    radio_configuration::stream rx_stream_config;
    for (unsigned port_id = 0; port_id != nof_ports; ++port_id) {
      // Create channel configuration and append.
      radio_configuration::channel tx_ch_config;
      tx_ch_config.freq.center_frequency_hz = tx_freq;
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

  // Create notification handler
  radio_notifier_spy notification_handler(log_level);

  // Create radio.
  radio = factory->create(radio_config, *async_task_executor, notification_handler);
  srsran_always_assert(radio, "Failed to create radio.");

  // Create symbol handler.
  rx_symbol_handler_sample rx_symbol_handler(log_level);
  timing_handler.set_log_level(log_level);

  // Create DFT processor factory configuration with default values.
  dft_processor_factory_fftw_config dft_factory_fftw_config;
  dft_factory_fftw_config.avoid_wisdom    = false;
  dft_factory_fftw_config.wisdom_filename = "";

  // Create DFT factory.
  std::unique_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw(dft_factory_fftw_config);

  // Create OFDM modulator factory configuration.
  ofdm_modulator_factory_config ofdm_factory_config = {*dft_factory};

  // Create OFDM modulator factory.
  std::unique_ptr<ofdm_modulator_factory> modulator_factory = create_ofdm_modulator_factory(ofdm_factory_config);
  srsran_always_assert(modulator_factory, "Failed to create OFDM modulator factory");

  // Create UL resource grid pool configuration.
  std::unique_ptr<resource_grid_pool> ul_rg_pool = nullptr;
  {
    resource_grid_pool_config rg_pool_config = {};
    rg_pool_config.nof_sectors               = 1;
    rg_pool_config.nof_slots                 = max_processing_delay_slots * 2;
    for (unsigned sector_id = 0; sector_id != rg_pool_config.nof_sectors; ++sector_id) {
      for (unsigned slot_id = 0; slot_id != rg_pool_config.nof_slots; ++slot_id) {
        rg_pool_config.grids.push_back(create_resource_grid(nof_ports, get_nsymb_per_slot(cp), bw_rb * NRE));
        srsran_always_assert(rg_pool_config.grids.back(), "Failed to create resource grid.");
      }
    }

    // Create UL resource grid pool.
    ul_rg_pool = create_resource_grid_pool(rg_pool_config);
    srsran_always_assert(ul_rg_pool, "Failed to create resource grid pool for UL");
  }

  // Create DL resource grid pool configuration.
  std::unique_ptr<resource_grid_pool> dl_rg_pool = nullptr;
  {
    std::vector<cf_t>         rg_data(bw_rb * NRE);
    resource_grid_pool_config rg_pool_config = {};
    rg_pool_config.nof_sectors               = 1;
    rg_pool_config.nof_slots                 = max_processing_delay_slots * 2;
    for (unsigned sector_id = 0; sector_id != rg_pool_config.nof_sectors; ++sector_id) {
      for (unsigned slot_id = 0; slot_id != rg_pool_config.nof_slots; ++slot_id) {
        rg_pool_config.grids.push_back(create_resource_grid(nof_ports, get_nsymb_per_slot(cp), bw_rb * NRE));
        srsran_always_assert(rg_pool_config.grids.back(), "Failed to create resource grid.");

        unsigned nsymbols = get_nsymb_per_slot(cp);
        for (unsigned port_id = 0; port_id != nof_ports; ++port_id) {
          for (unsigned symbol_id = 0; symbol_id != nsymbols; ++symbol_id) {
            unsigned nsteps      = rg_pool_config.nof_slots;
            unsigned step        = slot_id;
            unsigned cylon_id    = (step <= nsteps / 2) ? step : nsteps - step;
            unsigned subc_bundle = divide_ceil(bw_rb * NRE, nsteps / 2 + 1);
            unsigned subc_start  = subc_bundle * cylon_id;
            unsigned subc_end    = std::min(subc_bundle * (cylon_id + 1), bw_rb * NRE);

            // Generate symbol data.
            for (unsigned subc_id = 0; subc_id != rg_data.size(); ++subc_id) {
              if (subc_id >= subc_start && subc_id < subc_end) {
                rg_data[subc_id] = {dist(rgen), dist(rgen)};
              } else {
                rg_data[subc_id] = 0;
              }
            }

            // Put random generated data in grid.
            rg_pool_config.grids.back()->put(port_id, symbol_id, 0, rg_data);
          }
        }
      }
    }

    // Create UL resource grid pool.
    dl_rg_pool = create_resource_grid_pool(rg_pool_config);
    srsran_always_assert(dl_rg_pool, "Failed to create resource grid pool for DL");
  }

  // Create receive symbols adapter.
  phy_rx_symbol_adapter rx_symbol_adapter;
  rx_symbol_adapter.connect(&rx_symbol_handler);

  // Create timing adapter.
  phy_timing_adapter timing_adapter;
  timing_adapter.connect(&timing_handler);

  lower_phy_configuration phy_config;
  phy_config.dft_size_15kHz             = dft_size_15kHz;
  phy_config.numerology                 = numerology;
  phy_config.max_processing_delay_slots = max_processing_delay_slots;
  phy_config.ul_to_dl_slot_offset       = ul_to_dl_slot_offset;
  phy_config.nof_dl_rg_buffers          = max_processing_delay_slots * 2;
  phy_config.rx_to_tx_delay             = rx_to_tx_delay;
  phy_config.tx_scale                   = tx_scale;
  phy_config.cp                         = cp;
  phy_config.bb_gateway                 = &radio->get_baseband_gateway();
  phy_config.rx_symbol_notifier         = &rx_symbol_adapter;
  phy_config.timing_notifier            = &timing_adapter;
  phy_config.modulator_factory          = modulator_factory.get();
  phy_config.ul_resource_grid_pool      = ul_rg_pool.get();
  for (unsigned sector_id = 0; sector_id != nof_sectors; ++sector_id) {
    lower_phy_sector_description sector_config;
    sector_config.bandwidth_rb = bw_rb;
    sector_config.dl_freq_hz   = tx_freq;
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

  // Create lower physical layer.
  phy = lower_phy_factory().create(phy_config);
  srsran_always_assert(phy, "Failed to create lower physical layer.");

  // Set signal handler.
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGHUP, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGKILL, signal_handler);

  // Start processing.
  phy->start(*rt_task_executor);

  // Receive and transmit per block basis.
  for (unsigned slot_count = 0; slot_count != duration_slots && !stop; ++slot_count) {
    // Wait for PHY to detect a TTI boundary.
    slot_point slot = timing_handler.wait_tti_boundary();

    // Send resource grid per sector.
    for (unsigned sector_id = 0; sector_id != nof_sectors; ++sector_id) {
      resource_grid_context context;
      context.slot   = slot;
      context.sector = sector_id;
      phy->send(context, dl_rg_pool->get_resource_grid(context));
    }
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