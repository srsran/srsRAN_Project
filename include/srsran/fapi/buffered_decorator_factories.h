/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi/buffered_decorator.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <memory>

namespace srsran {

class task_executor;

namespace fapi {

class slot_message_gateway;

/// Creates a buffered decorator.
std::unique_ptr<buffered_decorator> create_buffered_decorator(unsigned              l2_nof_slots_ahead,
                                                              subcarrier_spacing    scs,
                                                              slot_message_gateway& gateway,
                                                              task_executor&        executor);

} // namespace fapi
} // namespace srsran
