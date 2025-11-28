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

#include "lower_phy_factory.h"

using namespace srsran;

static std::shared_ptr<lower_phy_factory> create_lower_phy_factory(const lower_phy_configuration& config,
                                                                   task_executor&                 prach_async_executor)
{
  // Deduce frequency range from the subcarrier spacing.
  frequency_range fr = frequency_range::FR1;
  if (config.scs > subcarrier_spacing::kHz60) {
    fr = frequency_range::FR2;
  }

  // Create DFT factory. It tries to create a FFTW based factory. If FFTW library is not available, it creates a generic
  // DFT factory.
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_fast();
  if (dft_factory == nullptr) {
    dft_factory = create_dft_processor_factory_generic();
  }
  report_fatal_error_if_not(dft_factory, "Failed to create DFT factory.");

  // Create OFDM modulator factory.
  ofdm_factory_generic_configuration      ofdm_common_config = {.dft_factory = dft_factory};
  std::shared_ptr<ofdm_modulator_factory> modulator_factory = create_ofdm_modulator_factory_generic(ofdm_common_config);
  report_fatal_error_if_not(modulator_factory, "Failed to create OFDM modulator factory.");

  // Wrap the OFDM modulator factory with a pool factory.
  modulator_factory =
      create_ofdm_modulator_pool_factory(std::move(modulator_factory), MAX_NSYMB_PER_SLOT * config.nof_tx_ports);
  report_fatal_error_if_not(modulator_factory, "Failed to create OFDM modulator pool factory.");

  // Create OFDM demodulator factory.
  std::shared_ptr<ofdm_demodulator_factory> demodulator_factory =
      create_ofdm_demodulator_factory_generic(ofdm_common_config);
  report_fatal_error_if_not(demodulator_factory, "Failed to create OFDM demodulator factory.");

  // Create OFDM PRACH demodulator factory.
  std::shared_ptr<ofdm_prach_demodulator_factory> prach_demodulator_factory =
      create_ofdm_prach_demodulator_factory_sw(dft_factory, config.srate, fr);
  report_fatal_error_if_not(prach_demodulator_factory, "Failed to create PRACH demodulator factory.");

  // Create amplitude control factory.
  std::shared_ptr<amplitude_controller_factory> amplitude_control_factory;
  if (config.amplitude_config.enable_clipping) {
    amplitude_control_factory = create_amplitude_controller_clipping_factory(config.amplitude_config);
  } else {
    amplitude_control_factory = create_amplitude_controller_scaling_factory(config.amplitude_config.input_gain_dB);
  }
  report_fatal_error_if_not(amplitude_control_factory, "Failed to create amplitude controller factory.");

  // Create PDxCH processor factory.
  std::shared_ptr<pdxch_processor_factory> pdxch_proc_factory =
      create_pdxch_processor_factory_sw(modulator_factory, amplitude_control_factory);
  report_fatal_error_if_not(pdxch_proc_factory, "Failed to create PDxCH processor factory.");

  // Create PRACH processor factory.
  std::shared_ptr<prach_processor_factory> prach_proc_factory =
      create_prach_processor_factory_sw(prach_demodulator_factory,
                                        prach_async_executor,
                                        config.srate,
                                        config.nof_rx_ports,
                                        config.max_nof_prach_concurrent_requests);
  report_fatal_error_if_not(prach_proc_factory, "Failed to create PRACH processor factory.");

  // Create PUxCH processor factory.
  std::shared_ptr<puxch_processor_factory> puxch_proc_factory = create_puxch_processor_factory_sw(demodulator_factory);
  report_fatal_error_if_not(puxch_proc_factory, "Failed to create PUxCH processor factory.");

  // Create Downlink processor factory.
  std::shared_ptr<lower_phy_downlink_processor_factory> downlink_proc_factory =
      create_downlink_processor_factory_sw(pdxch_proc_factory);
  report_fatal_error_if_not(downlink_proc_factory, "Failed to create downlink processor factory.");

  // Create Uplink processor factory.
  std::shared_ptr<lower_phy_uplink_processor_factory> uplink_proc_factory =
      create_uplink_processor_factory_sw(prach_proc_factory, puxch_proc_factory);
  report_fatal_error_if_not(uplink_proc_factory, "Failed to create uplink processor factory.");

  return create_lower_phy_factory_sw(downlink_proc_factory, uplink_proc_factory);
}

std::unique_ptr<lower_phy_sector> srsran::create_lower_phy_sector(const lower_phy_configuration&       lower_phy_config,
                                                                  const lower_phy_sector_dependencies& sector_deps)
{
  auto sector = std::make_unique<lower_phy_sector>(sector_deps.timing_notifier);

  lower_phy_dependencies dependencies = {.logger               = sector_deps.logger,
                                         .bb_gateway           = sector_deps.bb_gateway,
                                         .rx_symbol_notifier   = sector_deps.rx_symbol_notifier,
                                         .timing_notifier      = sector->get_timing_notifier(),
                                         .error_notifier       = sector_deps.error_notifier,
                                         .metric_notifier      = sector->get_metrics_collector(),
                                         .rx_task_executor     = sector_deps.rx_task_executor,
                                         .tx_task_executor     = sector_deps.tx_task_executor,
                                         .dl_task_executor     = sector_deps.dl_task_executor,
                                         .ul_task_executor     = sector_deps.ul_task_executor,
                                         .prach_async_executor = sector_deps.prach_async_executor};

  // Create lower PHY factory.
  auto lphy_factory = create_lower_phy_factory(lower_phy_config, dependencies.prach_async_executor);
  report_error_if_not(lphy_factory, "Failed to create lower PHY factory.");

  // Create lower PHY.
  auto phy = lphy_factory->create(lower_phy_config, dependencies);
  report_error_if_not(phy, "Unable to create lower PHY.");

  // Move PHY to the sector.
  sector->set_lower_phy(std::move(phy));

  return sector;
}
