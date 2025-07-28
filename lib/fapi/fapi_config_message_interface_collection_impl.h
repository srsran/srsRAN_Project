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

#pragma once

#include "configuration_procedure.h"
#include "srsran/fapi/fapi_config_message_interface_collection.h"

namespace srsran {
namespace fapi {

/// FAPI configuration message interface collection implementation.
class fapi_config_message_interface_collection_impl : public fapi_config_message_interface_collection
{
  configuration_procedure gateway;

public:
  explicit fapi_config_message_interface_collection_impl(srslog::basic_logger& logger) : gateway(logger) {}

  // See interface for documentation.
  config_message_gateway& get_config_message_gateway() override;

  // See interface for documentation.
  void set_config_message_notifier(config_message_notifier& config_notifier) override;

  // See interface for documentation.
  void set_error_message_notifier(error_message_notifier& err_notifier) override;

  // See interface for documentation.
  void set_cell_operation_request_notifier(cell_operation_request_notifier& cell_notifier) override;
};

} // namespace fapi
} // namespace srsran
