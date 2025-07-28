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

void fapi_config_message_interface_collection_impl::set_error_message_notifier(error_message_notifier& err_notifier)
{
  gateway.set_error_message_notifier(err_notifier);
}

void fapi_config_message_interface_collection_impl::set_cell_operation_request_notifier(
    cell_operation_request_notifier& cell_notifier)
{
  gateway.set_cell_operation_request_notifier(cell_notifier);
}

std::unique_ptr<fapi_config_message_interface_collection>
srsran::fapi::create_fapi_config_message_interface_collection(srslog::basic_logger& logger)
{
  return std::make_unique<fapi_config_message_interface_collection_impl>(logger);
}
