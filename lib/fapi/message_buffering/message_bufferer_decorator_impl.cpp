/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

slot_error_message_notifier& message_bufferer_decorator_impl::get_slot_error_message_notifier_from_this_decorator()
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

void message_bufferer_decorator_impl::set_slot_error_message_notifier(slot_error_message_notifier& notifier)
{
  error_notifier.set_slot_error_message_notifier(notifier);
}
