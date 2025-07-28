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

#include "split6_flexible_o_du_low_session_manager.h"
#include "srsran/du/du_operation_controller.h"
#include "srsran/fapi/fapi_config_message_interface_collection.h"
#include "srsran/fapi_adaptor/fapi_config_messages_adaptor.h"
#include <memory>

namespace srsran {

/// Split 6 flexible O-DU low dependencies.
struct split6_flexible_o_du_low_dependencies {
  std::unique_ptr<split6_flexible_o_du_low_session_factory>       odu_low_session_factory;
  std::unique_ptr<fapi::config_messages_adaptor>                  config_adaptor;
  std::unique_ptr<fapi::fapi_config_message_interface_collection> config_interface_collection;
};

/// \brief Split 6 flexible O-DU low.
///
/// This is the class returned by the application unit when it creates the O-DU low.
/// This object:
///  - Manages/owns the configuration adaptor and the FAPI configuration related objects.
///  - Controls the cell creation/destruction using the cell operation request handler implementation.
class split6_flexible_o_du_low : public du_operation_controller
{
public:
  explicit split6_flexible_o_du_low(split6_flexible_o_du_low_dependencies&& dependencies);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

private:
  split6_flexible_o_du_low_session_manager                        odu_low_session_manager;
  std::unique_ptr<fapi::config_messages_adaptor>                  config_msg_adaptor;
  std::unique_ptr<fapi::fapi_config_message_interface_collection> config_interface_collection;
};

} // namespace srsran
