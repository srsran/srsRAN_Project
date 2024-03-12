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

#include "rrc_cell_context.h"
#include "rrc_types.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/static_vector.h"
#include "srsran/asn1/rrc_nr/ue_cap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/cu_cp_ue_messages.h"
#include "srsran/cu_cp/up_resource_manager.h"
#include "srsran/rrc/rrc.h"
#include "srsran/rrc/rrc_ue_config.h"
#include "srsran/security/security.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/timers.h"

namespace asn1 {
namespace rrc_nr {

// ASN.1 forward declarations
struct dl_ccch_msg_s;
struct dl_dcch_msg_s;

} // namespace rrc_nr
} // namespace asn1

namespace srsran {
namespace srs_cu_cp {

/// Interface to notify F1AP about a new SRB PDU.
class rrc_pdu_f1ap_notifier
{
public:
  virtual ~rrc_pdu_f1ap_notifier() = default;

  /// \brief Notify the PDCP about a new RRC PDU that needs ciphering and integrity protection.
  /// \param[in] pdu The RRC PDU.
  /// \param[in] srb_id The SRB ID of the PDU.
  virtual void on_new_rrc_pdu(const srb_id_t srb_id, const byte_buffer& pdu) = 0;
};

/// Interface used by the RRC Setup procedure to notify the RRC UE.
class rrc_ue_setup_proc_notifier
{
public:
  virtual ~rrc_ue_setup_proc_notifier() = default;

  /// \brief Notify about a DL CCCH message.
  /// \param[in] dl_ccch_msg The DL CCCH message.
  virtual void on_new_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg) = 0;

  /// \brief Notify about the need to release a UE.
  virtual void on_ue_release_required(const ngap_cause_t& cause) = 0;
};

struct srb_creation_message {
  ue_index_t      ue_index        = ue_index_t::invalid;
  ue_index_t      old_ue_index    = ue_index_t::invalid;
  srb_id_t        srb_id          = srb_id_t::nulltype;
  bool            enable_security = false; // Activate security upon SRB creation.
  srb_pdcp_config pdcp_cfg;
};

/// Interface to handle the creation of SRBs.
class rrc_ue_srb_handler
{
public:
  virtual ~rrc_ue_srb_handler() = default;

  /// \brief Instruct the RRC UE to create a new SRB. It creates all
  /// required intermediate objects (e.g. PDCP) and connects them with one another.
  /// \param[in] msg The UE index, SRB ID and config.
  virtual void create_srb(const srb_creation_message& msg) = 0;

  /// \brief Get all SRBs of the UE.
  virtual static_vector<srb_id_t, MAX_NOF_SRBS> get_srbs() = 0;
};

/// Interface used by the RRC reconfiguration procedure to
/// invoke actions carried out by the main RRC UE class (i.e. send DL message, remove UE).
class rrc_ue_reconfiguration_proc_notifier
{
public:
  rrc_ue_reconfiguration_proc_notifier()          = default;
  virtual ~rrc_ue_reconfiguration_proc_notifier() = default;

  /// \brief Notify about a DL DCCH message.
  /// \param[in] dl_dcch_msg The DL DCCH message.
  virtual void on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg) = 0;

  /// \brief Notify about the need to release a UE.
  virtual void on_ue_release_required(const ngap_cause_t& cause) = 0;
};

/// Interface used by the RRC security mode procedure
/// to notify the RRC UE of the security mode context update.
class rrc_ue_security_mode_command_proc_notifier
{
public:
  rrc_ue_security_mode_command_proc_notifier()          = default;
  virtual ~rrc_ue_security_mode_command_proc_notifier() = default;

  /// \brief Notify about a DL DCCH message.
  /// \param[in] dl_dcch_msg The DL DCCH message.
  virtual void on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg) = 0;

  /// \brief Setup AS security in the UE. This includes configuring
  /// the PDCP entity security on SRB1 with the new AS keys.
  virtual void on_new_as_security_context() = 0;

  /// \brief Setup AS security in the UE. This includes configuring
  /// the PDCP entity security on SRB1 with the new AS keys.
  virtual void on_security_context_sucessful() = 0;
};

/// Interface used by the RRC reestablishment procedure to
/// invoke actions carried out by the main RRC UE class (i.e. send DL message, remove UE).
class rrc_ue_reestablishment_proc_notifier
{
public:
  rrc_ue_reestablishment_proc_notifier()          = default;
  virtual ~rrc_ue_reestablishment_proc_notifier() = default;

  /// \brief Notify about a DL DCCH message.
  /// \param[in] dl_dcch_msg The DL DCCH message.
  virtual void on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg) = 0;

  /// \brief Refresh AS security keys after horizontal key derivation.
  /// This includes configuring the PDCP entity security on SRB1 with the new AS keys.
  virtual void on_new_as_security_context() = 0;
};

/// Interface to notify about RRC UE Context messages.
class rrc_ue_du_processor_notifier
{
public:
  virtual ~rrc_ue_du_processor_notifier() = default;

  /// \brief Notify about a UE Context Release Command.
  /// \param[in] cmd The UE Context Release Command.
  virtual async_task<cu_cp_ue_context_release_complete>
  on_ue_context_release_command(const cu_cp_ue_context_release_command& cmd) = 0;

  /// \brief Notify about a required reestablishment context modification.
  /// \param[in] ue_index The index of the UE that needs the context modification.
  virtual async_task<bool> on_rrc_reestablishment_context_modification_required(ue_index_t ue_index) = 0;
};

/// Schedules asynchronous tasks associated with an UE.
class rrc_ue_task_scheduler
{
public:
  virtual ~rrc_ue_task_scheduler() = default;

  /// \brief Schedule an asynchronous task for the UE.
  virtual void schedule_async_task(async_task<void> task) = 0;

  /// \brief Create a new timer for the UE.
  virtual unique_timer make_unique_timer() = 0;

  /// \brief Get UE timer factory.
  virtual timer_factory get_timer_factory() = 0;
};

/// Interface to notify about NAS messages.
class rrc_ue_nas_notifier
{
public:
  virtual ~rrc_ue_nas_notifier() = default;

  /// \brief Notify about the Initial UE Message.
  /// \param[in] msg The initial UE message.
  virtual void on_initial_ue_message(const cu_cp_initial_ue_message& msg) = 0;

  /// \brief Notify about an Uplink NAS Transport message.
  /// \param[in] msg The Uplink NAS Transport message.
  virtual void on_ul_nas_transport_message(const cu_cp_ul_nas_transport& msg) = 0;
};

struct rrc_reconfiguration_response_message {
  ue_index_t ue_index = ue_index_t::invalid;
  bool       success  = false;
};

/// Interface to notify about control messages.
class rrc_ue_control_notifier
{
public:
  virtual ~rrc_ue_control_notifier() = default;

  virtual async_task<bool> on_ue_context_release_request(const cu_cp_ue_context_release_request& msg) = 0;

  /// \brief Notify about the reception of an inter CU handove related RRC Reconfiguration Complete.
  virtual void on_inter_cu_ho_rrc_recfg_complete_received(const ue_index_t           ue_index,
                                                          const nr_cell_global_id_t& cgi,
                                                          const unsigned             tac) = 0;
};

struct rrc_ue_release_context {
  cu_cp_user_location_info_nr user_location_info;
  byte_buffer                 rrc_release_pdu;
  srb_id_t                    srb_id = srb_id_t::nulltype;
};

/// Handle control messages.
class rrc_ue_control_message_handler
{
public:
  virtual ~rrc_ue_control_message_handler() = default;

  /// \brief Handle an RRC Reconfiguration Request.
  /// \param[in] msg The new RRC Reconfiguration Request.
  /// \returns The result of the rrc reconfiguration.
  virtual async_task<bool> handle_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) = 0;

  /// \brief Handle an RRC Reconfiguration Request for a handover.
  /// \param[in] msg The new RRC Reconfiguration Request.
  /// \returns The transaction ID of the RRC Reconfiguration request.
  virtual uint8_t handle_handover_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) = 0;

  /// \brief Await a RRC Reconfiguration Complete for a handover.
  /// \param[in] transaction_id The transaction ID of the RRC Reconfiguration Complete.
  /// \returns True if the RRC Reconfiguration Complete was received, false otherwise.
  virtual async_task<bool> handle_handover_reconfiguration_complete_expected(uint8_t transaction_id) = 0;

  /// \brief Initiate the UE capability transfer procedure.
  virtual async_task<bool> handle_rrc_ue_capability_transfer_request(const rrc_ue_capability_transfer_request& msg) = 0;

  /// \brief Get the RRC UE release context.
  /// \returns The release context of the UE.  If SRB1 is not created yet, a RrcReject message is contained in the
  /// release context, see section 5.3.15 in TS 38.331. Otherwise, a RrcRelease message is contained in the release
  /// context.
  virtual rrc_ue_release_context get_rrc_ue_release_context() = 0;

  /// \brief Retrieve RRC context of a UE to perform mobility (handover, reestablishment).
  /// \return Transfer context including UP context, security, SRBs, HO preparation, etc.
  virtual rrc_ue_transfer_context get_transfer_context() = 0;

  /// \brief Get the RRC measurement config for the current serving cell of the UE.
  /// \params[in] current_meas_config The current meas config of the UE (if applicable).
  /// \return The measurement config, if present.
  virtual optional<rrc_meas_cfg> generate_meas_config(optional<rrc_meas_cfg> current_meas_config) = 0;

  /// \brief Handle the reception of a new security context.
  /// \return True if the security context was applied successfully, false otherwise
  virtual bool handle_new_security_context(const security::security_context& sec_context) = 0;

  /// \brief Get the packed RRC Handover Command.
  /// \returns The RRC Handover Command.
  virtual byte_buffer get_rrc_handover_command(const rrc_reconfiguration_procedure_request& request,
                                               unsigned                                     transaction_id) = 0;

  virtual byte_buffer get_packed_handover_preparation_message() = 0;
};

/// Handler to initialize the security context from NGAP.
class rrc_ue_init_security_context_handler
{
public:
  virtual ~rrc_ue_init_security_context_handler() = default;

  /// \brief Handle the received Init Security Context.
  /// \param[in] sec_ctxt The Init Security Context.
  virtual async_task<bool> handle_init_security_context(const security::security_context& sec_ctxt) = 0;

  /// \brief Get the status of the security context.
  virtual bool get_security_enabled() = 0;
};

/// Handler to get the handover preparation context to the NGAP.
class rrc_ue_handover_preparation_handler
{
public:
  virtual ~rrc_ue_handover_preparation_handler() = default;

  virtual byte_buffer get_packed_handover_preparation_message() = 0;

  /// \brief Handle the handover command RRC PDU.
  /// \param[in] cmd The handover command RRC PDU.
  /// \returns true if the rrc reconfig was successfully forwarded to the DU, false otherwise.
  virtual bool handle_rrc_handover_command(byte_buffer cmd) = 0;
};

/// Struct containing all information needed from the old RRC UE for Reestablishment.
struct rrc_reestablishment_ue_context_t {
  ue_index_t                          ue_index = ue_index_t::invalid;
  security::security_context          sec_context;
  optional<asn1::rrc_nr::ue_nr_cap_s> capabilities;
  up_context                          up_ctx;
  bool                                old_ue_fully_attached = false;
};

/// Interface to notify about UE context updates.
class rrc_ue_context_update_notifier
{
public:
  virtual ~rrc_ue_context_update_notifier() = default;

  /// \brief Notifies that a new RRC UE needs to be setup.
  /// \return True if the UE is accepted.
  virtual bool on_ue_setup_request() = 0;

  /// \brief Notify about the reception of an RRC Reestablishment Request.
  /// \param[in] old_pci The old PCI contained in the RRC Reestablishment Request.
  /// \param[in] old_c_rnti The old C-RNTI contained in the RRC Reestablishment Request.
  /// \param[in] ue_index The new UE index of the UE that sent the Reestablishment Request.
  /// \returns The RRC Reestablishment UE context for the old UE.
  virtual rrc_reestablishment_ue_context_t
  on_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti, ue_index_t ue_index) = 0;

  /// \brief Notify the CU-CP to transfer and remove ue contexts.
  /// \param[in] ue_index The new UE index of the UE that sent the Reestablishment Request.
  /// \param[in] old_ue_index The old UE index of the UE that sent the Reestablishment Request.
  virtual async_task<bool> on_ue_transfer_required(ue_index_t ue_index, ue_index_t old_ue_index) = 0;

  /// \brief Notify the CU-CP to completly remove a UE from the CU-CP.
  /// \param[in] ue_index The index of the UE to remove.
  virtual void on_ue_removal_required(ue_index_t ue_index) = 0;
};

/// Interface to notify about measurements
class rrc_ue_measurement_notifier
{
public:
  virtual ~rrc_ue_measurement_notifier() = default;

  /// \brief Retrieve the measurement config (for any UE) connected to the given serving cell.
  /// \param[in] ue_index The index of the UE to retrieve the measurement config for.
  /// \param[in] nci The cell id of the serving cell to update.
  /// \param[in] current_meas_config The current meas config of the UE (if applicable).
  virtual optional<rrc_meas_cfg> on_measurement_config_request(ue_index_t             ue_index,
                                                               nr_cell_id_t           nci,
                                                               optional<rrc_meas_cfg> current_meas_config = {}) = 0;

  /// \brief Submit measurement report for given UE to cell manager.
  virtual void on_measurement_report(const ue_index_t ue_index, const rrc_meas_results& meas_results) = 0;
};

class rrc_ue_context_handler
{
public:
  virtual ~rrc_ue_context_handler() = default;

  /// \brief Get the RRC Reestablishment UE context to transfer it to new UE.
  /// \returns The RRC Reestablishment UE Context.
  virtual rrc_reestablishment_ue_context_t get_context() = 0;
};

/// Combined entry point for the RRC UE handling.
/// It will contain getters for the interfaces for the various logical channels handled by RRC.
class rrc_ue_interface : public rrc_ul_ccch_pdu_handler,
                         public rrc_ul_dcch_pdu_handler,
                         public rrc_dl_nas_message_handler,
                         public rrc_ue_srb_handler,
                         public rrc_ue_control_message_handler,
                         public rrc_ue_init_security_context_handler,
                         public rrc_ue_setup_proc_notifier,
                         public rrc_ue_security_mode_command_proc_notifier,
                         public rrc_ue_reconfiguration_proc_notifier,
                         public rrc_ue_context_handler,
                         public rrc_ue_reestablishment_proc_notifier,
                         public rrc_ue_handover_preparation_handler
{
public:
  rrc_ue_interface()          = default;
  virtual ~rrc_ue_interface() = default;

  virtual rrc_ul_ccch_pdu_handler&              get_ul_ccch_pdu_handler()                  = 0;
  virtual rrc_ul_dcch_pdu_handler&              get_ul_dcch_pdu_handler()                  = 0;
  virtual rrc_dl_nas_message_handler&           get_rrc_dl_nas_message_handler()           = 0;
  virtual rrc_ue_srb_handler&                   get_rrc_ue_srb_handler()                   = 0;
  virtual rrc_ue_control_message_handler&       get_rrc_ue_control_message_handler()       = 0;
  virtual rrc_ue_init_security_context_handler& get_rrc_ue_init_security_context_handler() = 0;
  virtual security::security_context&           get_rrc_ue_security_context()              = 0;
  virtual rrc_ue_context_handler&               get_rrc_ue_context_handler()               = 0;
  virtual rrc_ue_handover_preparation_handler&  get_rrc_ue_handover_preparation_handler()  = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
