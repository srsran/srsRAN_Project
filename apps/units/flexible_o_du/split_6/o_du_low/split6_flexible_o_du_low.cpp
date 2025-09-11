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

#include "split6_flexible_o_du_low.h"
#include "srsran/fapi_adaptor/fapi_operation_controller.h"

using namespace srsran;

split6_flexible_o_du_low::split6_flexible_o_du_low(split6_flexible_o_du_low_dependencies&& dependencies) :
  odu_low_session_manager(std::move(dependencies.odu_low_session_factory)),
  config_msg_adaptor(std::move(dependencies.config_adaptor)),
  config_interface_collection(std::move(dependencies.config_interface_collection))
{
  srsran_assert(config_msg_adaptor, "Invalid FAPI configuration message adaptor");
  srsran_assert(config_interface_collection, "Invalid FAPI configuration message interface collection");

  config_interface_collection->set_cell_operation_request_notifier(odu_low_session_manager);
  config_interface_collection->set_error_message_notifier(config_msg_adaptor->get_error_message_notifier());
  config_interface_collection->set_config_message_notifier(config_msg_adaptor->get_config_message_notifier());
}

void split6_flexible_o_du_low::start()
{
  config_msg_adaptor->get_operation_controller().start();
}

void split6_flexible_o_du_low::stop()
{
  config_msg_adaptor->get_operation_controller().stop();
  // Clear running session.
  odu_low_session_manager.on_stop_request();
}
