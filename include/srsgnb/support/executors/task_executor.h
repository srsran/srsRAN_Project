/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/unique_function.h"

namespace srsgnb {

class task_executor
{
public:
  virtual ~task_executor() = default;

  /// Dispatches a task to be executed. According to this policy, executor may invoke task inplace if it deemed safe
  /// to do so.
  virtual bool execute(unique_task task) = 0;

  /// Dispatches a task to be later executed. The task always gets enqueued for later invocation.
  virtual bool defer(unique_task task) = 0;
};

} // namespace srsgnb
