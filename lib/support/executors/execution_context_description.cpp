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

#include "srsran/support/executors/execution_context_description.h"
#include "execution_context_description_setup.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

thread_local unsigned nof_concurrent_workers = 1;
thread_local unsigned worker_index           = 0;

unsigned srsran::execution_context::get_current_nof_concurrent_workers()
{
  return nof_concurrent_workers;
}

unsigned srsran::execution_context::get_current_worker_index()
{
  return worker_index;
}

void srsran::execution_context::set_execution_context_description(unsigned nof_concurrent_workers_,
                                                                  unsigned worker_index_)
{
  srsran_sanity_check(nof_concurrent_workers_ > 0, "Invalid number of workers");
  nof_concurrent_workers = nof_concurrent_workers_;
  worker_index           = worker_index_;
}
