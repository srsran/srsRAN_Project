
#include "../radio/radio_notifier_sample.h"
#include "lower_phy_symbol_notifier_sample.h"
#include "lower_phy_timing_notifier_sample.h"
#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/phy/lower/lower_phy_factory.h"
#include "srsgnb/radio/radio_factory.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/math_utils.h"
#include <atomic>
#include <csignal>
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

using namespace srsgnb;

std::atomic<bool> stop = {false};

void signal_handler(int sig)
{
  stop = true;
}

int main(int argc, char** argv)
{
  // Program parameters.
  std::string                               driver_name                = "uhd";
  std::string                               device_arguments           = "type=b200";
  std::string                               log_level                  = "info";
  double                                    sampling_rate_hz           = 23.04e6;
  unsigned                                  numerology                 = 0;
  unsigned                                  max_processing_delay_slots = 4;
  unsigned                                  ul_to_dl_slot_offset       = 1;
  cyclic_prefix                             cp                         = cyclic_prefix::NORMAL;
  double                                    tx_freq                    = 3.5e9;
  double                                    tx_gain                    = 60.0;
  double                                    rx_freq                    = 3.5e9;
  double                                    rx_gain                    = 60.0;
  unsigned                                  duration_slots             = 60000;
  unsigned                                  nof_ports                  = 1;
  unsigned                                  nof_sectors                = 1;
  unsigned                                  bw_rb                      = 52;
  radio_configuration::over_the_wire_format otw_format = radio_configuration::over_the_wire_format::SC16;

  if (argc > 1 && argv[1] != nullptr) {
    if (std::strcmp(argv[1], "b200_50MHz") == 0) {
      device_arguments = "type=b200";
      sampling_rate_hz = 61.44e6;
      bw_rb            = 270;
      otw_format       = radio_configuration::over_the_wire_format::SC12;
    } else if (std::strcmp(argv[1], "x300_50MHz") == 0) {
      device_arguments = "type=x300";
      sampling_rate_hz = 92.16e6;
      tx_gain          = 10;
      rx_gain          = 10;
    } else {
      printf("Invalid arguments.\n");
      return -1;
    }
  }

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
  // Initialise random generator for data.
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
  radio_config.log_level                  = "debug";
  for (unsigned sector_id = 0; sector_id != nof_sectors; ++sector_id) {
    // For each channel in the streams...
    radio_configuration::stream tx_stream_config;
    radio_configuration::stream rx_stream_config;
    for (unsigned port_id = 0; port_id != nof_ports; ++port_id) {
      // Create channel configuration and append.
      radio_configuration::channel tx_ch_config;
      tx_ch_config.freq.center_frequency_hz = tx_freq;
      tx_ch_config.gain_dB                  = tx_gain;
      tx_stream_config.channels.emplace_back(tx_ch_config);

      radio_configuration::channel rx_ch_config;
      rx_ch_config.freq.center_frequency_hz = rx_freq;
      rx_ch_config.gain_dB                  = rx_gain;
      rx_stream_config.channels.emplace_back(rx_ch_config);
    }
    radio_config.tx_streams.emplace_back(tx_stream_config);
    radio_config.rx_streams.emplace_back(rx_stream_config);
  }

  // Create notification handler
  radio_notifier_spy notification_handler(log_level);

  // Create radio.
  std::unique_ptr<radio_session> radio = factory->create(radio_config, *async_task_executor, notification_handler);
  srsran_always_assert(radio, "Failed to create radio.");

  // Create symbol handler.
  lower_phy_rx_symbol_notifier_sample rx_symbol_handler(log_level);

  // Create timing handler.
  lower_phy_timing_notifier_sample timing_handler(log_level);

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
            std::vector<cf_t> rg_data(bw_rb * NRE);

            // Generate random data.
            for (cf_t& re : rg_data) {
              re = {dist(rgen), dist(rgen)};
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

  // Create lower physical layer configuration.
  lower_phy_configuration phy_config;
  phy_config.dft_size_15kHz             = dft_size_15kHz;
  phy_config.numerology                 = numerology;
  phy_config.max_processing_delay_slots = max_processing_delay_slots;
  phy_config.ul_to_dl_slot_offset       = ul_to_dl_slot_offset;
  phy_config.nof_dl_rg_buffers          = max_processing_delay_slots * 2;
  phy_config.rx_to_tx_delay             = rx_to_tx_delay;
  phy_config.tx_scale                   = tx_scale;
  phy_config.cp                         = cp;
  phy_config.radio                      = &radio->get_data_plane();
  phy_config.symbol_handler             = &rx_symbol_handler;
  phy_config.timing_handler             = &timing_handler;
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
  std::unique_ptr<lower_phy> lower_phy = lower_phy_factory().create(phy_config);
  srsran_always_assert(lower_phy, "Failed to create lower physical layer.");

  // Set signal handler.
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);
  signal(SIGHUP, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGKILL, signal_handler);

  // Start processing.
  lower_phy->start(*rt_task_executor);

  // Receive and transmit per block basis.
  for (unsigned slot_count = 0; slot_count != duration_slots && !stop; ++slot_count) {
    // Wait for PHY to detect a TTI boundary.
    slot_point slot = timing_handler.wait_tti_boundary();

    // Send resource grid per sector.
    for (unsigned sector_id = 0; sector_id != nof_sectors; ++sector_id) {
      resource_grid_context context;
      context.slot   = slot;
      context.sector = sector_id;
      lower_phy->send(context, dl_rg_pool->get_resource_grid(context));
    }
  }

  // Stop physical layer.
  lower_phy->stop();

  // Stop radio operation prior destruction.
  radio->stop();

  // Stop asynchronous thread.
  async_task_worker.stop();

  notification_handler.print();

  return 0;
}