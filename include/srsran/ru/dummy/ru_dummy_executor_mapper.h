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

#include "srsran/support/executors/task_executor.h"
#include <memory>
#include <vector>

namespace srsran {

class task_executor;

/// Dummy RU executor mapper interface.
class ru_dummy_executor_mapper
{
public:
  /// Default destructor.
  virtual ~ru_dummy_executor_mapper() = default;

  /// Retrieves the dummy RU executor reference.
  virtual task_executor& common_executor() = 0;
};

/// Creates an RU dummyu implementation executor mapper from a single task executor.
std::unique_ptr<ru_dummy_executor_mapper> create_ru_dummy_executor_mapper(task_executor& executor);

} // namespace srsran
