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

/// Set number of workers and worker index of the current execution context.
void set_execution_context_description(unsigned nof_concurrent_workers, unsigned worker_index);

} // namespace execution_context
} // namespace srsran