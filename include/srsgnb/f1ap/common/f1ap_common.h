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

#include "srsgnb/asn1/f1ap/f1ap.h"

namespace srsgnb {

struct f1ap_message {
  asn1::f1ap::f1ap_pdu_c pdu;
};

/// This interface is used to push F1AP messages to the F1 interface.
class f1ap_message_handler
{
public:
  virtual ~f1ap_message_handler() = default;

  /// Handle the incoming F1AP message.
  virtual void handle_message(const f1ap_message& msg) = 0;
};

/// Interface used by F1 to inform about events.
class f1ap_event_handler
{
public:
  virtual ~f1ap_event_handler()         = default;
  virtual void handle_connection_loss() = 0;
};

/// This interface notifies the reception of new F1AP messages over the F1-C interface.
class f1ap_message_notifier
{
public:
  virtual ~f1ap_message_notifier() = default;

  /// This callback is invoked on each received F1AP message.
  virtual void on_new_message(const f1ap_message& msg) = 0;
};

} // namespace srsgnb
