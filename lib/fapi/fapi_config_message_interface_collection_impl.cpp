/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "fapi_config_message_interface_collection_impl.h"

using namespace srsran;
using namespace fapi;

config_message_gateway& fapi_config_message_interface_collection_impl::get_config_message_gateway()
{
  return gateway;
}

void fapi_config_message_interface_collection_impl::set_config_message_notifier(
    config_message_notifier& config_notifier)
{
  gateway.set_config_message_notifier(config_notifier);
}

void fapi_config_message_interface_collection_impl::set_slot_error_message_notifier(
    slot_error_message_notifier& err_notifier)
{
  gateway.set_slot_error_message_notifier(err_notifier);
}

void fapi_config_message_interface_collection_impl::set_cell_operation_request_notifier(
    cell_operation_request_notifier& cell_notifier)
{
  gateway.set_cell_operation_request_notifier(cell_notifier);
}
