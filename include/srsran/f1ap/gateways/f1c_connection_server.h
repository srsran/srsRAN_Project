/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/optional.h"

namespace srsran {
namespace srs_cu_cp {

class cu_cp_f1c_handler;

/// Connection server responsible for handling new connection requests/drops coming from DUs via the F1-C interface
/// and converting them CU-CP commands.
class f1c_connection_server
{
public:
  virtual ~f1c_connection_server() = default;

  /// Attach a CU-CP handler to the F1-C connection server.
  virtual void attach_cu_cp(cu_cp_f1c_handler& cu_f1c_handler_) = 0;

  /// Get port on which the F1-C Server is listening for new connections.
  ///
  /// This method is useful in testing, where we don't want to use a specific port.
  /// \return The port number on which the F1-C Server is listening for new connections.
  virtual std::optional<uint16_t> get_listen_port() const = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
