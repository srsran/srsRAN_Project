/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lower_phy_sample_factory.h"
#include "srsgnb/phy/lower/lower_phy_factory.h"

using namespace srsgnb;

std::unique_ptr<lower_phy> srsgnb::create_lower_phy(lower_phy_configuration& config)
{
  // Create DFT factory.
  dft_processor_factory_fftw_configuration dft_factory_fftw_config;
  dft_factory_fftw_config.avoid_wisdom               = false;
  dft_factory_fftw_config.wisdom_filename            = "";
  std::unique_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw(dft_factory_fftw_config);
  srsran_assert(dft_factory, "Failed to create DFT factory.");

  // Create OFDM modulator factory.
  ofdm_factory_generic_configuration ofdm_factory_config;
  ofdm_factory_config.dft_factory = dft_factory.get();
  std::unique_ptr<ofdm_modulator_factory> modulator_factory =
      create_ofdm_modulator_factory_generic(ofdm_factory_config);
  srsran_always_assert(modulator_factory, "Failed to create OFDM modulator factory.");

  // Create OFDM demodulator factory.
  std::unique_ptr<ofdm_demodulator_factory> demodulator_factory =
      create_ofdm_demodulator_factory_generic(ofdm_factory_config);
  srsran_always_assert(demodulator_factory, "Failed to create OFDM demodulator factory.");

  // Create Lower PHY factory.
  lower_phy_factory_generic_configuration lower_phy_factory_config;
  lower_phy_factory_config.modulator_factory      = modulator_factory.get();
  lower_phy_factory_config.demodulator_factory    = demodulator_factory.get();
  std::unique_ptr<lower_phy_factory> lphy_factory = create_lower_phy_factory_generic(lower_phy_factory_config);
  srsran_always_assert(lphy_factory, "Failed to create lower PHY factory.");

  return lphy_factory->create(config);
}