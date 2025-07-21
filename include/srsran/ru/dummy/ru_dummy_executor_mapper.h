/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
