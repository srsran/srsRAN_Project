/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#pragma once

#include "srsran/phy/lower/lower_phy.h"
#include "srsran/phy/lower/lower_phy_factory.h"
#include <memory>

namespace srsran {

struct lower_phy_configuration;
class task_executor;

/// Helper class that creates the lower PHY factory using the given configuration.
inline std::shared_ptr<lower_phy_factory> create_lower_phy_factory(lower_phy_configuration& config,
                                                                   unsigned max_nof_prach_concurrent_requests)
{
  // Get the maximum number of receive ports.
  unsigned max_nof_rx_ports =
      std::max_element(config.sectors.begin(),
                       config.sectors.end(),
                       [](const lower_phy_sector_description& left, const lower_phy_sector_description& right) {
                         return left.nof_rx_ports > right.nof_rx_ports;
                       })
          ->nof_rx_ports;

  // Create DFT factory. It tries to create a FFTW based factory. If FFTW library is not available, it creates a generic
  // DFT factory.
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_fast();
  if (dft_factory == nullptr) {
    dft_factory = create_dft_processor_factory_generic();
  }
  report_fatal_error_if_not(dft_factory, "Failed to create DFT factory.");

  // Create OFDM modulator factory.
  ofdm_factory_generic_configuration ofdm_common_config;
  ofdm_common_config.dft_factory                            = dft_factory;
  std::shared_ptr<ofdm_modulator_factory> modulator_factory = create_ofdm_modulator_factory_generic(ofdm_common_config);
  report_fatal_error_if_not(modulator_factory, "Failed to create OFDM modulator factory.");

  // Create OFDM demodulator factory.
  std::shared_ptr<ofdm_demodulator_factory> demodulator_factory =
      create_ofdm_demodulator_factory_generic(ofdm_common_config);
  report_fatal_error_if_not(demodulator_factory, "Failed to create OFDM demodulator factory.");

  // Create OFDM PRACH demodulator factory.
  std::shared_ptr<ofdm_prach_demodulator_factory> prach_demodulator_factory =
      create_ofdm_prach_demodulator_factory_sw(dft_factory, config.srate);
  report_fatal_error_if_not(prach_demodulator_factory, "Failed to create PRACH demodulator factory.");

  // Create PDxCH processor factory.
  std::shared_ptr<pdxch_processor_factory> pdxch_proc_factory =
      create_pdxch_processor_factory_sw(config.max_processing_delay_slots, modulator_factory);
  report_fatal_error_if_not(pdxch_proc_factory, "Failed to create PDxCH processor factory.");

  // Create PRACH processor factory.
  std::shared_ptr<prach_processor_factory> prach_proc_factory =
      create_prach_processor_factory_sw(prach_demodulator_factory,
                                        *config.prach_async_executor,
                                        config.srate,
                                        max_nof_rx_ports,
                                        max_nof_prach_concurrent_requests);
  report_fatal_error_if_not(prach_proc_factory, "Failed to create PRACH processor factory.");

  // Create PUxCH processor factory.
  std::shared_ptr<puxch_processor_factory> puxch_proc_factory =
      create_puxch_processor_factory_sw(10, demodulator_factory);
  report_fatal_error_if_not(puxch_proc_factory, "Failed to create PUxCH processor factory.");

  // Create amplitude control factory.
  std::shared_ptr<amplitude_controller_factory> amplitude_control_factory;
  if (config.logger->debug.enabled() || config.amplitude_config.enable_clipping) {
    amplitude_control_factory = create_amplitude_controller_clipping_factory(config.amplitude_config);
  } else {
    amplitude_control_factory = create_amplitude_controller_scaling_factory(config.amplitude_config.input_gain_dB);
  }
  report_fatal_error_if_not(amplitude_control_factory, "Failed to create amplitude controller factory.");

  // Create Downlink processor factory.
  std::shared_ptr<lower_phy_downlink_processor_factory> downlink_proc_factory =
      create_downlink_processor_factory_sw(pdxch_proc_factory, amplitude_control_factory);
  report_fatal_error_if_not(downlink_proc_factory, "Failed to create downlink processor factory.");

  // Create Uplink processor factory.
  std::shared_ptr<lower_phy_uplink_processor_factory> uplink_proc_factory =
      create_uplink_processor_factory_sw(prach_proc_factory, puxch_proc_factory);
  report_fatal_error_if_not(uplink_proc_factory, "Failed to create uplink processor factory.");

  return create_lower_phy_factory_sw(downlink_proc_factory, uplink_proc_factory);
}

} // namespace srsran
