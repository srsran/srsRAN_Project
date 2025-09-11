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

#include "apps/services/remote_control/remote_command.h"
#include "srsran/adt/span.h"
#include <memory>

namespace srsran {

struct remote_control_appconfig;

namespace app_services {

/// \brief Remote server interface.
///
/// WebSocket server that listens for external incoming requests.
class remote_server
{
public:
  /// Default destructor.
  virtual ~remote_server() = default;

  /// Stops listening new requests.
  virtual void stop() = 0;
};

/// Creates a Remote Server instance with a give list of commands.
std::unique_ptr<remote_server> create_remote_server(const remote_control_appconfig&       cfg,
                                                    span<std::unique_ptr<remote_command>> commands);

/// Initializes the JSON log channel.
void initialize_json_channel();

} // namespace app_services
} // namespace srsran
