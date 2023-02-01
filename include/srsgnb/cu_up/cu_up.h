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

#include "srsgnb/e1/common/e1_common.h"
#include "srsgnb/e1/cu_up/e1ap_cu_up.h"
#include "srsgnb/gtpu/gtpu_demux.h"

#include <string>

namespace srsgnb {
namespace srs_cu_up {

/// Interface to notify about E1 connections (from the CU-CP) to the CU-UP
class cu_up_e1_connection_notifier
{
public:
  virtual ~cu_up_e1_connection_notifier() = default;

  /// \brief Notifies the CU-UP about a successful E1 connection.
  virtual void on_e1_connection_establish() = 0;

  /// \brief Notifies the CU-CP about a dropped E1 connection.
  virtual void on_e1_connection_drop() = 0;
};

class cu_up_e1_interface
{
public:
  virtual ~cu_up_e1_interface() = default;

  /// \brief Handle E1 Setup Request received from the CU-CP.
  /// \param[in] msg The CU-CP E1 Setup Request.
  /// \return The CU-CP E1 Setup Response.
  virtual cu_cp_e1_setup_response handle_cu_cp_e1_setup_request(const cu_cp_e1_setup_request& msg) = 0;

  /// \brief Create a new UE context and handle bearer setup request.
  /// \param[in] msg The original bearer setup request.
  /// \return Returns message containing the index of the created UE and all response/failure message.
  virtual e1ap_bearer_context_setup_response
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) = 0;

  /// \brief Create a new UE context and handle bearer modification request.
  /// \param[in] msg The original bearer modification request.
  /// \return Returns message containing the index of the created UE and all response/failure message.
  virtual e1ap_bearer_context_modification_response
  handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) = 0;

  /// \brief Handle bearer release command and remove the associated UE context.
  /// \param[in] msg The original bearer release command.
  virtual void handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg) = 0;

  /// \brief Get the E1 message handler interface.
  virtual e1_message_handler& get_e1_message_handler() = 0;

  /// \brief Get the state of the E1 connection.
  /// \return True if E1 is connected, false otherwise.
  virtual bool e1_is_connected() = 0;
};

/// Interface to notify about GTP-U packets (from the NGU) to the CU-UP
class cu_up_ngu_interface
{
public:
  virtual ~cu_up_ngu_interface() = default;

  /// \brief Get the NGu PDU handler interface.
  /// \return The NGu PDU handler interface.
  virtual gtpu_demux_rx_upper_layer_interface& get_ngu_pdu_handler() = 0;
};

class cu_up_interface : public cu_up_e1_connection_notifier, public cu_up_e1_interface, public cu_up_ngu_interface
{
public:
  virtual ~cu_up_interface() = default;

  /// \brief Get the N3 bind port.
  virtual int get_n3_bind_port() = 0;
};

} // namespace srs_cu_up
} // namespace srsgnb
