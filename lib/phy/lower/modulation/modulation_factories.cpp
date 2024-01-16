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

#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "ofdm_demodulator_impl.h"
#include "ofdm_modulator_impl.h"
#include "ofdm_prach_demodulator_impl.h"

using namespace srsran;

namespace {
class ofdm_modulator_factory_generic : public ofdm_modulator_factory
{
private:
  std::shared_ptr<dft_processor_factory> dft_factory;

public:
  explicit ofdm_modulator_factory_generic(ofdm_factory_generic_configuration config) :
    dft_factory(std::move(config.dft_factory))
  {
    srsran_assert(dft_factory, "Invalid DFT factory.");
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
  explicit ofdm_demodulator_factory_generic(ofdm_factory_generic_configuration config) :
    dft_factory(std::move(config.dft_factory))
  {
    srsran_assert(dft_factory, "Invalid DFT factory.");
  }

  std::unique_ptr<ofdm_symbol_demodulator>
  create_ofdm_symbol_demodulator(const ofdm_demodulator_configuration& config) override
  {
    ofdm_demodulator_common_configuration common_config;
    common_config.dft = dft_factory->create({config.dft_size, dft_processor::direction::DIRECT});
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
  sampling_rate                          srate;

public:
  ofdm_prach_demodulator_factory_sw(std::shared_ptr<dft_processor_factory> dft_factory_, sampling_rate srate_) :
    dft_factory(std::move(dft_factory_)), srate(srate_)
  {
    srsran_assert(dft_factory, "Invalid DFT factory.");
  }

  std::unique_ptr<ofdm_prach_demodulator> create() override
  {
    using int_type = std::underlying_type_t<prach_subcarrier_spacing>;
    ofdm_prach_demodulator_impl::dft_processors_table dft_processors;
    for (int_type i_scs     = static_cast<int_type>(prach_subcarrier_spacing::kHz15),
                  i_scs_end = static_cast<int_type>(prach_subcarrier_spacing::invalid);
         i_scs != i_scs_end;
         ++i_scs) {
      prach_subcarrier_spacing     ra_scs     = static_cast<prach_subcarrier_spacing>(i_scs);
      dft_processor::configuration dft_config = {.size = srate.get_dft_size(ra_scs_to_Hz(ra_scs)),
                                                 .dir  = dft_processor::direction::DIRECT};
      dft_processors.emplace(ra_scs, dft_factory->create(dft_config));
    }
    return std::make_unique<ofdm_prach_demodulator_impl>(srate, std::move(dft_processors));
  }
};

} // namespace

std::shared_ptr<ofdm_modulator_factory>
srsran::create_ofdm_modulator_factory_generic(ofdm_factory_generic_configuration& config)
{
  return std::make_shared<ofdm_modulator_factory_generic>(config);
}

std::shared_ptr<ofdm_demodulator_factory>
srsran::create_ofdm_demodulator_factory_generic(ofdm_factory_generic_configuration& config)
{
  return std::make_shared<ofdm_demodulator_factory_generic>(config);
}

std::shared_ptr<ofdm_prach_demodulator_factory>
srsran::create_ofdm_prach_demodulator_factory_sw(std::shared_ptr<dft_processor_factory> dft_factory,
                                                 sampling_rate                          srate)
{
  return std::make_shared<ofdm_prach_demodulator_factory_sw>(std::move(dft_factory), srate);
}
