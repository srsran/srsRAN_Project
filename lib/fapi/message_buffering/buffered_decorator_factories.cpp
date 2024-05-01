/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/fapi/buffered_decorator_factories.h"
#include "buffered_decorator_impl.h"

using namespace srsran;
using namespace fapi;

std::unique_ptr<buffered_decorator> srsran::fapi::create_buffered_decorator(unsigned              l2_nof_slots_ahead,
                                                                            subcarrier_spacing    scs,
                                                                            slot_message_gateway& gateway,
                                                                            task_executor&        executor)
{
  return std::make_unique<buffered_decorator_impl>(l2_nof_slots_ahead, scs, gateway, executor);
}
