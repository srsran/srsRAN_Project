/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../common/f1ap_ue_id.h"
#include "du_setup_notifier.h"
#include "f1ap_cu_ue_context_update.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/expected.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/cu_cp_ue_messages.h"
#include "srsran/f1ap/common/f1ap_common.h"
#include "srsran/ran/lcid.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_cu_cp {

struct f1ap_dl_rrc_message {
  ue_index_t  ue_index = ue_index_t::invalid;
  srb_id_t    srb_id   = srb_id_t::nulltype;
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

struct f1ap_ue_context_release_command {
  ue_index_t         ue_index = ue_index_t::invalid;
  f1ap_cause_t       cause;
  byte_buffer        rrc_release_pdu;
  optional<srb_id_t> srb_id;
};

/// Handle F1AP UE context management procedures as defined in TS 38.473 section 8.3.
class f1ap_ue_context_manager
{
public:
  virtual ~f1ap_ue_context_manager() = default;

  /// Establish the UE context in F1.
  virtual async_task<f1ap_ue_context_setup_response>
  handle_ue_context_setup_request(const f1ap_ue_context_setup_request& request,
                                  optional<rrc_ue_transfer_context>    rrc_context) = 0;

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

  /// \brief Indicates that a UE changed identifiers (e.g. due to a Reestablishment).
  /// \param[in] ue_index New index of the UE.
  /// \param[in] old_ue_index Old index of the UE.
  /// \return True if both the new and old UE exist, false otherwise.
  virtual bool handle_ue_id_update(ue_index_t ue_index, ue_index_t old_ue_index) = 0;
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

  /// This callback is invoked on each received UL CCCH RRC message.
  virtual void on_ul_ccch_pdu(byte_buffer pdu) = 0;

  /// This callback is invoked on each received UL DCCH RRC message.
  virtual void on_ul_dcch_pdu(const srb_id_t srb_id, byte_buffer pdu) = 0;
};

/// \brief Request made by the F1AP-CU to create a RRC context for an existing UE context in the CU-CP.
///
/// This request should be made once the C-RNTI and cell of the UE is known. That generally corresponds to the moment
/// a Initial UL RRC Message or a F1AP UE Context Setup Response are received.
struct ue_rrc_context_creation_request {
  ue_index_t                        ue_index;
  rnti_t                            c_rnti;
  nr_cell_global_id_t               cgi;
  byte_buffer                       du_to_cu_rrc_container;
  optional<rrc_ue_transfer_context> prev_context;
};

/// \brief Response by CU-CP to F1AP-CU request to create UE RRC context.
struct ue_rrc_context_creation_response {
  /// Notifier to be used by the F1AP to push new RRC PDUs to the UE RRC layer.
  f1ap_rrc_message_notifier* f1ap_rrc_notifier = nullptr;
};

struct ue_update_message {
  ue_index_t          ue_index = ue_index_t::invalid;
  nr_cell_global_id_t cgi;
  rnti_t              c_rnti = rnti_t::INVALID_RNTI;
  byte_buffer         cell_group_cfg;
  byte_buffer         meas_gap_cfg;
  byte_buffer         requested_p_max_fr1;
};

struct ue_update_complete_message {
  ue_index_t                 ue_index          = ue_index_t::invalid;
  f1ap_rrc_message_notifier* f1ap_rrc_notifier = nullptr;
};

/// Methods used by F1AP to notify the DU processor.
class f1ap_du_processor_notifier : public du_setup_notifier
{
public:
  virtual ~f1ap_du_processor_notifier() = default;

  /// \brief Notifies the CU-CP to create a new UE instance.
  virtual ue_index_t on_new_cu_cp_ue_required() = 0;

  /// \brief Notifies the CU-CP that an RRC context has been created for an existing CU-CP UE.
  virtual ue_rrc_context_creation_response
  on_ue_rrc_context_creation_request(const ue_rrc_context_creation_request& req) = 0;

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
  virtual size_t get_nof_ues() const = 0;
};

/// Handle UE context removal
class f1ap_ue_context_removal_handler
{
public:
  virtual ~f1ap_ue_context_removal_handler() = default;

  /// \brief Remove the context of an UE.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void remove_ue_context(ue_index_t ue_index) = 0;
};

/// Interface to notify about necessary UE removals.
class f1ap_ue_removal_notifier
{
public:
  virtual ~f1ap_ue_removal_notifier() = default;

  /// \brief Notify the CU-CP to completly remove a UE from the CU-CP.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void on_ue_removal_required(ue_index_t ue_index) = 0;
};

/// Combined entry point for F1AP handling.
class f1ap_cu : public f1ap_message_handler,
                public f1ap_event_handler,
                public f1ap_rrc_message_handler,
                public f1ap_ue_context_manager,
                public f1ap_statistics_handler,
                public f1ap_paging_manager,
                public f1ap_ue_context_removal_handler
{
public:
  virtual ~f1ap_cu() = default;

  virtual f1ap_message_handler&            get_f1ap_message_handler()            = 0;
  virtual f1ap_event_handler&              get_f1ap_event_handler()              = 0;
  virtual f1ap_rrc_message_handler&        get_f1ap_rrc_message_handler()        = 0;
  virtual f1ap_ue_context_manager&         get_f1ap_ue_context_manager()         = 0;
  virtual f1ap_statistics_handler&         get_f1ap_statistics_handler()         = 0;
  virtual f1ap_paging_manager&             get_f1ap_paging_manager()             = 0;
  virtual f1ap_ue_context_removal_handler& get_f1ap_ue_context_removal_handler() = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
