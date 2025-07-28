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

#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "prach_detector_generic_impl.h"
#include "prach_detector_pool.h"
#include "prach_generator_impl.h"
#include "srsran/phy/support/support_formatters.h"
#include "srsran/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"
#include "srsran/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

namespace {

class prach_detector_factory_sw : public prach_detector_factory
{
private:
  std::shared_ptr<dft_processor_factory>   dft_factory;
  std::shared_ptr<prach_generator_factory> prach_gen_factory;
  unsigned                                 idft_long_size;
  unsigned                                 idft_short_size;
  bool                                     combine_symbols;

public:
  prach_detector_factory_sw(std::shared_ptr<dft_processor_factory>         dft_factory_,
                            std::shared_ptr<prach_generator_factory>       prach_gen_factory_,
                            const prach_detector_factory_sw_configuration& config) :
    dft_factory(std::move(dft_factory_)),
    prach_gen_factory(std::move(prach_gen_factory_)),
    idft_long_size(config.idft_long_size),
    idft_short_size(config.idft_short_size),
    combine_symbols(config.combine_symbols)
  {
    srsran_assert(dft_factory, "Invalid DFT factory.");
    srsran_assert(prach_gen_factory, "Invalid PRACH generator factory.");
  }

  std::unique_ptr<prach_detector> create() override
  {
    dft_processor::configuration idft_long_config;
    idft_long_config.size = idft_long_size;
    idft_long_config.dir  = dft_processor::direction::INVERSE;
    dft_processor::configuration idft_short_config;
    idft_short_config.size = idft_short_size;
    idft_short_config.dir  = dft_processor::direction::INVERSE;
    return std::make_unique<prach_detector_generic_impl>(dft_factory->create(idft_long_config),
                                                         dft_factory->create(idft_short_config),
                                                         prach_gen_factory->create(),
                                                         combine_symbols);
  }

  std::unique_ptr<prach_detector_validator> create_validator() override
  {
    return std::make_unique<prach_detector_validator_impl>();
  }
};

class prach_detector_pool_factory : public prach_detector_factory
{
public:
  prach_detector_pool_factory(std::shared_ptr<prach_detector_factory> factory_, unsigned nof_concurrent_threads_) :
    factory(std::move(factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    srsran_assert(factory, "Invalid PRACH detector factory.");
    srsran_assert(nof_concurrent_threads > 1, "Number of concurrent threads must be greater than one.");
  }

  std::unique_ptr<prach_detector> create() override
  {
    if (!pool) {
      std::vector<std::unique_ptr<prach_detector>> detectors(nof_concurrent_threads);
      std::generate(detectors.begin(), detectors.end(), [this]() { return factory->create(); });
      pool = std::make_shared<prach_detector_pool::detector_pool>(detectors);
    }

    return std::make_unique<prach_detector_pool>(pool);
  }

  std::unique_ptr<prach_detector> create(srslog::basic_logger& logger, bool log_all_opportunities) override
  {
    if (!pool) {
      std::vector<std::unique_ptr<prach_detector>> detectors(nof_concurrent_threads);
      std::generate(detectors.begin(), detectors.end(), [this, &logger, log_all_opportunities]() {
        return factory->create(logger, log_all_opportunities);
      });
      pool = std::make_shared<prach_detector_pool::detector_pool>(detectors);
    }

    return std::make_unique<prach_detector_pool>(pool);
  }

  std::unique_ptr<prach_detector_validator> create_validator() override { return factory->create_validator(); }

private:
  std::shared_ptr<prach_detector_factory>             factory;
  std::shared_ptr<prach_detector_pool::detector_pool> pool;
  unsigned                                            nof_concurrent_threads;
};

class prach_generator_factory_sw : public prach_generator_factory
{
public:
  std::unique_ptr<prach_generator> create() override { return std::make_unique<prach_generator_impl>(); }
};

} // namespace

std::shared_ptr<prach_detector_factory>
srsran::create_prach_detector_factory_sw(std::shared_ptr<dft_processor_factory>         dft_factory,
                                         std::shared_ptr<prach_generator_factory>       prach_gen_factory,
                                         const prach_detector_factory_sw_configuration& config)
{
  return std::make_shared<prach_detector_factory_sw>(std::move(dft_factory), std::move(prach_gen_factory), config);
}

std::shared_ptr<prach_detector_factory>
srsran::create_prach_detector_pool_factory(std::shared_ptr<prach_detector_factory> factory,
                                           unsigned                                nof_concurrent_threads)
{
  return std::make_shared<prach_detector_pool_factory>(std::move(factory), nof_concurrent_threads);
}

std::shared_ptr<prach_generator_factory> srsran::create_prach_generator_factory_sw()
{
  return std::make_shared<prach_generator_factory_sw>();
}

namespace {

class logging_prach_detector_decorator : public prach_detector
{
  template <typename Func>
  static std::chrono::nanoseconds time_execution(Func&& func)
  {
    auto start = std::chrono::steady_clock::now();
    func();
    auto end = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
  }

public:
  logging_prach_detector_decorator(srslog::basic_logger&           logger_,
                                   bool                            log_all_opportunities_,
                                   std::unique_ptr<prach_detector> detector_) :
    logger(logger_), log_all_opportunities(log_all_opportunities_), detector(std::move(detector_))
  {
    srsran_assert(detector, "Invalid detector.");
  }

  prach_detection_result detect(const prach_buffer& input, const configuration& config) override
  {
    prach_detection_result result;
    const auto&&           func = [&]() { result = detector->detect(input, config); };

    std::chrono::nanoseconds time_ns = time_execution(func);

    if (log_all_opportunities || !result.preambles.empty()) {
      if (logger.debug.enabled()) {
        // Detailed log information, including a list of all PRACH config and result fields.
        logger.debug(config.slot.sfn(),
                     config.slot.slot_index(),
                     "PRACH: {:s} {:s} {}\n  {:n}\n  {:n}\n",
                     config,
                     result,
                     time_ns,
                     config,
                     result);
      } else {
        // Single line log entry.
        logger.info(config.slot.sfn(), config.slot.slot_index(), "PRACH: {:s} {:s} {}", config, result, time_ns);
      }
    }

    return result;
  }

private:
  srslog::basic_logger&           logger;
  bool                            log_all_opportunities;
  std::unique_ptr<prach_detector> detector;
};

} // namespace

std::unique_ptr<prach_detector> prach_detector_factory::create(srslog::basic_logger& logger, bool log_all_opportunities)
{
  return std::make_unique<logging_prach_detector_decorator>(logger, log_all_opportunities, create());
}
