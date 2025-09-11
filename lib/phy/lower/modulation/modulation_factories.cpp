/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "ofdm_demodulator_pool.h"
#include "ofdm_modulator_impl.h"
#include "ofdm_modulator_pool.h"
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
    return std::make_unique<ofdm_slot_modulator_impl>(config, create_ofdm_symbol_modulator(config));
  }
};

class ofdm_modulator_pool_factory : public ofdm_modulator_factory
{
public:
  explicit ofdm_modulator_pool_factory(std::shared_ptr<ofdm_modulator_factory> base_, unsigned max_nof_threads_) :
    base(std::move(base_)), max_nof_threads(max_nof_threads_)
  {
    srsran_assert(base, "Invalid base factory.");
  }

  std::unique_ptr<ofdm_symbol_modulator>
  create_ofdm_symbol_modulator(const ofdm_modulator_configuration& config) override
  {
    // Create pool instances.
    std::vector<std::unique_ptr<ofdm_symbol_modulator>> instances(max_nof_threads);
    std::generate(
        instances.begin(), instances.end(), [this, &config]() { return base->create_ofdm_symbol_modulator(config); });

    // Create pool of modulators. As the configuration might change, it cannot be shared.
    std::shared_ptr<ofdm_symbol_modulator_pool::modulator_pool> modulators =
        std::make_shared<ofdm_symbol_modulator_pool::modulator_pool>(instances);

    return std::make_unique<ofdm_symbol_modulator_pool>(base->create_ofdm_symbol_modulator(config),
                                                        std::move(modulators));
  }

  std::unique_ptr<ofdm_slot_modulator> create_ofdm_slot_modulator(const ofdm_modulator_configuration& config) override
  {
    return std::make_unique<ofdm_slot_modulator_impl>(config, create_ofdm_symbol_modulator(config));
  }

private:
  std::shared_ptr<ofdm_modulator_factory> base;
  unsigned                                max_nof_threads;
};

class ofdm_demodulator_pool_factory : public ofdm_demodulator_factory
{
public:
  explicit ofdm_demodulator_pool_factory(std::shared_ptr<ofdm_demodulator_factory> base_, unsigned max_nof_threads_) :
    base(std::move(base_)), max_nof_threads(max_nof_threads_)
  {
    srsran_assert(base, "Invalid base factory.");
  }

  std::unique_ptr<ofdm_symbol_demodulator>
  create_ofdm_symbol_demodulator(const ofdm_demodulator_configuration& config) override
  {
    // Create pool instances.
    std::vector<std::unique_ptr<ofdm_symbol_demodulator>> instances(max_nof_threads);
    std::generate(
        instances.begin(), instances.end(), [this, &config]() { return base->create_ofdm_symbol_demodulator(config); });

    // Create pool of demodulators. As the configuration might change, it cannot be shared.
    std::shared_ptr<ofdm_symbol_demodulator_pool::demodulator_pool> demodulators =
        std::make_shared<ofdm_symbol_demodulator_pool::demodulator_pool>(instances);

    return std::make_unique<ofdm_symbol_demodulator_pool>(base->create_ofdm_symbol_demodulator(config),
                                                          std::move(demodulators));
  }

  std::unique_ptr<ofdm_slot_demodulator>
  create_ofdm_slot_demodulator(const ofdm_demodulator_configuration& config) override
  {
    return std::make_unique<ofdm_slot_demodulator_impl>(config, create_ofdm_symbol_demodulator(config));
  }

private:
  std::shared_ptr<ofdm_demodulator_factory> base;
  unsigned                                  max_nof_threads;
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
    return std::make_unique<ofdm_slot_demodulator_impl>(config, create_ofdm_symbol_demodulator(config));
  }
};

class ofdm_prach_demodulator_factory_sw : public ofdm_prach_demodulator_factory
{
private:
  std::shared_ptr<dft_processor_factory> dft_factory;
  sampling_rate                          srate;
  frequency_range                        fr;

  static constexpr std::array<prach_subcarrier_spacing, 5> fr1_prach_scs = {prach_subcarrier_spacing::kHz15,
                                                                            prach_subcarrier_spacing::kHz30,
                                                                            prach_subcarrier_spacing::kHz60,
                                                                            prach_subcarrier_spacing::kHz1_25,
                                                                            prach_subcarrier_spacing::kHz5};

  static constexpr std::array<prach_subcarrier_spacing, 1> fr2_prach_scs = {prach_subcarrier_spacing::kHz120};

public:
  ofdm_prach_demodulator_factory_sw(std::shared_ptr<dft_processor_factory> dft_factory_,
                                    sampling_rate                          srate_,
                                    frequency_range                        fr_) :
    dft_factory(std::move(dft_factory_)), srate(srate_), fr(fr_)
  {
    srsran_assert(dft_factory, "Invalid DFT factory.");
  }

  std::unique_ptr<ofdm_prach_demodulator> create() override
  {
    // Select set of valid PRACH subcarrier spacing for the given frequency range. This avoids having extremely large
    // unused DFTs.
    span<const prach_subcarrier_spacing> possible_prach_scs = (fr == frequency_range::FR1)
                                                                  ? span<const prach_subcarrier_spacing>(fr1_prach_scs)
                                                                  : span<const prach_subcarrier_spacing>(fr2_prach_scs);

    ofdm_prach_demodulator_impl::dft_processors_table dft_processors;

    for (prach_subcarrier_spacing ra_scs : possible_prach_scs) {
      // Create DFT for the given PRACH subcarrier spacing.
      dft_processor::configuration   dft_config = {.size = srate.get_dft_size(ra_scs_to_Hz(ra_scs)),
                                                   .dir  = dft_processor::direction::DIRECT};
      std::unique_ptr<dft_processor> dft_proc   = dft_factory->create(dft_config);
      srsran_assert(dft_proc,
                    "Invalid DFT processor of size {}, for subcarrier spacing of {} and sampling rate {}.",
                    dft_config.size,
                    to_string(ra_scs),
                    srate);

      // Emplace the DFT into the dictionary.
      dft_processors.emplace(ra_scs, std::move(dft_proc));
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

std::shared_ptr<ofdm_modulator_factory>
srsran::create_ofdm_modulator_pool_factory(std::shared_ptr<ofdm_modulator_factory> base, unsigned max_nof_threads)
{
  return std::make_shared<ofdm_modulator_pool_factory>(base, max_nof_threads);
}

std::shared_ptr<ofdm_demodulator_factory>
srsran::create_ofdm_demodulator_factory_generic(ofdm_factory_generic_configuration& config)
{
  return std::make_shared<ofdm_demodulator_factory_generic>(config);
}

std::shared_ptr<ofdm_demodulator_factory>
srsran::create_ofdm_demodulator_pool_factory(std::shared_ptr<ofdm_demodulator_factory> base, unsigned max_nof_threads)
{
  return std::make_shared<ofdm_demodulator_pool_factory>(base, max_nof_threads);
}

std::shared_ptr<ofdm_prach_demodulator_factory>
srsran::create_ofdm_prach_demodulator_factory_sw(std::shared_ptr<dft_processor_factory> dft_factory,
                                                 sampling_rate                          srate,
                                                 frequency_range                        fr)
{
  return std::make_shared<ofdm_prach_demodulator_factory_sw>(std::move(dft_factory), srate, fr);
}
