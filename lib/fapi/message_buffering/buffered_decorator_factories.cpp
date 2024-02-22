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
