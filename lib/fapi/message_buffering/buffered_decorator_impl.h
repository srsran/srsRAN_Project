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

#include "buffered_slot_gateway_task_dispatcher.h"
#include "buffered_slot_time_notifier_decorator.h"
#include "srsran/fapi/buffered_decorator.h"

namespace srsran {
namespace fapi {

class buffered_decorator_impl : public buffered_decorator
{
public:
  buffered_decorator_impl(unsigned              l2_nof_slots_ahead,
                          subcarrier_spacing    scs,
                          slot_message_gateway& gateway,
                          task_executor&        executor) :
    dispatcher(l2_nof_slots_ahead, scs, gateway, executor), time_notifier(l2_nof_slots_ahead, scs, dispatcher)
  {
  }

  // See interface for documentation.
  slot_message_gateway& get_slot_message_gateway() override;

  // See interface for documentation.
  slot_time_message_notifier& get_slot_time_message_notifier() override;

  // See interface for documentation.
  void set_slot_time_notifier(slot_time_message_notifier& notifier) override;

private:
  buffered_slot_gateway_task_dispatcher dispatcher;
  buffered_slot_time_notifier_decorator time_notifier;
};

} // namespace fapi
} // namespace srsran
