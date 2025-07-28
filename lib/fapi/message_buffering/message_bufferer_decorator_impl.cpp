/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "message_bufferer_decorator_impl.h"

using namespace srsran;
using namespace fapi;

slot_message_gateway& message_bufferer_decorator_impl::get_slot_message_gateway()
{
  return dispatcher;
}

slot_time_message_notifier& message_bufferer_decorator_impl::get_slot_time_message_notifier_from_this_decorator()
{
  return time_notifier;
}

slot_data_message_notifier& message_bufferer_decorator_impl::get_slot_data_message_notifier_from_this_decorator()
{
  return data_notifier;
}

slot_last_message_notifier& message_bufferer_decorator_impl::get_slot_last_message_notifier()
{
  return last_msg_notifier;
}

error_message_notifier& message_bufferer_decorator_impl::get_error_message_notifier_from_this_decorator()
{
  return error_notifier;
}

void message_bufferer_decorator_impl::set_slot_time_message_notifier(slot_time_message_notifier& notifier)
{
  time_notifier.set_slot_time_notifier(notifier);
}

void message_bufferer_decorator_impl::set_slot_data_message_notifier(slot_data_message_notifier& notifier)
{
  data_notifier.set_slot_data_message_notifier(notifier);
}

void message_bufferer_decorator_impl::set_error_message_notifier(error_message_notifier& notifier)
{
  error_notifier.set_error_message_notifier(notifier);
}
