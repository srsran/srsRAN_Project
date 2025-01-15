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

namespace srsran {

class worker_manager_executor_getter
{
public:
  virtual ~worker_manager_executor_getter() = default;

  virtual task_executor& get_executor(const std::string& name) const = 0;
};

} // namespace srsran
