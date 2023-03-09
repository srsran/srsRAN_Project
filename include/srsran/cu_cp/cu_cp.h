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
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/ngap/ngap.h"
#include "srsran/rrc/rrc_ue.h"
#include <string>

namespace srsran {
namespace srs_cu_cp {

/// Interface to handle AMF connections
class cu_cp_ngap_connection_handler
{
public:
  virtual ~cu_cp_ngap_connection_handler() = default;

  /// \brief Handles a AMF connection notification.
  virtual void handle_amf_connection() = 0;

  /// \brief Handles a AMF connection drop notfication.
  virtual void handle_amf_connection_drop() = 0;
};

/// Interface to handle Paging messages
class cu_cp_ngap_paging_handler
{
public:
  virtual ~cu_cp_ngap_paging_handler() = default;

  /// \brief Handles a Paging message notification.
  virtual void handle_paging_message(cu_cp_paging_message& msg) = 0;
};

/// Methods used by CU-CP to initiate NGAP connection procedures.
class cu_cp_ngap_control_notifier
{
public:
  virtual ~cu_cp_ngap_control_notifier() = default;

  /// \brief Notifies the NGAP to initiate a NG Setup Procedure.
  /// \param[in] request The NG Setup Request.
  virtual async_task<ng_setup_response> on_ng_setup_request(const ng_setup_request& request) = 0;
};

/// Interface used to handle DU specific procedures
class cu_cp_du_handler
{
public:
  virtual ~cu_cp_du_handler() = default;

  /// \brief Handles a new DU connection.
  virtual void handle_new_du_connection() = 0;

  /// \brief Handles a remove request. The corresponding DU processor object will be removed.
  /// \param[in] du_index The index of the DU processor object to delete.
  virtual void handle_du_remove_request(const du_index_t du_index) = 0;

  /// \brief Handle a RRC UE creation notification from the DU processor.
  /// \param[in] du_index The index of the DU the UE is connected to.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] rrc_ue_msg_handler The rrc_ue_dl_nas_message_handler of the created RRC UE.
  virtual void handle_rrc_ue_creation(du_index_t du_index, ue_index_t ue_index, rrc_ue_interface* rrc_ue) = 0;
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

  /// \brief Get the F1AP message handler interface of the DU processor object.
  /// \param[in] du_index The index of the DU processor object.
  /// \return The F1AP message handler interface of the DU processor object.
  virtual f1ap_message_handler& get_f1ap_message_handler(const du_index_t du_index) = 0;

  /// \brief Get the F1AP statistics handler interface of the DU processor object.
  /// \param[in] du_index The index of the DU processor object.
  /// \return The F1AP statistics handler interface of the DU processor object.
  virtual f1ap_statistics_handler& get_f1ap_statistics_handler(du_index_t du_index) = 0;
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

class cu_cp_cu_up_interface
{
public:
  virtual ~cu_cp_cu_up_interface() = default;

  /// \brief Get the number of CU-UPs connected to the CU-CP.
  /// \return The number of CU-UPs.
  virtual size_t get_nof_cu_ups() const = 0;

  /// \brief Get the E1AP message handler interface of the CU-UP processor object.
  /// \param[in] cu_up_index The index of the CU-UP processor object.
  /// \return The E1AP message handler interface of the CU-UP processor object.
  virtual e1ap_message_handler& get_e1ap_message_handler(const cu_up_index_t cu_up_index) = 0;
};

class cu_cp_ng_interface
{
public:
  virtual ~cu_cp_ng_interface() = default;

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

class cu_cp_interface : public cu_cp_du_handler,
                        public cu_cp_du_interface,
                        public cu_cp_cu_up_handler,
                        public cu_cp_cu_up_interface,
                        public cu_cp_ng_interface,
                        public cu_cp_ngap_connection_handler,
                        public cu_cp_ngap_paging_handler
{
public:
  virtual ~cu_cp_interface() = default;

  virtual cu_cp_du_handler&              get_cu_cp_du_handler()              = 0;
  virtual cu_cp_du_interface&            get_cu_cp_du_interface()            = 0;
  virtual cu_cp_cu_up_handler&           get_cu_cp_cu_up_handler()           = 0;
  virtual cu_cp_cu_up_interface&         get_cu_cp_cu_up_interface()         = 0;
  virtual cu_cp_ng_interface&            get_cu_cp_ng_interface()            = 0;
  virtual cu_cp_ngap_connection_handler& get_cu_cp_ngap_connection_handler() = 0;
  virtual cu_cp_ngap_paging_handler&     get_cu_cp_ngap_paging_handler()     = 0;

  virtual void start() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
