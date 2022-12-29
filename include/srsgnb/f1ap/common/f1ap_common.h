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

#include "srsgnb/asn1/f1ap/f1ap.h"

namespace srsgnb {

struct f1c_message {
  asn1::f1ap::f1ap_pdu_c pdu;
};

/// This interface is used to push F1C messages to the F1 interface.
class f1c_message_handler
{
public:
  virtual ~f1c_message_handler() = default;

  /// Handle the incoming F1C message.
  virtual void handle_message(const f1c_message& msg) = 0;
};

/// Interface used by F1 to inform about events.
class f1c_event_handler
{
public:
  virtual ~f1c_event_handler()          = default;
  virtual void handle_connection_loss() = 0;
};

/// This interface notifies the reception of new F1C messages over the F1-C interface.
class f1c_message_notifier
{
public:
  virtual ~f1c_message_notifier() = default;

  /// This callback is invoked on each received F1C message.
  virtual void on_new_message(const f1c_message& msg) = 0;
};

} // namespace srsgnb
