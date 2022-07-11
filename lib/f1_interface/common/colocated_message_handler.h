/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_F1_INTERFACE_COMMON_COLOCATED_MESSAGE_HANDLER_H
#define SRSGNB_LIB_F1_INTERFACE_COMMON_COLOCATED_MESSAGE_HANDLER_H

#include "srsgnb/f1_interface/du/f1ap_du.h"

namespace srsgnb {

/// This implementation forwards F1AP messages through a function call to the peer F1 interface. It is useful for
/// co-located scenarios where both F1 interfaces reside in the same process.
class colocated_f1c_pdu_handler : public f1c_message_handler
{
  f1c_message_handler& peer_handler;

public:
  explicit colocated_f1c_pdu_handler(f1c_message_handler& peer_handler) : peer_handler(peer_handler) {}

  void handle_message(const asn1::f1ap::f1_ap_pdu_c& msg) override { peer_handler.handle_message(msg); }
};

} // namespace srsgnb

#endif // SRSGNB_LIB_F1_INTERFACE_COMMON_COLOCATED_MESSAGE_HANDLER_H
