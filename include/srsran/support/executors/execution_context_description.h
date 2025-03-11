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
