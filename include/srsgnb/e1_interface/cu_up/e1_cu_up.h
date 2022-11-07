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

#include "srsgnb/asn1/e1ap.h"
#include "srsgnb/e1_interface/common/e1_common.h"

namespace srsgnb {
namespace srs_cu_up {

/// Handle E1 interface management procedures as defined in TS 38.463 section 8.2.4.
class e1_connection_manager
{
public:
  virtual ~e1_connection_manager() = default;

  /// \brief Creates and transmits the CU-CP initiated E1 Setup outcome to the CU-CP.
  virtual void handle_cu_cp_e1_setup_response(const cu_cp_e1_setup_response_message& msg) = 0;
};

/// Methods used by E1 to notify the CU-CP (manager).
class e1_cu_cp_notifier
{
public:
  virtual ~e1_cu_cp_notifier() = default;

  /// \brief Notifies about the reception of a E1 Setup Request message.
  virtual void on_e1_setup_request_received(const cu_cp_e1_setup_request_message& msg) = 0;
};

/// Combined entry point for E1 handling.
class e1_interface : public e1_message_handler, public e1_event_handler, public e1_connection_manager
{
public:
  virtual ~e1_interface() = default;
};

} // namespace srs_cu_up
} // namespace srsgnb
