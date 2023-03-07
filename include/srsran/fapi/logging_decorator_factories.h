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

#include "srsran/fapi/slot_data_message_notifier.h"
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi/slot_time_message_notifier.h"
#include <memory>

namespace srsran {
namespace fapi {

/// Decorates the given slot message gateway with logging capabilities.
std::unique_ptr<slot_message_gateway> create_logging_slot_gateway(slot_message_gateway& gateway);

/// Decorates the given slot data notifier with logging capabilities.
std::unique_ptr<slot_data_message_notifier> create_logging_slot_data_notifier(slot_data_message_notifier& notifier);

/// Decorates the given slot time notifier with logging capabilities.
std::unique_ptr<slot_time_message_notifier> create_logging_slot_time_notifier(slot_time_message_notifier& notifier);

} // namespace fapi
} // namespace srsran
