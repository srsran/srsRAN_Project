/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lower_phy_example_factory.h"
#include "srsgnb/phy/lower/lower_phy_factory.h"

using namespace srsgnb;

std::unique_ptr<lower_phy> srsgnb::create_lower_phy(lower_phy_configuration& config)
{
  // Create DFT factory. It tries to create a FFTW based factory. If FFTW library is not available, it creates a FFTX
  // based factory.
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw();
  if (dft_factory == nullptr) {
    dft_factory = create_dft_processor_factory_fftx();
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
      create_ofdm_prach_demodulator_factory_sw(dft_factory, config.srate.get_dft_size(15e3));

  // Create PRACH processor factory.
  std::shared_ptr<prach_processor_factory> prach_processor_factory =
      create_prach_processor_factory_sw(prach_demodulator_factory, config.srate.get_dft_size(15e3), 1);
  report_fatal_error_if_not(prach_processor_factory, "Failed to create PRACH processor factory.");

  // Create amplitude control factory.
  std::shared_ptr<amplitude_controller_factory> amplitude_control_factory = create_amplitude_controller_factory();
  report_fatal_error_if_not(amplitude_control_factory, "Failed to create amplitude controller factory.");

  // Create Lower PHY factory.
  std::shared_ptr<lower_phy_factory> lphy_factory = create_lower_phy_factory_sw(
      modulator_factory, demodulator_factory, prach_processor_factory, amplitude_control_factory);
  report_fatal_error_if_not(lphy_factory, "Failed to create lower PHY factory.");

  return lphy_factory->create(config);
}
