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

#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi/slot_time_message_notifier.h"
#include "srsran/ran/subcarrier_spacing.h"
#include <memory>

namespace srsran {

class task_executor;

namespace fapi {

/// Buffered decorator modules.
struct buffered_decorator_modules {
  std::unique_ptr<slot_message_gateway>       gateway;
  std::unique_ptr<slot_time_message_notifier> notifier;
};

/// Creates the buffered decorator modules.
std::unique_ptr<buffered_decorator_modules>
create_buffered_decorator_modules(unsigned                    l2_nof_slots_ahead,
                                  subcarrier_spacing          scs,
                                  slot_message_gateway&       gateway,
                                  task_executor&              executor,
                                  slot_time_message_notifier& delayed_notifier);

} // namespace fapi
} // namespace srsran
