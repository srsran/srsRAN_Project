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

#include "srsran/phy/upper/channel_processors/ssb/factories.h"
#include "logging_ssb_processor_decorator.h"
#include "pbch_encoder_impl.h"
#include "pbch_modulator_impl.h"
#include "ssb_processor_impl.h"
#include "ssb_processor_pool.h"

using namespace srsran;

namespace {

class pbch_encoder_factory_sw : public pbch_encoder_factory
{
public:
  pbch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory>          crc_calc_factory_,
                          std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                          std::shared_ptr<polar_factory>                   polar_fec_factory_) :
    crc_calc_factory(std::move(crc_calc_factory_)),
    prg_factory(std::move(prg_factory_)),
    polar_fec_factory(std::move(polar_fec_factory_))
  {
    srsran_assert(crc_calc_factory, "Invalid CRC factory.");
    srsran_assert(prg_factory, "Invalid CRC factory.");
    srsran_assert(polar_fec_factory, "Invalid polar factory.");
  }

  std::unique_ptr<pbch_encoder> create() override
  {
    return std::make_unique<pbch_encoder_impl>(
        create_crc_calculator_factory_sw("auto")->create(crc_generator_poly::CRC24C),
        prg_factory->create(),
        polar_fec_factory->create_interleaver(),
        polar_fec_factory->create_allocator(),
        polar_fec_factory->create_code(),
        polar_fec_factory->create_encoder(),
        polar_fec_factory->create_rate_matcher());
  }

private:
  std::shared_ptr<crc_calculator_factory>          crc_calc_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<polar_factory>                   polar_fec_factory;
};

class pbch_modulator_factory_sw : public pbch_modulator_factory
{
private:
  std::shared_ptr<modulation_mapper_factory>       modulator_factory;
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;

public:
  pbch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory_,
                            std::shared_ptr<pseudo_random_generator_factory> prg_factory_) :
    modulator_factory(std::move(modulator_factory_)), prg_factory(std::move(prg_factory_))
  {
    srsran_assert(modulator_factory, "Invalid modulator factory.");
    srsran_assert(prg_factory, "Invalid PRG factory.");
  }

  std::unique_ptr<pbch_modulator> create() override
  {
    return std::make_unique<pbch_modulator_impl>(modulator_factory->create(), prg_factory->create());
  }
};

class ssb_processor_factory_sw : public ssb_processor_factory
{
public:
  explicit ssb_processor_factory_sw(ssb_processor_factory_sw_configuration& config) :
    encoder_factory(std::move(config.encoder_factory)),
    modulator_factory(std::move(config.modulator_factory)),
    dmrs_factory(std::move(config.dmrs_factory)),
    pss_factory(std::move(config.pss_factory)),
    sss_factory(std::move(config.sss_factory))
  {
    srsran_assert(encoder_factory, "Invalid encoder factory");
    srsran_assert(modulator_factory, "Invalid modulator factory");
    srsran_assert(dmrs_factory, "Invalid DMRS factory");
    srsran_assert(pss_factory, "Invalid PSS factory");
    srsran_assert(sss_factory, "Invalid SSS factory");
  }

  std::unique_ptr<ssb_processor> create() override
  {
    ssb_processor_config config;
    config.encoder   = encoder_factory->create();
    config.modulator = modulator_factory->create();
    config.dmrs      = dmrs_factory->create();
    config.pss       = pss_factory->create();
    config.sss       = sss_factory->create();

    return std::make_unique<ssb_processor_impl>(std::move(config));
  }

  std::unique_ptr<ssb_pdu_validator> create_validator() override
  {
    return std::make_unique<ssb_processor_validator_impl>();
  }

private:
  std::shared_ptr<pbch_encoder_factory>        encoder_factory;
  std::shared_ptr<pbch_modulator_factory>      modulator_factory;
  std::shared_ptr<dmrs_pbch_processor_factory> dmrs_factory;
  std::shared_ptr<pss_processor_factory>       pss_factory;
  std::shared_ptr<sss_processor_factory>       sss_factory;
};

class ssb_processor_pool_factory : public ssb_processor_factory
{
public:
  ssb_processor_pool_factory(std::shared_ptr<ssb_processor_factory> factory_, unsigned nof_concurrent_threads_) :
    factory(std::move(factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    srsran_assert(factory, "Invalid SSB processor factory.");
    srsran_assert(nof_concurrent_threads > 1, "Number of concurrent threads must be greater than one.");
  }

  std::unique_ptr<ssb_processor> create() override
  {
    if (!processors) {
      std::vector<std::unique_ptr<ssb_processor>> instances(nof_concurrent_threads);
      std::generate(instances.begin(), instances.end(), [this]() { return factory->create(); });
      processors = std::make_shared<ssb_processor_pool::ssb_processor_pool_type>(instances);
    }

    return std::make_unique<ssb_processor_pool>(std::move(processors));
  }

  std::unique_ptr<ssb_processor> create(srslog::basic_logger& logger) override
  {
    if (!processors) {
      std::vector<std::unique_ptr<ssb_processor>> instances(nof_concurrent_threads);
      std::generate(instances.begin(), instances.end(), [this, &logger]() { return factory->create(logger); });
      processors = std::make_shared<ssb_processor_pool::ssb_processor_pool_type>(instances);
    }

    return std::make_unique<ssb_processor_pool>(std::move(processors));
  }

  std::unique_ptr<ssb_pdu_validator> create_validator() override { return factory->create_validator(); }

private:
  std::shared_ptr<ssb_processor_factory>                       factory;
  unsigned                                                     nof_concurrent_threads;
  std::shared_ptr<ssb_processor_pool::ssb_processor_pool_type> processors;
};

} // namespace

std::shared_ptr<pbch_encoder_factory>
srsran::create_pbch_encoder_factory_sw(std::shared_ptr<crc_calculator_factory>          crc_factory,
                                       std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                       std::shared_ptr<polar_factory>                   polar_factory)
{
  return std::make_shared<pbch_encoder_factory_sw>(
      std::move(crc_factory), std::move(prg_factory), std::move(polar_factory));
}

std::shared_ptr<pbch_modulator_factory>
srsran::create_pbch_modulator_factory_sw(std::shared_ptr<modulation_mapper_factory>       modulator_factory,
                                         std::shared_ptr<pseudo_random_generator_factory> prg_factory)
{
  return std::make_shared<pbch_modulator_factory_sw>(std::move(modulator_factory), std::move(prg_factory));
}

std::shared_ptr<ssb_processor_factory>
srsran::create_ssb_processor_factory_sw(ssb_processor_factory_sw_configuration& config)
{
  return std::make_shared<ssb_processor_factory_sw>(config);
}

std::shared_ptr<ssb_processor_factory>
srsran::create_ssb_processor_pool_factory(std::shared_ptr<ssb_processor_factory> processor_factory,
                                          unsigned                               nof_concurrent_threads)
{
  return std::make_shared<ssb_processor_pool_factory>(processor_factory, nof_concurrent_threads);
}

std::unique_ptr<ssb_processor> ssb_processor_factory::create(srslog::basic_logger& logger)
{
  return std::make_unique<logging_ssb_processor_decorator>(logger, create());
}
