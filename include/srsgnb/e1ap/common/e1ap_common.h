/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/asn1/e1ap/e1ap.h"

namespace srsran {

struct e1ap_message {
  asn1::e1ap::e1ap_pdu_c pdu;
};

/// This interface is used to push E1AP messages to the E1AP interface.
class e1ap_message_handler
{
public:
  virtual ~e1ap_message_handler() = default;

  /// Handle the incoming E1AP message.
  virtual void handle_message(const e1ap_message& msg) = 0;
};

/// Interface used by E1AP to inform about events.
class e1ap_event_handler
{
public:
  virtual ~e1ap_event_handler()         = default;
  virtual void handle_connection_loss() = 0;
};

/// This interface notifies the reception of new E1AP messages over the E1AP interface.
class e1ap_message_notifier
{
public:
  virtual ~e1ap_message_notifier() = default;

  /// This callback is invoked on each received E1AP message.
  virtual void on_new_message(const e1ap_message& msg) = 0;
};

} // namespace srsran
