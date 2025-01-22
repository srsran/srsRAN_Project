/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "apps/services/remote_control/remote_command.h"
#include "srsran/adt/span.h"
#include <memory>

namespace srsran {
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
std::unique_ptr<remote_server> create_remote_server(unsigned port, span<std::unique_ptr<remote_command>> commands);

} // namespace app_services
} // namespace srsran
