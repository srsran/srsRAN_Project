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

#include "cu_cp_metrics_handler.h"
#include "cu_cp_types.h"
#include "srsran/cu_cp/cu_up_repository.h"
#include "srsran/cu_cp/du_repository.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/ngap/ngap.h"

namespace srsran {
namespace srs_cu_cp {

class cu_cp_ngap_connection_interface
{
public:
  virtual ~cu_cp_ngap_connection_interface() = default;

  /// \brief Get the NG message handler interface.
  /// \return The NG message handler interface.
  virtual ngap_message_handler& get_ngap_message_handler() = 0;

  /// \brief Get the NG event handler interface.
  /// \return The NG event handler interface.
  virtual ngap_event_handler& get_ngap_event_handler() = 0;

  /// \brief Get the state of the AMF connection.
  /// \return True if AMF is connected, false otherwise.
  virtual bool amf_is_connected() = 0;
};

class cu_cp_cu_up_connection_interface
{
public:
  virtual ~cu_cp_cu_up_connection_interface() = default;

  /// \brief Get the E1AP message handler interface of the CU-UP processor object.
  /// \param[in] cu_up_index The index of the CU-UP processor object.
  /// \return The E1AP message handler interface of the CU-UP processor object.
  virtual e1ap_message_handler& get_e1ap_message_handler(const cu_up_index_t cu_up_index) = 0;
};

class cu_cp_interface
{
public:
  virtual ~cu_cp_interface() = default;

  /// \brief Get repository of handlers for the DUs currently connected to the CU-CP.
  virtual du_repository& get_connected_dus() = 0;

  /// \brief Get repository of handlers for the CU-UPs currently connected to the CU-CP.
  virtual cu_up_repository& get_connected_cu_ups() = 0;

  /// \brief Get handler of the NG interface of the CU-CP.
  virtual cu_cp_ngap_connection_interface& get_cu_cp_ngap_connection_interface() = 0;

  /// \brief Get handler of the E1 interface of the CU-CP.
  virtual cu_cp_cu_up_connection_interface& get_cu_cp_cu_up_connection_interface() = 0;

  /// \brief Get the metrics handler interface of the CU-CP.
  virtual metrics_handler& get_metrics_handler() = 0;

  /// \brief Initiate NG Setup Procedure and wait for AMF response.
  /// \return Returns true if the connection to the AMF was successful. False, otherwise.
  virtual bool start() = 0;

  /// \brief Stop the CU-CP operation.
  virtual void stop() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
