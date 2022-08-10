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
  std::shared_ptr<ofdm_modulator_factory>   modulator_factory;
  std::shared_ptr<ofdm_demodulator_factory> demodulator_factory;
  std::shared_ptr<prach_processor_factory>  prach_factory;

  lower_phy_factory_generic(std::shared_ptr<ofdm_modulator_factory>&   modulator_factory_,
                            std::shared_ptr<ofdm_demodulator_factory>& demodulator_factory_,
                            std::shared_ptr<prach_processor_factory>&  prach_processor_factory_) :
    modulator_factory(modulator_factory_),
    demodulator_factory(demodulator_factory_),
    prach_factory(prach_processor_factory_)
  {
    srsgnb_assert(modulator_factory, "Invalid modulator factory.");
    srsgnb_assert(demodulator_factory, "Invalid demodulator factory.");
    srsgnb_assert(prach_factory, "Invalid PRACH processor factory.");
  }

  std::unique_ptr<srsgnb::lower_phy> create(lower_phy_configuration& config) override
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
      configuration.dft_size                     = config.srate.get_dft_size(config.scs);
      configuration.cp                           = config.cp;
      configuration.scale                        = config.tx_scale;
      configuration.center_freq_hz               = sector.dl_freq_hz;

      // Create modulator.
      common_config.modulators.emplace_back(modulator_factory->create_ofdm_symbol_modulator(configuration));

      // Make sure the modulator creation is successful.
      srsgnb_assert(common_config.modulators.back(), "Failed to create OFDM modulator.");
    }

    // For each sector, create a demodulator.
    for (const lower_phy_sector_description& sector : config.sectors) {
      // Prepare sector demodulator.
      ofdm_demodulator_configuration configuration = {};
      configuration.numerology                     = to_numerology_value(config.scs);
      configuration.bw_rb                          = sector.bandwidth_rb;
      configuration.dft_size                       = config.srate.get_dft_size(config.scs);
      configuration.cp                             = config.cp;
      configuration.scale                          = config.tx_scale;
      configuration.center_freq_hz                 = sector.ul_freq_hz;

      // Create demodulator.
      common_config.demodulators.emplace_back(demodulator_factory->create_ofdm_symbol_demodulator(configuration));

      // Make sure the demodulator creation is successful.
      srsgnb_assert(common_config.demodulators.back(), "Error: failed to create OFDM demodulator.");
    }

    common_config.prach_proc = prach_factory->create(*config.prach_async_executor);
    srsgnb_assert(common_config.prach_proc, "Failed to create PRACH processor.");

    return std::make_unique<lower_phy_impl>(std::move(common_config), config);
  }
};

std::unique_ptr<lower_phy_factory>
srsgnb::create_lower_phy_factory_sw(std::shared_ptr<ofdm_modulator_factory>   modulator_factory,
                                    std::shared_ptr<ofdm_demodulator_factory> demodulator_factory,
                                    std::shared_ptr<prach_processor_factory>  prach_processor_factory)
{
  return std::make_unique<lower_phy_factory_generic>(modulator_factory, demodulator_factory, prach_processor_factory);
}
