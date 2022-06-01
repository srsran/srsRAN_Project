/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/lower/lower_phy_factory.h"
#include "lower_phy_impl.h"

using namespace srsgnb;

class lower_phy_factory_generic : public lower_phy_factory
{
public:
  /// OFDM Modulation factory.
  ofdm_modulator_factory& modulator_factory;
  /// OFDM Demodulation factory.
  ofdm_demodulator_factory& demodulator_factory;

  lower_phy_factory_generic(lower_phy_factory_generic_configuration config) :
    modulator_factory(*config.modulator_factory), demodulator_factory(*config.demodulator_factory)
  {
    srsran_assert(config.modulator_factory, "Invalid modulator factory.");
    srsran_assert(config.demodulator_factory, "Invalid demodulator factory.");
  }

  std::unique_ptr<srsgnb::lower_phy> create(lower_phy_configuration& config) override
  {
    lower_phy_factory_configuration factory_config;
    factory_config.modulators.reserve(config.sectors.size());
    factory_config.demodulators.reserve(config.sectors.size());

    // For each sector, create modulator.
    for (const lower_phy_sector_description& sector : config.sectors) {
      // Prepare sector modulator.
      ofdm_modulator_configuration configuration = {};
      configuration.numerology                   = config.numerology;
      configuration.bw_rb                        = sector.bandwidth_rb;
      configuration.dft_size                     = config.dft_size_15kHz / pow2(config.numerology);
      configuration.cp                           = config.cp;
      configuration.scale                        = config.tx_scale;
      configuration.center_freq_hz               = sector.dl_freq_hz;

      // Create modulator.
      factory_config.modulators.emplace_back(modulator_factory.create_ofdm_symbol_modulator(configuration));

      // Make sure the modulator creation is successful.
      srsran_assert(factory_config.modulators.back() != nullptr, "Error: failed to create OFDM modulator.");
    }

    // For each sector, create demodulator.
    for (const lower_phy_sector_description& sector : config.sectors) {
      // Prepare sector modulator.
      ofdm_demodulator_configuration configuration = {};
      configuration.numerology                     = config.numerology;
      configuration.bw_rb                          = sector.bandwidth_rb;
      configuration.dft_size                       = config.dft_size_15kHz / pow2(config.numerology);
      configuration.cp                             = config.cp;
      configuration.scale                          = config.tx_scale;
      configuration.center_freq_hz                 = sector.ul_freq_hz;

      // Create modulator.
      factory_config.demodulators.emplace_back(demodulator_factory.create_ofdm_symbol_demodulator(configuration));

      // Make sure the modulator creation is successful.
      srsran_assert(factory_config.modulators.back() != nullptr, "Error: failed to create OFDM modulator.");
    }

    return std::make_unique<lower_phy_impl>(factory_config, config);
  }
};

std::unique_ptr<lower_phy_factory>
srsgnb::create_lower_phy_factory_generic(lower_phy_factory_generic_configuration& config)
{
  return std::make_unique<lower_phy_factory_generic>(config);
}
