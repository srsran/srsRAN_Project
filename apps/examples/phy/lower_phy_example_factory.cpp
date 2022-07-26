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

std::unique_ptr<lower_phy_controller> srsgnb::create_lower_phy(lower_phy_configuration& config)
{
  // Create DFT factory. It tries to create a FFTW based factory. If FFTW library is not available, it creates a FFTX
  // based factory.
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw();
  if (dft_factory == nullptr) {
    dft_factory = create_dft_processor_factory_fftx();
  }
  srsgnb_assert(dft_factory, "Failed to create DFT factory.");

  // Create OFDM modulator factory.
  ofdm_factory_generic_configuration ofdm_common_config;
  ofdm_common_config.dft_factory                            = dft_factory;
  std::shared_ptr<ofdm_modulator_factory> modulator_factory = create_ofdm_modulator_factory_generic(ofdm_common_config);
  REPORT_FATAL_ERROR_IF_NOT(modulator_factory, "Failed to create OFDM modulator factory.");

  // Create OFDM demodulator factory.
  std::shared_ptr<ofdm_demodulator_factory> demodulator_factory =
      create_ofdm_demodulator_factory_generic(ofdm_common_config);
  REPORT_FATAL_ERROR_IF_NOT(demodulator_factory, "Failed to create OFDM demodulator factory.");

  // Create Lower PHY factory.
  lower_phy_factory_generic_configuration lower_phy_common_config;
  lower_phy_common_config.modulator_factory       = modulator_factory;
  lower_phy_common_config.demodulator_factory     = demodulator_factory;
  std::shared_ptr<lower_phy_factory> lphy_factory = create_lower_phy_factory_generic(lower_phy_common_config);
  REPORT_FATAL_ERROR_IF_NOT(lphy_factory, "Failed to create lower PHY factory.");

  return lphy_factory->create(config);
}