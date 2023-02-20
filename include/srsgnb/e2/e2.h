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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/e2ap/e2ap.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/support/async/async_task.h"
namespace srsgnb {

struct e2_message {
  asn1::e2ap::e2_ap_pdu_c pdu;
};

/// \brief CU initiated E2 setup request.
struct e2_setup_request_message {
  asn1::e2ap::e2setup_request_s request;
  unsigned                      max_setup_retries = 5;
};

/// \brief Response to CU initiated E2 setup request.
struct e2_setup_response_message {
  asn1::e2ap::e2setup_resp_s response;
  asn1::e2ap::e2setup_fail_s failure;
  bool                       success;
};

/// This interface is used to push E2 messages to the E2 interface.
class e2_message_handler
{
public:
  virtual ~e2_message_handler() = default;

  /// Handle the incoming E2 message.
  virtual void handle_message(const e2_message& msg) = 0;
};

/// Interface used by E2 to inform about events.
class e2_event_handler
{
public:
  virtual ~e2_event_handler()           = default;
  virtual void handle_connection_loss() = 0;
};

/// This interface notifies the reception of new E2 messages over the E2 interface.
class e2_message_notifier
{
public:
  virtual ~e2_message_notifier() = default;

  /// This callback is invoked on each received E2 message.
  virtual void on_new_message(const e2_message& msg) = 0;
};

class e2_connection_manager
{
public:
  virtual ~e2_connection_manager() = default;

  /// \brief Creates and transmits the E2 Setup outcome to the CU-UP.
  /// \param[in] msg The cu_up_e1_setup_response_message to transmit.
  /// \remark The CU transmits the E2SetupResponse/E2SetupFailure as per______.
  virtual void handle_e2_setup_response(const e2_setup_response_message& msg) = 0;

  /// \brief Initiates the E2 Setup procedure as per _____
  /// \param[in] request The E1SetupRequest message to transmit.
  /// \return Returns a e2_setup_response_message struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise.
  /// \remark The CU transmits the E2SetupRequest as per ______.
  /// and awaits the response. If a E2SetupFailure is received the E2 will handle the failure.
  virtual async_task<e2_setup_response_message> handle_e2_setup_request(const e2_setup_request_message& request) = 0;
};

/// Combined entry point for E2 handling.
class e2_interface : public e2_message_handler, public e2_event_handler, public e2_connection_manager
{
public:
  virtual ~e2_interface() = default;
};
} // namespace srsgnb
