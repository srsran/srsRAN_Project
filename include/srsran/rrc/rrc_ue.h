/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/static_vector.h"
#include "srsran/asn1/rrc_nr/ue_cap.h"
#include "srsran/asn1/rrc_nr/ul_dcch_msg_ies.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/cu_cp_ue_messages.h"
#include "srsran/ran/rnti.h"
#include "srsran/rrc/rrc_types.h"
#include "srsran/rrc/rrc_ue_config.h"
#include "srsran/security/security.h"
#include "srsran/support/async/async_task.h"

namespace asn1 {
namespace rrc_nr {

// ASN.1 forward declarations
struct dl_ccch_msg_s;
struct dl_dcch_msg_s;

} // namespace rrc_nr
} // namespace asn1

namespace srsran {
namespace srs_cu_cp {

/// RRC states (3GPP 38.331 v15.5.1 Sec 4.2.1)
enum class rrc_state { idle = 0, connected, connected_inactive };

class rrc_ue_controller
{
public:
  virtual ~rrc_ue_controller() = default;

  /// \brief Cancel currently running transactions.
  virtual void stop() = 0;
};

enum ue_context_release_cause : uint16_t {
  radio_network = 0,
  transport     = 1,
  protocol      = 2,
  misc          = 3,
  choice_ext    = 4,
  nulltype      = 5
};

/// This interface represents the data entry point for the RRC UE receiving UL PDUs on the CCCH and DCCH logical
/// channel. The lower-layers will use this class to pass PDUs into the RRC.
class rrc_ul_pdu_handler
{
public:
  virtual ~rrc_ul_pdu_handler() = default;

  /// Handle the incoming PDU on the UL-CCCH logical channel.
  virtual void handle_ul_ccch_pdu(byte_buffer pdu) = 0;

  /// Handle the incoming SRB PDCP PDU on the UL-DCCH logical channel.
  virtual void handle_ul_dcch_pdu(const srb_id_t srb_id, byte_buffer pdu) = 0;
};

/// This interface represents the data entry point for the RRC receiving NAS and control messages from the NGAP.
/// The higher-layers will use this class to pass PDUs into the RRC.
class rrc_ngap_message_handler
{
public:
  virtual ~rrc_ngap_message_handler() = default;

  /// \brief Handle the received Downlink NAS Transport message.
  /// \param[in] nas_pdu The received NAS PDU.
  virtual void handle_dl_nas_transport_message(byte_buffer nas_pdu) = 0;

  /// \brief Get the packed UE Radio Access Cap Info.
  /// \returns The packed UE Radio Access Cap Info.
  virtual byte_buffer get_packed_ue_radio_access_cap_info() const = 0;

  /// \brief Get the packed Handover Preparation Message.
  virtual byte_buffer get_packed_handover_preparation_message() = 0;
};

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

/// Interface to notify about NGAP messages.
class rrc_ue_ngap_notifier
{
public:
  virtual ~rrc_ue_ngap_notifier() = default;

  /// \brief Notify about the Initial UE Message.
  /// \param[in] msg The initial UE message.
  virtual void on_initial_ue_message(const cu_cp_initial_ue_message& msg) = 0;

  /// \brief Notify about an Uplink NAS Transport message.
  /// \param[in] msg The Uplink NAS Transport message.
  virtual void on_ul_nas_transport_message(const cu_cp_ul_nas_transport& msg) = 0;

  /// \brief Notify about the reception of an inter CU handove related RRC Reconfiguration Complete.
  virtual void on_inter_cu_ho_rrc_recfg_complete_received(const ue_index_t           ue_index,
                                                          const nr_cell_global_id_t& cgi,
                                                          const tac_t                tac) = 0;
};

struct rrc_reconfiguration_response_message {
  ue_index_t ue_index = ue_index_t::invalid;
  bool       success  = false;
};

struct rrc_ue_security_mode_command_context {
  unsigned            transaction_id;
  nr_cell_global_id_t sp_cell_id;
  byte_buffer         rrc_ue_security_mode_command_pdu;
};

struct rrc_ue_release_context {
  cu_cp_user_location_info_nr user_location_info;
  byte_buffer                 rrc_release_pdu;
  srb_id_t                    srb_id = srb_id_t::nulltype;
};

struct rrc_ue_handover_reconfiguration_context {
  unsigned    transaction_id;
  byte_buffer rrc_ue_handover_reconfiguration_pdu;
};

/// Handle control messages.
class rrc_ue_control_message_handler
{
public:
  virtual ~rrc_ue_control_message_handler() = default;

  /// \brief Get the packed Security Mode Command.
  /// \returns The Security Mode Command context.
  virtual rrc_ue_security_mode_command_context get_security_mode_command_context() = 0;

  /// \brief Await a RRC Security Mode Complete.
  /// \param[in] transaction_id The transaction ID of the RRC Security Mode Complete.
  /// \returns True if the RRC Security Mode Complete was received, false otherwise.
  virtual async_task<bool> handle_security_mode_complete_expected(uint8_t transaction_id) = 0;

  /// \brief Get the packed UE Capability RAT Container List.
  /// \returns The packed UE Capability RAT Container List.
  virtual byte_buffer get_packed_ue_capability_rat_container_list() const = 0;

  /// \brief Handle an RRC Reconfiguration Request.
  /// \param[in] msg The new RRC Reconfiguration Request.
  /// \returns The result of the rrc reconfiguration.
  virtual async_task<bool> handle_rrc_reconfiguration_request(const rrc_reconfiguration_procedure_request& msg) = 0;

  /// \brief Get the RRC Reconfiguration context for a handover.
  /// \param[in] msg The new RRC Reconfiguration Request.
  /// \returns The RRC handover reconfiguration context.
  virtual rrc_ue_handover_reconfiguration_context
  get_rrc_ue_handover_reconfiguration_context(const rrc_reconfiguration_procedure_request& msg) = 0;

  /// \brief Await a RRC Reconfiguration Complete for a handover.
  /// \param[in] transaction_id The transaction ID of the RRC Reconfiguration Complete.
  /// \param[in] timeout_ms The timeout for the RRC Reconfiguration Complete.
  /// \returns True if the RRC Reconfiguration Complete was received, false otherwise.
  virtual async_task<bool> handle_handover_reconfiguration_complete_expected(uint8_t                   transaction_id,
                                                                             std::chrono::milliseconds timeout_ms) = 0;

  /// \brief Store UE capabilities received from the NGAP.
  /// \param[in] ue_capabilities The UE capabilities.
  /// \returns True if the UE capabilities were stored successfully, false otherwise.
  virtual bool store_ue_capabilities(byte_buffer ue_capabilities) = 0;

  /// \brief Initiate the UE capability transfer procedure.
  virtual async_task<bool> handle_rrc_ue_capability_transfer_request(const rrc_ue_capability_transfer_request& msg) = 0;

  /// \brief Get the RRC UE release context.
  /// \returns The release context of the UE. If SRB1 is not created yet, a RrcReject message is contained in the
  /// release context, see section 5.3.15 in TS 38.331. Otherwise, a RrcRelease message is contained in the release
  /// context.
  virtual rrc_ue_release_context
  get_rrc_ue_release_context(bool                                requires_rrc_msg,
                             std::optional<std::chrono::seconds> release_wait_time = std::nullopt) = 0;

  /// \brief Retrieve RRC context of a UE to perform mobility (handover, reestablishment).
  /// \return Transfer context including UP context, security, SRBs, HO preparation, etc.
  virtual rrc_ue_transfer_context get_transfer_context() = 0;

  /// \brief Get the RRC measurement config for the current serving cell of the UE.
  /// \param[in] current_meas_config The current meas config of the UE (if applicable).
  /// \return The measurement config, if present.
  virtual std::optional<rrc_meas_cfg>
  generate_meas_config(const std::optional<rrc_meas_cfg>& current_meas_config = std::nullopt) = 0;

  /// \brief Get the packed RRC measurement config for the current serving cell of the UE.
  virtual byte_buffer get_packed_meas_config() = 0;

  /// \brief Handle the handover command RRC PDU.
  /// \param[in] cmd The handover command RRC PDU.
  /// \returns The handover RRC Reconfiguration PDU. If the handover command is invalid, the PDU is empty.
  virtual byte_buffer handle_rrc_handover_command(byte_buffer cmd) = 0;

  /// \brief Handle the handover preparation info RRC PDU.
  /// \param[in] pdu The handover preparation info RRC PDU.
  /// \returns True if the handover preparation info was successfully handled, false otherwise.
  virtual bool handle_rrc_handover_preparation_info(byte_buffer pdu) = 0;

  /// \brief Get the packed RRC Handover Command.
  /// \param[in] msg The new RRC Reconfiguration Request.
  /// \returns The RRC Handover Command.
  virtual byte_buffer get_rrc_handover_command(const rrc_reconfiguration_procedure_request& request,
                                               unsigned                                     transaction_id) = 0;

  /// \brief Get the packed RRC Handover Preparation Message.
  virtual byte_buffer get_packed_handover_preparation_message() = 0;

  /// \brief Instruct the RRC UE to create a new SRB. It creates all
  /// required intermediate objects (e.g. PDCP) and connects them with one another.
  /// \param[in] msg The UE index, SRB ID and config.
  virtual void create_srb(const srb_creation_message& msg) = 0;

  /// \brief Get all SRBs of the UE.
  virtual static_vector<srb_id_t, MAX_NOF_SRBS> get_srbs() = 0;

  /// \brief Get the RRC connection state of the UE.
  virtual rrc_state get_rrc_state() const = 0;

  /// \brief Cancel an ongoing handover reconfiguration transaction.
  /// \param[in] transaction_id The transaction ID of the handover reconfiguration transaction.
  virtual void cancel_handover_reconfiguration_transaction(uint8_t transaction_id) = 0;
};

class rrc_ue_cu_cp_ue_notifier
{
public:
  virtual ~rrc_ue_cu_cp_ue_notifier() = default;

  /// \brief Get the timer factory for the UE.
  virtual timer_factory get_timer_factory() = 0;

  /// \brief Get the task executor for the UE.
  virtual task_executor& get_executor() = 0;

  /// \brief Schedule an async task for the UE.
  virtual bool schedule_async_task(async_task<void> task) = 0;

  /// \brief Get the AS configuration for the RRC domain
  virtual security::sec_as_config get_rrc_as_config() = 0;

  /// \brief Get the AS configuration for the RRC domain with 128-bit keys
  virtual security::sec_128_as_config get_rrc_128_as_config() = 0;

  /// \brief Enable security
  virtual void enable_security() = 0;

  /// \brief Get the current security context
  virtual security::security_context get_security_context() = 0;

  /// \brief Get the selected security algorithms
  virtual security::sec_selected_algos get_security_algos() = 0;

  /// \brief Update the security context
  /// \param[in] sec_ctxt The new security context
  virtual void update_security_context(const security::security_context& sec_ctxt) = 0;

  /// \brief Perform horizontal key derivation
  virtual void perform_horizontal_key_derivation(pci_t target_pci, unsigned target_ssb_arfcn) = 0;
};

/// Struct containing all information needed from the old RRC UE for Reestablishment.
struct rrc_ue_reestablishment_context_response {
  ue_index_t                                               ue_index = ue_index_t::invalid;
  security::security_context                               sec_context;
  std::optional<asn1::rrc_nr::ue_cap_rat_container_list_l> capabilities_list;
  up_context                                               up_ctx;
  bool                                                     old_ue_fully_attached   = false;
  bool                                                     reestablishment_ongoing = false;
};

/// Interface to notify about UE context updates.
class rrc_ue_context_update_notifier
{
public:
  virtual ~rrc_ue_context_update_notifier() = default;

  /// \brief Notifies that a new RRC UE needs to be setup.
  /// \param[in] plmn The PLMN of the UE.
  /// \return True if the UE is accepted.
  virtual bool on_ue_setup_request(plmn_identity plmn) = 0;

  /// \brief Notify about the reception of an RRC Reestablishment Request.
  /// \param[in] old_pci The old PCI contained in the RRC Reestablishment Request.
  /// \param[in] old_c_rnti The old C-RNTI contained in the RRC Reestablishment Request.
  /// \param[in] ue_index The new UE index of the UE that sent the Reestablishment Request.
  /// \returns The RRC Reestablishment UE context for the old UE.
  virtual rrc_ue_reestablishment_context_response on_rrc_reestablishment_request(pci_t old_pci, rnti_t old_c_rnti) = 0;

  /// \brief Notify about a required reestablishment context modification.
  virtual async_task<bool> on_rrc_reestablishment_context_modification_required() = 0;

  /// \brief Notify the CU-CP to release the old UE after a reestablishment failure.
  /// \param[in] request The release request.
  virtual void on_rrc_reestablishment_failure(const cu_cp_ue_context_release_request& request) = 0;

  /// \brief Notify the CU-CP to remove the old UE from the CU-CP after an successful reestablishment.
  /// \param[in] old_ue_index The index of the old UE to remove.
  virtual void on_rrc_reestablishment_complete(ue_index_t old_ue_index) = 0;

  /// \brief Notify the CU-CP that RRC Reconfiguration has been received, so that the CU-CP can notify the DU if
  /// required.
  /// \param[in] ue_index The index of the UE that received the reconfiguration complete.
  virtual void on_rrc_reconfiguration_complete_indicator() = 0;

  /// \brief Notify the CU-CP to transfer and remove ue contexts.
  /// \param[in] old_ue_index The old UE index of the UE that sent the Reestablishment Request.
  virtual async_task<bool> on_ue_transfer_required(ue_index_t old_ue_index) = 0;

  /// \brief Notify the CU-CP to release a UE.
  /// \param[in] request The release request.
  virtual async_task<void> on_ue_release_required(const cu_cp_ue_context_release_request& request) = 0;

  /// \brief Notify the CU-CP to setup an UP context.
  /// \param[in] ctxt The UP context to setup.
  virtual void on_up_context_setup_required(const up_context& ctxt) = 0;

  /// \brief Get the UP context of the UE.
  /// \returns The UP context of the UE.
  virtual up_context on_up_context_required() = 0;

  /// \brief Notify the CU-CP to remove a UE from the CU-CP.
  virtual async_task<void> on_ue_removal_required() = 0;
};

/// Interface to notify about measurements
class rrc_ue_measurement_notifier
{
public:
  virtual ~rrc_ue_measurement_notifier() = default;

  /// \brief Retrieve the measurement config (for any UE) connected to the given serving cell.
  /// \param[in] nci The cell id of the serving cell to update.
  /// \param[in] current_meas_config The current meas config of the UE (if applicable).
  virtual std::optional<rrc_meas_cfg>
  on_measurement_config_request(nr_cell_identity                   nci,
                                const std::optional<rrc_meas_cfg>& current_meas_config = std::nullopt) = 0;

  /// \brief Submit measurement report for given UE to cell manager.
  virtual void on_measurement_report(const rrc_meas_results& meas_results) = 0;
};

class rrc_ue_context_handler
{
public:
  virtual ~rrc_ue_context_handler() = default;

  /// \brief Get the RRC Reestablishment UE context to transfer it to new UE.
  /// \returns The RRC Reestablishment UE Context.
  virtual rrc_ue_reestablishment_context_response get_context() = 0;
};

class rrc_ue_event_notifier
{
public:
  virtual ~rrc_ue_event_notifier() = default;

  /// \brief Notify the RRC DU about a new RRC connection.
  virtual void on_new_rrc_connection() = 0;

  /// \brief Notify the RRC DU about a successful RRC release.
  virtual void on_successful_rrc_release() = 0;

  /// \brief Notify the RRC DU about a new RRC connection establishment attempt.
  /// \param[in] cause The establishment cause of the RRC connection.
  virtual void on_attempted_rrc_connection_establishment(establishment_cause_t cause) = 0;

  /// \brief Notify the RRC DU about a successful RRC connection establishment.
  /// \param[in] cause The establishment cause of the RRC connection.
  virtual void on_successful_rrc_connection_establishment(establishment_cause_t cause) = 0;

  /// \brief Notify the RRC DU about the attempted RRC connection re-establishment.
  virtual void on_attempted_rrc_connection_reestablishment() = 0;

  /// \brief Notify the RRC DU about the successful RRC connection re-establishment.
  virtual void on_successful_rrc_connection_reestablishment() = 0;

  /// \brief Notify the RRC DU about the successful RRC connection re-establishment fallback.
  virtual void on_successful_rrc_connection_reestablishment_fallback() = 0;
};

/// Combined entry point for the RRC UE handling.
/// It will contain getters for the interfaces for the various logical channels handled by RRC.
class rrc_ue_interface : public rrc_ul_pdu_handler,
                         public rrc_ngap_message_handler,
                         public rrc_ue_control_message_handler,
                         public rrc_ue_setup_proc_notifier,
                         public rrc_ue_security_mode_command_proc_notifier,
                         public rrc_ue_reconfiguration_proc_notifier,
                         public rrc_ue_context_handler,
                         public rrc_ue_reestablishment_proc_notifier
{
public:
  rrc_ue_interface()          = default;
  virtual ~rrc_ue_interface() = default;

  virtual rrc_ue_controller&              get_controller()                     = 0;
  virtual rrc_ul_pdu_handler&             get_ul_pdu_handler()                 = 0;
  virtual rrc_ngap_message_handler&       get_rrc_ngap_message_handler()       = 0;
  virtual rrc_ue_control_message_handler& get_rrc_ue_control_message_handler() = 0;
  virtual rrc_ue_context_handler&         get_rrc_ue_context_handler()         = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
