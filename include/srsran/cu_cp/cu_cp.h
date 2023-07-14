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

#include "cu_cp_types.h"
#include "srsran/cu_cp/du_repository.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/ngap/ngap.h"
#include "srsran/rrc/rrc_ue.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

/// Interface to handle AMF connections
class cu_cp_ngap_handler
{
public:
  virtual ~cu_cp_ngap_handler() = default;

  /// \brief Handles a AMF connection notification.
  virtual void handle_amf_connection() = 0;

  /// \brief Handles a AMF connection drop notfication.
  virtual void handle_amf_connection_drop() = 0;
};

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

/// Interface used to handle CU-UP specific procedures
class cu_cp_cu_up_handler
{
public:
  virtual ~cu_cp_cu_up_handler() = default;

  /// \brief Handles a new CU-UP connection.
  virtual void handle_new_cu_up_connection() = 0;

  /// \brief Handles a remove request. The corresponding CU-UP processor object will be removed.
  /// \param[in] cu_up_index The index of the CU-UP processor object to delete.
  virtual void handle_cu_up_remove_request(const cu_up_index_t cu_up_index) = 0;
};

class cu_cp_cu_up_connection_interface
{
public:
  virtual ~cu_cp_cu_up_connection_interface() = default;

  /// \brief Get the number of CU-UPs connected to the CU-CP.
  /// \return The number of CU-UPs.
  virtual size_t get_nof_cu_ups() const = 0;

  /// \brief Get the E1AP message handler interface of the CU-UP processor object.
  /// \param[in] cu_up_index The index of the CU-UP processor object.
  /// \return The E1AP message handler interface of the CU-UP processor object.
  virtual e1ap_message_handler& get_e1ap_message_handler(const cu_up_index_t cu_up_index) = 0;
};

class cu_cp_interface : public cu_cp_ngap_connection_interface,
                        public cu_cp_ngap_handler,
                        public cu_cp_cu_up_handler,
                        public cu_cp_cu_up_connection_interface

{
public:
  virtual ~cu_cp_interface() = default;

  virtual du_repository&                    get_connected_dus()                    = 0;
  virtual cu_cp_ngap_handler&               get_cu_cp_ngap_handler()               = 0;
  virtual cu_cp_ngap_connection_interface&  get_cu_cp_ngap_connection_interface()  = 0;
  virtual cu_cp_cu_up_handler&              get_cu_cp_cu_up_handler()              = 0;
  virtual cu_cp_cu_up_connection_interface& get_cu_cp_cu_up_connection_interface() = 0;

  virtual void start() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
