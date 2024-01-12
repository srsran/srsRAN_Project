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
#include "buffered_slot_gateway_task_dispatcher.h"
#include "buffered_slot_time_notifier_decorator.h"

using namespace srsran;
using namespace fapi;

std::unique_ptr<buffered_decorator_modules>
srsran::fapi::create_buffered_decorator_modules(unsigned int                l2_nof_slots_ahead,
                                                subcarrier_spacing          scs,
                                                slot_message_gateway&       gateway,
                                                task_executor&              executor,
                                                slot_time_message_notifier& delayed_notifier)
{
  auto modules = std::make_unique<buffered_decorator_modules>();

  auto slot_gateway =
      std::make_unique<buffered_slot_gateway_task_dispatcher>(l2_nof_slots_ahead, scs, gateway, executor);
  modules->notifier =
      std::make_unique<buffered_slot_time_notifier_decorator>(l2_nof_slots_ahead, scs, *slot_gateway, delayed_notifier);
  modules->gateway = std::move(slot_gateway);

  return modules;
}
