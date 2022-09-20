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

#include "cu_cp_types.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/ngap/ngc.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

/// Interface to notify about NGC connections to the CU-CP
class cu_cp_ngc_connection_notifier
{
public:
  virtual ~cu_cp_ngc_connection_notifier() = default;

  /// \brief Notifies the CU-CP about a successful AMF connection.
  virtual void on_amf_connection() = 0;

  /// \brief Notifies the CU-CP about a dropped AMF connection.
  virtual void on_amf_connection_drop() = 0;
};

/// Interface to notify about DU connections to the CU-CP
class cu_cp_du_connection_notifier
{
public:
  virtual ~cu_cp_du_connection_notifier() = default;

  /// \brief Notifies the CU-CP about a new DU connection.
  virtual void on_new_connection() = 0;
};

/// Interface used to handle DU specific F1C procedure outcomes (e.g. F1 removal request)
class cu_cp_du_handler
{
public:
  virtual ~cu_cp_du_handler() = default;

  /// \brief Handles a remove request. The corresponding DU processor object will be removed.
  /// \param[in] du_index The index of the DU processor object to delete.
  virtual void handle_du_remove_request(const du_index_t du_index) = 0;
};

class cu_cp_du_interface
{
public:
  virtual ~cu_cp_du_interface() = default;

  /// \brief Get the number of DUs connected to the CU-CP.
  /// \return The number of DUs.
  virtual size_t get_nof_dus() const = 0;

  /// \brief Get the number of UEs connected to the all DUs of the CU-CP.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;

  /// \brief Get the F1C message handler interface of the DU processor object.
  /// \param[in] du_index The index of the DU processor object.
  /// \return The F1C message handler interface of the DU processor object.
  virtual f1c_message_handler& get_f1c_message_handler(const du_index_t du_index) = 0;

  /// \brief Get the F1C statistics handler interface of the DU processor object.
  /// \param[in] du_index The index of the DU processor object.
  /// \return The F1C statistics handler interface of the DU processor object.
  virtual f1c_statistics_handler& get_f1c_statistics_handler(du_index_t du_index) = 0;
};

class cu_cp_ng_interface
{
public:
  virtual ~cu_cp_ng_interface() = default;

  /// \brief Get the NG message handler interface.
  /// \return The NG message handler interface.
  virtual ngc_message_handler& get_ngc_message_handler() = 0;

  /// \brief Get the state of the AMF connection.
  /// \return True if AMF is connected, false otherwise.
  virtual bool amf_is_connected() = 0;
};

class cu_cp_interface : public cu_cp_du_connection_notifier,
                        public cu_cp_du_handler,
                        public cu_cp_du_interface,
                        public cu_cp_ng_interface,
                        public cu_cp_ngc_connection_notifier
{
public:
  virtual ~cu_cp_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsgnb
