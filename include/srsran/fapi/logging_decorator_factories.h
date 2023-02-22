/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi/slot_time_message_notifier.h"
#include <memory>

namespace srsran {
namespace fapi {

/// Factory that builds logging slot gateway decorators.
class logging_slot_gateway_decorator_factory
{
public:
  std::unique_ptr<slot_message_gateway> create(slot_message_gateway& gateway);
};

/// Factory that builds logging slot data notifier decorators.
class logging_slot_data_notifier_decorator_factory
{
public:
  std::unique_ptr<slot_data_message_notifier> create(slot_data_message_notifier& notifier);
};

/// Factory that builds logging slot time notifier decorators.
class logging_slot_time_notifier_decorator_factory
{
public:
  std::unique_ptr<slot_time_message_notifier> create(slot_time_message_notifier& notifier);
};

} // namespace fapi
} // namespace srsran