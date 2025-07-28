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

#include <memory>

namespace srsran {
namespace fapi {

class cell_operation_request_notifier;
class config_message_gateway;
class config_message_notifier;
class error_message_notifier;

/// \brief FAPI configuration message interface collection interface.
///
/// This interface gives access to the FAPI configuration message interfaces.
class fapi_config_message_interface_collection
{
public:
  virtual ~fapi_config_message_interface_collection() = default;

  /// Returns the config message gateway of this FAPI cell configurator.
  virtual config_message_gateway& get_config_message_gateway() = 0;

  /// Sets the config message notifier of this FAPI cell configurator.
  virtual void set_config_message_notifier(config_message_notifier& config_notifier) = 0;

  /// Sets the error message notifier of this FAPI cell configurator.
  virtual void set_error_message_notifier(error_message_notifier& err_notifier) = 0;

  /// Sets the cell operation request notifier of this FAPI cell configurator.
  virtual void set_cell_operation_request_notifier(cell_operation_request_notifier& cell_notifier) = 0;
};

/// Creates a FAPI configuration message interface collection.
std::unique_ptr<fapi_config_message_interface_collection>
create_fapi_config_message_interface_collection(srslog::basic_logger& logger);

} // namespace fapi
} // namespace srsran
