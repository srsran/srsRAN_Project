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

#include "srsran/fapi/slot_data_message_notifier.h"
#include "srsran/fapi/slot_error_message_notifier.h"
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi/slot_time_message_notifier.h"
#include <memory>

namespace srsran {
namespace fapi {

/// Decorates the given slot message gateway with logging capabilities.
std::unique_ptr<slot_message_gateway> create_logging_slot_gateway(slot_message_gateway& gateway);

/// Decorates the given slot time notifier with logging capabilities.
std::unique_ptr<slot_time_message_notifier> create_logging_slot_time_notifier(slot_time_message_notifier& notifier);

/// Decorates the given slot error notifier with logging capabilities.
std::unique_ptr<slot_error_message_notifier> create_logging_slot_error_notifier(slot_error_message_notifier& notifier);

/// Decorates the given slot data notifier with logging capabilities.
std::unique_ptr<slot_data_message_notifier> create_logging_slot_data_notifier(slot_data_message_notifier& notifier);

} // namespace fapi
} // namespace srsran
