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

#include "rrc_cell_context.h"
#include "rrc_types.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/asn1/rrc_nr/dl_dcch_msg.h"
#include "srsran/asn1/rrc_nr/msg_common.h"
#include "srsran/asn1/rrc_nr/ue_cap.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/up_resource_manager.h"
#include "srsran/rrc/rrc.h"
#include "srsran/security/security.h"
#include "srsran/support/async/async_task.h"
#include "srsran/support/timers.h"

namespace srsran {

namespace srs_cu_cp {

struct rrc_pdu_message {
  rrc_pdu_message(byte_buffer_slice pdu_) : pdu(std::move(pdu_)) {}
  byte_buffer_slice pdu;
};

/// Interface to notify about a new SRB PDU.
class rrc_pdu_notifier
{
public:
  virtual ~rrc_pdu_notifier() = default;

  /// \brief Notify about a new PDU.
  /// \param[in] msg The RRC PDU message.
  /// \param[in] old_ue_index Optional old index of UE, e.g. for reestablishment.
  virtual void on_new_pdu(const rrc_pdu_message& msg, ue_index_t old_ue_index = ue_index_t::invalid) = 0;
};

/// Interface to configure security in a SRB
/// TX PDCP entity.
class rrc_tx_security_notifier
{
public:
  virtual ~rrc_tx_security_notifier() = default;

  virtual void enable_security(security::sec_128_as_config sec_cfg) = 0;
};

/// Interface to configure security in a SRB.
/// RX PDCP entity.
class rrc_rx_security_notifier
{
public:
  virtual ~rrc_rx_security_notifier() = default;

  virtual void enable_security(security::sec_128_as_config sec_cfg) = 0;
};

/// Struct to hold notifiers for a specific SRB
struct srb_notifiers {
  rrc_pdu_notifier*         pdu_notifier    = nullptr;
  rrc_tx_security_notifier* tx_sec_notifier = nullptr;
  rrc_rx_security_notifier* rx_sec_notifier = nullptr;
};

/// Non-owning handlers to PDU notifiers.
using srb_notifiers_array = std::array<srb_notifiers, MAX_NOF_SRBS>;

/// Dummy notifier that just logs the PDU.
/// An object of this type is instantiated upon creation of the SRB context to avoid nullptr checks.
class rrc_pdu_null_notifier : public rrc_pdu_notifier
{
public:
  rrc_pdu_null_notifier() = default;
  void on_new_pdu(const rrc_pdu_message& msg, ue_index_t old_ue_index) override
  {
    srsran_assertion_failure("Received PDU on unconnected notifier. Discarding.");
    logger.error("Received PDU on unconnected notifier. Discarding.");
  };

private:
  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC");
};

/// Interface used by the RRC Setup procedure to notifiy the RRC UE.
class rrc_ue_setup_proc_notifier
{
public:
  rrc_ue_setup_proc_notifier()          = default;
  virtual ~rrc_ue_setup_proc_notifier() = default;

  /// \brief Notify about a DL CCCH message.
  /// \param[in] dl_ccch_msg The DL CCCH message.
  virtual void on_new_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg) = 0;

  /// \brief Notify about the need to delete a UE.
  virtual void on_ue_delete_request(const cause_t& cause) = 0;
};

struct srb_creation_message {
  ue_index_t               ue_index     = ue_index_t::invalid;
  ue_index_t               old_ue_index = ue_index_t::invalid;
  srb_id_t                 srb_id       = srb_id_t::nulltype;
  asn1::rrc_nr::pdcp_cfg_s pdcp_cfg;
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

  /// \brief Notify about the need to delete a UE.
  virtual void on_ue_delete_request(const cause_t& cause) = 0;
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

  /// \brief Notify about the need to delete a UE.
  virtual void on_ue_delete_request(const cause_t& cause) = 0;

  /// \brief Setup AS security in the UE. This includes configuring
  /// the PDCP entity security on SRB1 with the new AS keys.
  virtual void on_new_as_security_context() = 0;
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

  /// \brief Notify about a DL DCCH message.
  /// \param[in] dl_dcch_msg The DL DCCH message.
  /// \param[in] ue_index The old index of the UE.
  virtual void
  on_new_dl_dcch(srb_id_t srb_id, const asn1::rrc_nr::dl_dcch_msg_s& dl_dcch_msg, ue_index_t old_ue_index) = 0;

  /// \brief Refresh AS security keys after horizontal key derivation.
  /// This includes configuring the PDCP entity security on SRB1 with the new AS keys.
  virtual void on_new_as_security_context() = 0;

  /// \brief Notify about the need to delete a UE.
  virtual void on_ue_delete_request(const cause_t& cause) = 0;
};

struct rrc_ue_context_release_command {
  ue_index_t         ue_index = ue_index_t::invalid;
  cause_t            cause    = cause_t::nulltype;
  byte_buffer        rrc_release_pdu;
  optional<srb_id_t> srb_id;
};

/// Interface to notify about RRC UE Context messages.
class rrc_ue_du_processor_notifier
{
public:
  virtual ~rrc_ue_du_processor_notifier() = default;

  /// \brief Notify about the need to create an SRB.
  /// \param[in] msg The SRB creation message.
  virtual void on_create_srb(const srb_creation_message& msg) = 0;

  /// \brief Notify about a UE Context Release Command.
  /// \param[in] cmd The UE Context Release Command.
  virtual void on_ue_context_release_command(const rrc_ue_context_release_command& cmd) = 0;

  /// \brief Notify about a required reestablishment context modification.
  /// \param[in] ue_index The index of the UE that needs the context modification.
  virtual async_task<bool> on_rrc_reestablishment_context_modification_required(ue_index_t ue_index) = 0;
};

/// Schedules asynchronous tasks associated with an UE.
class rrc_ue_task_scheduler
{
public:
  virtual ~rrc_ue_task_scheduler()                                   = default;
  virtual void          schedule_async_task(async_task<void>&& task) = 0;
  virtual unique_timer  make_unique_timer()                          = 0;
  virtual timer_factory get_timer_factory()                          = 0;
};

struct initial_ue_message {
  ue_index_t                             ue_index = ue_index_t::invalid;
  byte_buffer                            nas_pdu;
  rrc_cell_context                       cell;
  asn1::rrc_nr::establishment_cause_opts establishment_cause;
  optional<cu_cp_five_g_s_tmsi>          five_g_s_tmsi;
};

struct ul_nas_transport_message {
  ue_index_t       ue_index = ue_index_t::invalid;
  byte_buffer      nas_pdu;
  rrc_cell_context cell;
};

/// Interface to notify about NAS messages.
class rrc_ue_nas_notifier
{
public:
  virtual ~rrc_ue_nas_notifier() = default;

  /// \brief Notify about the Initial UE Message.
  /// \param[in] msg The initial UE message.
  virtual void on_initial_ue_message(const initial_ue_message& msg) = 0;

  /// \brief Notify about an Uplink NAS Transport message.
  /// \param[in] msg The Uplink NAS Transport message.
  virtual void on_ul_nas_transport_message(const ul_nas_transport_message& msg) = 0;
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

  virtual void on_ue_context_release_request(const cu_cp_ue_context_release_request& msg) = 0;
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
  /// \returns The release context of the UE.
  virtual rrc_ue_release_context get_rrc_ue_release_context() = 0;

  /// \brief Get the RRC measurement config for the current serving cell of the UE.
  /// \return The measurement config, if present.
  virtual optional<rrc_meas_cfg> get_rrc_ue_meas_config() = 0;
};

/// Handler to initialize the security context from NGAP.
class rrc_ue_init_security_context_handler
{
public:
  virtual ~rrc_ue_init_security_context_handler() = default;

  /// \brief Handle the received Downlink NAS Transport message.
  /// \param[in] msg The Downlink NAS Transport message.
  virtual async_task<bool> handle_init_security_context(const security::security_context& msg) = 0;
};

/// Handler to get the handover preparation context to the NGAP.
class rrc_ue_handover_preparation_handler
{
public:
  virtual ~rrc_ue_handover_preparation_handler() = default;

  virtual byte_buffer get_packed_handover_preparation_message() = 0;
};

/// Struct containing all information needed from the old RRC UE for Reestablishment.
struct rrc_reestablishment_ue_context_t {
  ue_index_t                          ue_index = ue_index_t::invalid;
  security::security_context          sec_context;
  optional<asn1::rrc_nr::ue_nr_cap_s> capabilities;
  up_context                          up_ctx;
};

/// Interface to notify about RRC Reestablishment Requests.
class rrc_ue_reestablishment_notifier
{
public:
  virtual ~rrc_ue_reestablishment_notifier() = default;

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
  virtual void on_ue_transfer_required(ue_index_t ue_index, ue_index_t old_ue_index) = 0;
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
  virtual rrc_ue_control_message_handler&       get_rrc_ue_control_message_handler()       = 0;
  virtual rrc_ue_init_security_context_handler& get_rrc_ue_init_security_context_handler() = 0;
  virtual security::security_context&           get_rrc_ue_security_context()              = 0;
  virtual rrc_ue_context_handler&               get_rrc_ue_context_handler()               = 0;
  virtual rrc_ue_handover_preparation_handler&  get_rrc_ue_handover_preparation_handler()  = 0;

  virtual void connect_srb_notifier(srb_id_t                  srb_id,
                                    rrc_pdu_notifier&         notifier,
                                    rrc_tx_security_notifier* tx_sec,
                                    rrc_rx_security_notifier* rx_sec) = 0;
};

} // namespace srs_cu_cp

} // namespace srsran
