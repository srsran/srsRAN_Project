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
    srsgnb_assert(config.modulator_factory, "Invalid modulator factory.");
    srsgnb_assert(config.demodulator_factory, "Invalid demodulator factory.");
  }

  std::unique_ptr<srsgnb::lower_phy> create(const lower_phy_configuration& config) override
  {
    lower_phy_common_configuration common_config;
    common_config.modulators.reserve(config.sectors.size());
    common_config.demodulators.reserve(config.sectors.size());

    // For each sector, create a modulator.
    for (const lower_phy_sector_description& sector : config.sectors) {
      // Prepare sector modulator.
      ofdm_modulator_configuration configuration = {};
      configuration.numerology                   = to_numerology_value(config.scs);
      configuration.bw_rb                        = sector.bandwidth_rb;
      configuration.dft_size                     = config.dft_size_15kHz / pow2(to_numerology_value(config.scs));
      configuration.cp                           = config.cp;
      configuration.scale                        = config.tx_scale;
      configuration.center_freq_hz               = sector.dl_freq_hz;

      // Create modulator.
      common_config.modulators.emplace_back(modulator_factory.create_ofdm_symbol_modulator(configuration));

      // Make sure the modulator creation is successful.
      srsgnb_assert(common_config.modulators.back() != nullptr, "Error: failed to create OFDM modulator.");
    }

    // For each sector, create a demodulator.
    for (const lower_phy_sector_description& sector : config.sectors) {
      // Prepare sector demodulator.
      ofdm_demodulator_configuration configuration = {};
      configuration.numerology                     = to_numerology_value(config.scs);
      configuration.bw_rb                          = sector.bandwidth_rb;
      configuration.dft_size                       = config.dft_size_15kHz / pow2(to_numerology_value(config.scs));
      configuration.cp                             = config.cp;
      configuration.scale                          = config.tx_scale;
      configuration.center_freq_hz                 = sector.ul_freq_hz;

      // Create demodulator.
      common_config.demodulators.emplace_back(demodulator_factory.create_ofdm_symbol_demodulator(configuration));

      // Make sure the demodulator creation is successful.
      srsgnb_assert(common_config.demodulators.back() != nullptr, "Error: failed to create OFDM demodulator.");
    }

    return std::make_unique<lower_phy_impl>(common_config, config);
  }
};

std::unique_ptr<lower_phy_factory>
srsgnb::create_lower_phy_factory_generic(lower_phy_factory_generic_configuration& config)
{
  return std::make_unique<lower_phy_factory_generic>(config);
}
