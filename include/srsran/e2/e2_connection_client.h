/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/e2/e2.h"

namespace srsran {

/// Interface used by the E2 Agent to establish a new connection with a RIC.
class e2_connection_client
{
public:
  virtual ~e2_connection_client() = default;

  /// Establish a new connection with a RIC.
  ///
  /// \return Notifier that the E2 Agent can use to send E2AP Tx PDUs to the RIC it connected to.
  virtual std::unique_ptr<e2_message_notifier> handle_connection_request() = 0;

  /// Connect E2AP adapter to SCTP network gateway.
  virtual void connect_e2ap(std::unique_ptr<e2_message_notifier> e2_rx_pdu_notifier,
                            e2_message_handler*                  e2ap_msg_handler_,
                            e2_event_handler*                    event_handler_) = 0;

  /// Close all established connections with a RIC.
  virtual void close() = 0;
};

} // namespace srsran