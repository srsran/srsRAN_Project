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

#pragma once

#include "prs_generator_pool.h"

namespace srsran {

/// Concurrent PRS generator pool factory - instantiates PRS generators that can be executed concurrently.
class prs_generator_pool_factory : public prs_generator_factory
{
public:
  prs_generator_pool_factory(std::shared_ptr<prs_generator_factory> factory_, unsigned nof_concurrent_threads_) :
    factory(std::move(factory_)), nof_concurrent_threads(nof_concurrent_threads_)
  {
    srsran_assert(factory, "Invalid PRS generator factory.");
    srsran_assert(nof_concurrent_threads > 1, "Number of concurrent threads must be greater than one.");
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator> create() override
  {
    if (!pool) {
      std::vector<std::unique_ptr<prs_generator>> generators(nof_concurrent_threads);
      std::generate(generators.begin(), generators.end(), [this]() { return factory->create(); });
      pool = std::make_shared<prs_generator_pool::generator_pool_type>(generators);
    }

    return std::make_unique<prs_generator_pool>(pool);
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator> create(srslog::basic_logger& logger) override
  {
    if (!pool) {
      std::vector<std::unique_ptr<prs_generator>> generators(nof_concurrent_threads);
      std::generate(generators.begin(), generators.end(), [this, &logger]() { return factory->create(logger); });
      pool = std::make_shared<prs_generator_pool::generator_pool_type>(generators);
    }

    return std::make_unique<prs_generator_pool>(pool);
  }

  // See interface for documentation.
  std::unique_ptr<prs_generator_validator> create_validator() override { return factory->create_validator(); }

private:
  /// Base factory.
  std::shared_ptr<prs_generator_factory> factory;
  /// Number of threads that use the instance.
  unsigned nof_concurrent_threads;
  /// Collection of PRS generators.
  std::shared_ptr<prs_generator_pool::generator_pool_type> pool;
};

} // namespace srsran
