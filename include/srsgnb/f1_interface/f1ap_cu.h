/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1AP_CU_H
#define SRSGNB_F1AP_CU_H

#include "f1c_common.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

struct f1_cu_setup_response_message {
  asn1::f1ap::f1_setup_resp_s response;
  asn1::f1ap::f1_setup_fail_s failure;
  bool                        success;
};

struct f1_cu_setup_request_message {
  asn1::f1ap::f1_setup_request_s request;
};

/// Handle F1AP interface management procedures as defined in TS 38.473 section 8.2.
class f1ap_cu_du_connection_manager
{
public:
  virtual ~f1ap_cu_du_connection_manager() = default;

  /// \brief Creates and transmits the F1 Setup outcome to the DU.
  /// \param[in] msg The f1_cu_setup_response_message to transmit.
  /// \remark The CU transmits the F1SetupResponse/F1SetupFailure as per TS 38.473 section 8.2.3.
  virtual void handle_f1ap_setup_response(const f1_cu_setup_response_message& msg) = 0;
};

/// Methods used by F1AP to notify events.
class f1ap_cu_message_notifier
{
public:
  virtual ~f1ap_cu_message_notifier()                                               = default;
  virtual void on_f1_setup_request_received(const f1_cu_setup_request_message& msg) = 0;
};

/// Combined entry point for F1C/U handling.
class f1_cu_interface : public f1c_message_handler, public f1c_event_handler, public f1ap_cu_du_connection_manager
{
public:
  virtual ~f1_cu_interface() = default;
};

} // namespace srsgnb

#endif // SRSGNB_F1AP_CU_H
