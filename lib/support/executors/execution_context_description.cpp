/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
