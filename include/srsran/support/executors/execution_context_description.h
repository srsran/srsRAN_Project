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

namespace srsran {
namespace execution_context {

/// \brief Get number of concurrent workers in the current execution context.
///
/// This function will return 1 if called from within a single task worker and the number of threads of a thread pool
/// if called from within the same thread pool.
[[nodiscard]] unsigned get_current_nof_concurrent_workers();

/// \brief Get the index of the worker in the current execution context. If the worker is a single task_worker, this
// function will return 0. If the worker belongs to a thread pool, it will return the index of the thread in the pool.
[[nodiscard]] unsigned get_current_worker_index();

} // namespace execution_context
} // namespace srsran
