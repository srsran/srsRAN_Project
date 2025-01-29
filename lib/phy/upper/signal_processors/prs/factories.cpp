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

#include "srsran/phy/upper/signal_processors/prs/factories.h"
#include "logging_prs_generator_decorator.h"
#include "prs_generator_impl.h"
#include "prs_generator_pool_factory.h"
#include "prs_generator_validator_impl.h"

using namespace srsran;

namespace {

/// Implements a generic PRS generator factory.
class prs_generator_factory_generic_impl : public prs_generator_factory
{
public:
  prs_generator_factory_generic_impl(std::shared_ptr<pseudo_random_generator_factory> prg_factory_,
                                     std::shared_ptr<channel_precoder_factory>        precoder_factory_) :
    prg_factory(std::move(prg_factory_)), precoder_factory(std::move(precoder_factory_))
  {
    srsran_assert(prg_factory, "Invalid pseudo-random sequence generator factory.");
    srsran_assert(precoder_factory, "Invalid channel precoder factory.");
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator> create() override
  {
    return std::make_unique<prs_generator_impl>(prg_factory->create(), precoder_factory->create());
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator_validator> create_validator() override
  {
    return std::make_unique<prs_generator_validator_impl>();
  }

private:
  std::shared_ptr<pseudo_random_generator_factory> prg_factory;
  std::shared_ptr<channel_precoder_factory>        precoder_factory;
};

} // namespace

std::unique_ptr<prs_generator> srsran::prs_generator_factory::create(srslog::basic_logger& logger)
{
  return std::make_unique<logging_prs_generator_decorator>(logger, create());
}

std::shared_ptr<prs_generator_factory>
srsran::create_prs_generator_generic_factory(std::shared_ptr<pseudo_random_generator_factory> prg_factory,
                                             std::shared_ptr<channel_precoder_factory>        precoder_factory)
{
  return std::make_shared<prs_generator_factory_generic_impl>(std::move(prg_factory), std::move(precoder_factory));
}

std::shared_ptr<prs_generator_factory>
srsran::create_prs_generator_pool_factory(std::shared_ptr<prs_generator_factory> generator_factory,
                                          unsigned                               nof_concurrent_threads)
{
  return std::make_shared<prs_generator_pool_factory>(std::move(generator_factory), nof_concurrent_threads);
}
