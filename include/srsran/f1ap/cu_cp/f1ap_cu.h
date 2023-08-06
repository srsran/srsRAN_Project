/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../common/f1ap_types.h"
#include "f1ap_cu_ue_context_update.h"
#include "f1ap_interface_management_types.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/ran/lcid.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

struct f1ap_ul_rrc_message {
  ue_index_t                        ue_index = ue_index_t::invalid;
  asn1::f1ap::ul_rrc_msg_transfer_s msg;
};

struct f1ap_dl_rrc_message {
  ue_index_t  ue_index     = ue_index_t::invalid;
  ue_index_t  old_ue_index = ue_index_t::invalid;
  srb_id_t    srb_id       = srb_id_t::nulltype;
  byte_buffer rrc_container;
};

class f1ap_rrc_message_handler
{
public:
  virtual ~f1ap_rrc_message_handler() = default;

  /// \brief Packs and transmits the DL RRC message transfer as per TS 38.473 section 8.4.2.
  /// \param[in] msg The DL RRC message transfer message to transmit.
  virtual void handle_dl_rrc_message_transfer(const f1ap_dl_rrc_message& msg) = 0;
};

/// Handle F1AP interface management procedures as defined in TS 38.473 section 8.2.
class f1ap_connection_manager
{
public:
  virtual ~f1ap_connection_manager() = default;

  /// \brief Creates and transmits the F1 Setup outcome to the DU.
  /// \param[in] msg The common type F1 Setup Response Message to transmit.
  /// \remark The CU transmits the F1SetupResponse/F1SetupFailure as per TS 38.473 section 8.2.3.
  virtual void handle_f1_setup_response(const f1ap_f1_setup_response& msg) = 0;
};

struct f1ap_ue_context_release_command {
  ue_index_t         ue_index = ue_index_t::invalid;
  cause_t            cause;
  byte_buffer        rrc_release_pdu;
  optional<srb_id_t> srb_id;
};

struct f1ap_ue_context_release_complete {
  asn1::f1ap::ue_context_release_complete_s msg;
};

/// Handle F1AP UE context management procedures as defined in TS 38.473 section 8.3.
class f1ap_ue_context_manager
{
public:
  virtual ~f1ap_ue_context_manager() = default;

  /// Establish the UE context in F1.
  virtual async_task<f1ap_ue_context_setup_response>
  handle_ue_context_setup_request(const f1ap_ue_context_setup_request& request) = 0;

  /// \brief Initiates the UE Context Release procedure as per TS 38.473 section 8.3.3.
  /// \param[in] msg The UE Context Release message to transmit.
  /// \return Retruns the index of the released UE.
  virtual async_task<ue_index_t> handle_ue_context_release_command(const f1ap_ue_context_release_command& msg) = 0;

  /// \brief Initiates the UE Context Modification procedure as per TS 38.473 section 8.3.4.
  /// \param[in] request The UE Context Modification message to transmit.
  /// \return Returns a f1ap_ue_context_modification_response_message struct with the success member set to
  /// 'true' in case of a successful outcome, 'false' otherwise.
  virtual async_task<f1ap_ue_context_modification_response>
  handle_ue_context_modification_request(const f1ap_ue_context_modification_request& request) = 0;
};

/// Handle F1AP paging procedures as defined in TS 38.473 section 8.7.
class f1ap_paging_manager
{
public:
  virtual ~f1ap_paging_manager() = default;

  virtual void handle_paging(const cu_cp_paging_message& msg) = 0;
};

/// Interface to notify the reception of an new RRC message.
class f1ap_rrc_message_notifier
{
public:
  virtual ~f1ap_rrc_message_notifier() = default;

  /// This callback is invoked on each received RRC message.
  virtual void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) = 0;
};

/// Dummy notifier that just logs the RRC message.
/// An object of this type is instantiated upon creation of the SRB context to avoid nullptr checks.
class f1ap_rrc_null_notifier : public f1ap_rrc_message_notifier
{
public:
  f1ap_rrc_null_notifier() = default;
  void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) override
  {
    srsran_assertion_failure("Received RRC message on unconnected notifier. Discarding.");
    logger.error("Received RRC message on unconnected notifier. Discarding.");
  };

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-CP-F1");
};

/// Non-owning handlers to RRC message notifiers.
using f1ap_srb_notifiers = std::array<f1ap_rrc_message_notifier*, MAX_NOF_SRBS>;

struct f1ap_srb_creation_message {
  ue_index_t ue_index = ue_index_t::invalid;
  srb_id_t   srb_id   = srb_id_t::nulltype;
};

struct ue_creation_complete_message {
  ue_index_t         ue_index = ue_index_t::invalid;
  f1ap_srb_notifiers srbs;
};

struct ue_update_message {
  ue_index_t          ue_index = ue_index_t::invalid;
  nr_cell_global_id_t cgi;
  rnti_t              c_rnti = INVALID_RNTI;
  byte_buffer         cell_group_cfg;
  byte_buffer         meas_gap_cfg;
  byte_buffer         requested_p_max_fr1;
};

struct ue_update_complete_message {
  ue_index_t         ue_index = ue_index_t::invalid;
  f1ap_srb_notifiers srbs;
};

/// Methods used by F1AP to notify the DU processor.
class f1ap_du_processor_notifier
{
public:
  virtual ~f1ap_du_processor_notifier() = default;

  /// \brief Notifies about the reception of a F1 Setup Request message.
  /// \param[in] msg The received F1 Setup Request message.
  virtual void on_f1_setup_request_received(const f1ap_f1_setup_request& msg) = 0;

  /// \brief Request allocation of a new UE index.
  virtual ue_index_t on_new_ue_index_required() = 0;

  /// \brief Notifies the DU processor to create a UE.
  /// \param[in] msg The ue creation message.
  /// \return Returns a UE creation complete message containing the index of the created UE and its SRB notifiers.
  virtual ue_creation_complete_message on_create_ue(const cu_cp_ue_creation_message& msg) = 0;

  /// \brief Instructs the DU processor to delete the given UE.
  virtual void on_delete_ue(ue_index_t ue_index) = 0;

  /// \brief Indicates the reception of a UE Context Release Request (gNB-DU initiated) as per TS 38.473
  /// section 8.3.2.
  virtual void on_du_initiated_ue_context_release_request(const f1ap_ue_context_release_request& req) = 0;

  /// \brief Get the DU index.
  /// \return The DU index.
  virtual du_index_t get_du_index() = 0;
};

/// Methods used by F1AP to notify about DU specific events.
class f1ap_du_management_notifier
{
public:
  virtual ~f1ap_du_management_notifier() = default;

  /// \brief Notifies about a successful F1 Removal procedure.
  /// The corresponding DU processor will be removed now.
  /// \param[in] du_index The index of the DU processor to delete.
  virtual void on_du_remove_request_received(const du_index_t du_index) = 0;
};

/// Methods to get statistics of the F1AP.
class f1ap_statistics_handler
{
public:
  virtual ~f1ap_statistics_handler() = default;

  /// \brief Returns the number of connected UEs at the F1AP
  /// \return The number of connected UEs.
  virtual int get_nof_ues() = 0;
};

/// Combined entry point for F1AP handling.
class f1ap_cu : public f1ap_message_handler,
                public f1ap_event_handler,
                public f1ap_rrc_message_handler,
                public f1ap_connection_manager,
                public f1ap_ue_context_manager,
                public f1ap_statistics_handler,
                public f1ap_paging_manager
{
public:
  virtual ~f1ap_cu() = default;

  virtual f1ap_message_handler&     get_f1ap_message_handler()     = 0;
  virtual f1ap_event_handler&       get_f1ap_event_handler()       = 0;
  virtual f1ap_rrc_message_handler& get_f1ap_rrc_message_handler() = 0;
  virtual f1ap_connection_manager&  get_f1ap_connection_manager()  = 0;
  virtual f1ap_ue_context_manager&  get_f1ap_ue_context_manager()  = 0;
  virtual f1ap_statistics_handler&  get_f1ap_statistics_handler()  = 0;
  virtual f1ap_paging_manager&      get_f1ap_paging_manager()      = 0;

  /// \brief Update a notifier to higher layers for a UE.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] srb_id The SRB ID to update.
  /// \param[in] notifier The new notifier.
  virtual void connect_srb_notifier(ue_index_t ue_index, srb_id_t srb_id, f1ap_rrc_message_notifier& notifier) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
