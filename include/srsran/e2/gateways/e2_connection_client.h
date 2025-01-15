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