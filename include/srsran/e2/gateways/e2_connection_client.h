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

#include "srsran/e2/e2.h"
#include <memory>

namespace srsran {

/// Interface used by the E2 agent to establish a new connection with a ORAN Near Real-Time RIC.
class e2_connection_client
{
public:
  virtual ~e2_connection_client() = default;

  /// Establish a new connection with a Near Real-Time RIC.
  ///
  /// \param e2ap_rx_pdu_notifier Notifier that will be used to forward the E2AP PDUs sent by the RIC to the E2 Agent.
  /// \return Notifier that the E2 Agent can use to send E2AP Tx PDUs to the RIC it connected to.
  virtual std::unique_ptr<e2_message_notifier>
  handle_e2_connection_request(std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier) = 0;
};

} // namespace srsran
