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

namespace srsgnb {

struct e1_message {
  asn1::e1ap::e1ap_pdu_c pdu;
};

/// \brief CU-CP initiated E1 setup request.
struct cu_cp_e1_setup_request {
  asn1::e1ap::gnb_cu_cp_e1_setup_request_s request;
  unsigned                                 max_setup_retries = 5;
};

/// \brief Response to CU-CP initiated E1 setup request.
struct cu_cp_e1_setup_response {
  asn1::e1ap::gnb_cu_cp_e1_setup_resp_s response;
  asn1::e1ap::gnb_cu_cp_e1_setup_fail_s failure;
  bool                                  success;
};

/// \brief CU-UP initiated E1 setup request.
struct cu_up_e1_setup_request {
  asn1::e1ap::gnb_cu_up_e1_setup_request_s request;
  unsigned                                 max_setup_retries = 5;
};

/// \brief Response to CU-UP initiated E1 setup request.
struct cu_up_e1_setup_response {
  asn1::e1ap::gnb_cu_up_e1_setup_resp_s response;
  asn1::e1ap::gnb_cu_up_e1_setup_fail_s failure;
  bool                                  success;
};

/// This interface is used to push E1 messages to the E1 interface.
class e1_message_handler
{
public:
  virtual ~e1_message_handler() = default;

  /// Handle the incoming E1 message.
  virtual void handle_message(const e1_message& msg) = 0;
};

/// Interface used by E1 to inform about events.
class e1_event_handler
{
public:
  virtual ~e1_event_handler()           = default;
  virtual void handle_connection_loss() = 0;
};

/// This interface notifies the reception of new E1 messages over the E1 interface.
class e1_message_notifier
{
public:
  virtual ~e1_message_notifier() = default;

  /// This callback is invoked on each received E1 message.
  virtual void on_new_message(const e1_message& msg) = 0;
};

} // namespace srsgnb
