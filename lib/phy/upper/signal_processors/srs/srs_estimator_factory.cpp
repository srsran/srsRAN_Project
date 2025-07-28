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

#include "srsran/phy/upper/signal_processors/srs/srs_estimator_factory.h"
#include "logging_srs_estimator_decorator.h"
#include "srs_estimator_generic_impl.h"
#include "srs_estimator_pool.h"
#include "srs_validator_generic_impl.h"
#include "srsran/phy/support/time_alignment_estimator/time_alignment_estimator_factories.h"

using namespace srsran;

namespace {

class srs_estimator_factory_generic : public srs_estimator_factory
{
public:
  srs_estimator_factory_generic(std::shared_ptr<low_papr_sequence_generator_factory> sequence_generator_factory_,
                                std::shared_ptr<time_alignment_estimator_factory>    ta_estimator_factory_,
                                unsigned                                             max_nof_prb_) :
    sequence_generator_factory(std::move(sequence_generator_factory_)),
    ta_estimator_factory(std::move(ta_estimator_factory_)),
    max_nof_prb(max_nof_prb_)
  {
    srsran_assert(sequence_generator_factory, "Invalid sequence generator factory.");
    srsran_assert(ta_estimator_factory, "Invalid TA estimator factory.");
    srsran_assert(max_nof_prb != 0, "Maximum number of PRB cannot be zero.");
  }

  std::unique_ptr<srs_estimator> create() override
  {
    srs_estimator_generic_impl::dependencies deps;
    deps.sequence_generator = sequence_generator_factory->create();
    deps.ta_estimator       = ta_estimator_factory->create();

    return std::make_unique<srs_estimator_generic_impl>(std::move(deps), max_nof_prb);
  }

  std::unique_ptr<srs_estimator_configuration_validator> create_validator() override
  {
    return std::make_unique<srs_validator_generic_impl>(max_nof_prb);
  }

private:
  std::shared_ptr<low_papr_sequence_generator_factory> sequence_generator_factory;
  std::shared_ptr<time_alignment_estimator_factory>    ta_estimator_factory;
  unsigned                                             max_nof_prb;
};

class srs_estimator_factory_pool : public srs_estimator_factory
{
public:
  srs_estimator_factory_pool(std::shared_ptr<srs_estimator_factory> factory_, unsigned nof_concurrent_threads_) :
    factory(std::move(factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    srsran_assert(factory, "Invalid factory.");
    srsran_assert(nof_concurrent_threads != 0, "Number of threads must be larger than 0.");
  }

  // See interface for documentation.
  std::unique_ptr<srs_estimator> create() override
  {
    if (!pool) {
      std::vector<std::unique_ptr<srs_estimator>> estimators(nof_concurrent_threads);

      for (auto& estimator : estimators) {
        estimator = factory->create();
      }

      pool = std::make_shared<srs_estimator_pool::estimator_pool>(estimators);
    }

    return std::make_unique<srs_estimator_pool>(pool);
  }

  // See interface for documentation.
  std::unique_ptr<srs_estimator_configuration_validator> create_validator() override
  {
    return factory->create_validator();
  }

private:
  std::shared_ptr<srs_estimator_factory>              factory;
  std::shared_ptr<srs_estimator_pool::estimator_pool> pool;
  unsigned                                            nof_concurrent_threads;
};

} // namespace

std::shared_ptr<srs_estimator_factory> srsran::create_srs_estimator_generic_factory(
    std::shared_ptr<low_papr_sequence_generator_factory> sequence_generator_factory,
    std::shared_ptr<time_alignment_estimator_factory>    ta_estimator_factory,
    unsigned                                             max_nof_prb)
{
  return std::make_shared<srs_estimator_factory_generic>(
      std::move(sequence_generator_factory), std::move(ta_estimator_factory), max_nof_prb);
}

std::shared_ptr<srs_estimator_factory>
srsran::create_srs_estimator_pool(std::shared_ptr<srs_estimator_factory> base_factory, unsigned max_nof_threads)
{
  return std::make_shared<srs_estimator_factory_pool>(std::move(base_factory), max_nof_threads);
}

std::unique_ptr<srs_estimator> srs_estimator_factory::create(srslog::basic_logger& logger)
{
  return std::make_unique<logging_srs_estimator_decorator>(logger, create());
}
