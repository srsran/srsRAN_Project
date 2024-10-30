/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "e2_event_manager.h"
#include "e2_messages.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/expected.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2sm/e2sm_common_ies.h"
#include "srsran/asn1/e2sm/e2sm_kpm_ies.h"
#include "srsran/ran/rb_id.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/async/eager_async_task.h"

namespace srsran {

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

  /// \brief Request a new TNL association to the Near-RT RIC.
  virtual bool handle_e2_tnl_connection_request() = 0;

  /// \brief Request the E2AP handler to disconnect from the Near-RT RIC.
  virtual async_task<void> handle_e2_disconnection_request() = 0;

  /// \brief Initiates the E2 Setup procedure as per _____
  /// \param[in] request The E2SetupRequest message to transmit.
  /// \return Returns a e2_setup_response_message struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise.
  /// and awaits the response. If a E2SetupFailure is received the E2 will handle the failure.
  virtual async_task<e2_setup_response_message> handle_e2_setup_request(e2_setup_request_message& request) = 0;

  /// \brief Initiates the E2 Setup procedure as per _____. Setup Request generated from cfg.
  /// \return Returns a e2_setup_response_message struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise.
  /// and awaits the response. If a E2SetupFailure is received the E2 will handle the failure.
  virtual async_task<e2_setup_response_message> start_initial_e2_setup_routine() = 0;
};

/// This interface is used to pack outgoing and unpack incoming E2 messages.
class e2ap_packer : public e2_message_handler
{
public:
  virtual ~e2ap_packer() = default;

  /// Handle packed E2AP PDU that needs to be unpacked and forwarded.
  virtual void handle_packed_pdu(const byte_buffer& pdu) = 0;
};

/// Combined entry point for E2 handling.
class e2_interface : public e2_message_handler, public e2_event_handler, public e2_connection_manager
{
public:
  virtual ~e2_interface() = default;
  virtual void start()    = 0;
  virtual void stop()     = 0;
};

/// E2AP notifier to the E2 Agent.
class e2ap_e2agent_notifier
{
public:
  virtual ~e2ap_e2agent_notifier() = default;

  /// \brief Notify that the TNL connection to the RIC was lost.
  virtual void on_e2_disconnection() = 0;
};

/// \brief Public interface for a E2 Agent instance.
class e2_agent : public e2ap_e2agent_notifier
{
public:
  virtual ~e2_agent() = default;

  /// \brief Get handler of the E2 interface of the E2 Agent.
  virtual e2_interface& get_e2_interface() = 0;

  /// \brief Initiate RIC TNL connection and run E2 Setup Procedure.
  ///
  /// This function blocks until the procedure is complete. Once completed, the E2 Agent is in operational state.
  /// \return Returns true if the connection to the RIC and E2 setup procedure were successful. False, otherwise.
  virtual void start() = 0;

  /// \brief Stop the E2 Agent operation.
  virtual void stop() = 0;
};

} // namespace srsran
