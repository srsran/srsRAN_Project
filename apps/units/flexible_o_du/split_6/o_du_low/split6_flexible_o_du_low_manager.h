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

#include "cell_operation_request_impl.h"
#include "split6_cell_configurator_plugin.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/fapi/fapi_config_message_interface_collection.h"
#include <memory>

namespace srsran {

/// \brief Split 6 flexible O-DU low manager.
///
/// This is the class returned by the application unit when it creates the O-DU low.
/// This object:
///  - Manages/owns the configuration plugin and the FAPI configuration related objects.
///  - Controls the cell creation/destruction using the cell operation request handler implementation.
class split6_flexible_o_du_low_manager : public du_operation_controller
{
public:
  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  cell_operation_request_handler_impl                             cell_controller;
  std::unique_ptr<split6_cell_configurator_plugin>                cell_plugin;
  std::unique_ptr<fapi::fapi_config_message_interface_collection> config_interface_collection;
}

} // namespace srsran
