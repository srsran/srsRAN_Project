/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
