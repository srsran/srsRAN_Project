/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/fapi/slot_data_message_notifier.h"
#include "srsgnb/fapi/slot_message_gateway.h"
#include "srsgnb/fapi/slot_time_message_notifier.h"
#include <memory>

namespace srsgnb {
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
} // namespace srsgnb