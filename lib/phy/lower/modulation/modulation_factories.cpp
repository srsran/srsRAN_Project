/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/lower/modulation/modulation_factories.h"
#include "ofdm_demodulator_impl.h"
#include "ofdm_modulator_impl.h"

namespace srsgnb {
class ofdm_modulator_factory_generic : public ofdm_modulator_factory
{
private:
  std::shared_ptr<dft_processor_factory> dft_factory;

public:
  ofdm_modulator_factory_generic(const ofdm_factory_generic_configuration& config) : dft_factory(config.dft_factory)
  {
    srsran_assert(config.dft_factory, "Invalid DFT factory.");
  }

  std::unique_ptr<ofdm_symbol_modulator>
  create_ofdm_symbol_modulator(const ofdm_modulator_configuration& config) override
  {
    ofdm_modulator_common_configuration common_config;
    common_config.dft = dft_factory->create({config.dft_size, dft_processor::direction::INVERSE});
    return std::make_unique<ofdm_symbol_modulator_impl>(common_config, config);
  }

  std::unique_ptr<ofdm_slot_modulator> create_ofdm_slot_modulator(const ofdm_modulator_configuration& config) override
  {
    ofdm_modulator_common_configuration common_config;
    common_config.dft = dft_factory->create({config.dft_size, dft_processor::direction::INVERSE});
    return std::make_unique<ofdm_slot_modulator_impl>(common_config, config);
  }
};

class ofdm_demodulator_factory_generic : public ofdm_demodulator_factory
{
private:
  std::shared_ptr<dft_processor_factory> dft_factory;

public:
  ofdm_demodulator_factory_generic(const ofdm_factory_generic_configuration& config) : dft_factory(config.dft_factory)
  {
    srsran_assert(config.dft_factory, "Invalid DFT factory.");
  }

  std::unique_ptr<ofdm_symbol_demodulator>
  create_ofdm_symbol_demodulator(const ofdm_demodulator_configuration& config) override
  {
    ofdm_demodulator_common_configuration common_config;
    common_config.dft = dft_factory->create({config.dft_size, dft_processor::direction::INVERSE});
    return std::make_unique<ofdm_symbol_demodulator_impl>(common_config, config);
  }

  std::unique_ptr<ofdm_slot_demodulator>
  create_ofdm_slot_demodulator(const ofdm_demodulator_configuration& config) override
  {
    ofdm_demodulator_common_configuration common_config;
    common_config.dft = dft_factory->create({config.dft_size, dft_processor::direction::DIRECT});
    return std::make_unique<ofdm_slot_demodulator_impl>(common_config, config);
  }
};

} // namespace srsgnb

using namespace srsgnb;

std::shared_ptr<ofdm_modulator_factory>
srsgnb::create_ofdm_modulator_factory_generic(ofdm_factory_generic_configuration& config)
{
  return std::make_shared<ofdm_modulator_factory_generic>(config);
}

std::shared_ptr<ofdm_demodulator_factory>
srsgnb::create_ofdm_demodulator_factory_generic(ofdm_factory_generic_configuration& config)
{
  return std::make_shared<ofdm_demodulator_factory_generic>(config);
}
