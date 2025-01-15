/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "logging_decorator_impl.h"

using namespace srsran;
using namespace fapi;

logging_decorator_impl::logging_decorator_impl(srslog::basic_logger&       logger,
                                               slot_message_gateway&       gateway_,
                                               slot_last_message_notifier& last_msg_notifier_) :
  fapi_decorator({}), data_notifier(logger), error_notifier(logger), time_notifier(logger), gateway(logger, gateway_)
{
  last_msg_notifier.set_slot_last_message_notifier(last_msg_notifier_);
}

logging_decorator_impl::logging_decorator_impl(srslog::basic_logger&           logger,
                                               std::unique_ptr<fapi_decorator> next_decorator_) :
  fapi_decorator({std::move(next_decorator_)}),
  data_notifier(logger),
  error_notifier(logger),
  time_notifier(logger),
  gateway(logger, next_decorator->get_slot_message_gateway())
{
  last_msg_notifier.set_slot_last_message_notifier(next_decorator->get_slot_last_message_notifier());
  connect_notifiers();
}

slot_data_message_notifier& logging_decorator_impl::get_slot_data_message_notifier_from_this_decorator()
{
  return data_notifier;
}

slot_last_message_notifier& logging_decorator_impl::get_slot_last_message_notifier()
{
  return last_msg_notifier;
}

slot_message_gateway& logging_decorator_impl::get_slot_message_gateway()
{
  return gateway;
}

slot_error_message_notifier& logging_decorator_impl::get_slot_error_message_notifier_from_this_decorator()
{
  return error_notifier;
}

slot_time_message_notifier& logging_decorator_impl::get_slot_time_message_notifier_from_this_decorator()
{
  return time_notifier;
}

void logging_decorator_impl::set_slot_data_message_notifier(slot_data_message_notifier& notifier)
{
  data_notifier.set_slot_data_message_notifier(notifier);
}

void logging_decorator_impl::set_slot_error_message_notifier(slot_error_message_notifier& notifier)
{
  error_notifier.set_slot_error_message_notifier(notifier);
}

void logging_decorator_impl::set_slot_time_message_notifier(slot_time_message_notifier& notifier)
{
  time_notifier.set_slot_time_message_notifier(notifier);
}
