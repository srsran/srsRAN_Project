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
#include "ofdm_prach_demodulator_impl.h"

using namespace srsgnb;

namespace {
class ofdm_modulator_factory_generic : public ofdm_modulator_factory
{
private:
  std::shared_ptr<dft_processor_factory> dft_factory;

public:
  ofdm_modulator_factory_generic(const ofdm_factory_generic_configuration& config) : dft_factory(config.dft_factory)
  {
    srsgnb_assert(config.dft_factory, "Invalid DFT factory.");
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
    srsgnb_assert(config.dft_factory, "Invalid DFT factory.");
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

class ofdm_prach_demodulator_factory_sw : public ofdm_prach_demodulator_factory
{
private:
  std::shared_ptr<dft_processor_factory> dft_factory;
  unsigned                               dft_size_15kHz;

public:
  ofdm_prach_demodulator_factory_sw(std::shared_ptr<dft_processor_factory> dft_factory_, unsigned dft_size_15kHz_) :
    dft_factory(dft_factory_), dft_size_15kHz(dft_size_15kHz_)
  {
    srsgnb_assert(dft_factory, "Invalid DFT factory.");
    srsgnb_assert(dft_size_15kHz, "Invalid DFT size for 15kHz.");
  }

  std::unique_ptr<ofdm_prach_demodulator> create() override
  {
    dft_processor::configuration dft_config_1_25_kHz;
    dft_config_1_25_kHz.size = (dft_size_15kHz * 15000) / 1250;
    dft_config_1_25_kHz.dir  = dft_processor::direction::DIRECT;

    dft_processor::configuration dft_config_5_kHz;
    dft_config_5_kHz.size = (dft_size_15kHz * 15000) / 5000;
    dft_config_5_kHz.dir  = dft_processor::direction::DIRECT;

    return std::make_unique<ofdm_prach_demodulator_impl>(dft_factory->create(dft_config_1_25_kHz),
                                                         dft_factory->create(dft_config_5_kHz));
  }
};

} // namespace

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

std::shared_ptr<ofdm_prach_demodulator_factory>
srsgnb::create_ofdm_prach_demodulator_factory_sw(std::shared_ptr<dft_processor_factory> dft_factory,
                                                 unsigned                               dft_size_15kHz)
{
  return std::make_shared<ofdm_prach_demodulator_factory_sw>(dft_factory, dft_size_15kHz);
}
