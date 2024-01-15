/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/unique_function.h"
#include "srsran/support/compiler.h"

namespace srsran {

class task_executor
{
public:
  virtual ~task_executor() = default;

  /// Dispatches a task to be executed. According to this policy, executor may invoke task inplace if it deemed safe
  /// to do so.
  virtual SRSRAN_NODISCARD bool execute(unique_task task) = 0;

  /// Dispatches a task to be later executed. The task always gets enqueued for later invocation.
  virtual SRSRAN_NODISCARD bool defer(unique_task task) = 0;
};

} // namespace srsran
