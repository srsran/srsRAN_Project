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

#include "srsran/ru/dummy/ru_dummy_executor_mapper.h"

using namespace srsran;

namespace {

/// Implements an RU dummy executor mapper.
class ru_dummy_executor_mapper_impl : public ru_dummy_executor_mapper
{
public:
  /// Contructs the executor mapper with a single executor reference.
  ru_dummy_executor_mapper_impl(task_executor& executor_) : executor(executor_) {}

  // See interface for documentation.
  task_executor& common_executor() override { return executor; }

private:
  // Actual executor.
  task_executor& executor;
};

} // namespace

std::unique_ptr<ru_dummy_executor_mapper> srsran::create_ru_dummy_executor_mapper(srsran::task_executor& executor)
{
  return std::make_unique<ru_dummy_executor_mapper_impl>(executor);
}
