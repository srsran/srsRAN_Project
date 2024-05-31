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

#include "buffered_decorator_impl.h"

using namespace srsran;
using namespace fapi;

slot_message_gateway& buffered_decorator_impl::get_slot_message_gateway()
{
  return dispatcher;
}

slot_time_message_notifier& buffered_decorator_impl::get_slot_time_message_notifier()
{
  return time_notifier;
}

void buffered_decorator_impl::set_slot_time_notifier(slot_time_message_notifier& notifier)
{
  time_notifier.set_slot_time_notifier(notifier);
}
