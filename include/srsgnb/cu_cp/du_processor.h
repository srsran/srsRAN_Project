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
#include "du_processor_context.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/e1/cu_cp/e1ap_cu_cp_bearer_context_update.h"
#include "srsgnb/f1ap/cu_cp/f1ap_cu.h"
#include "srsgnb/ran/nr_cgi.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/rrc/rrc.h"
#include "srsgnb/rrc/rrc_config.h"
#include "srsgnb/rrc/rrc_du.h"
#include "srsgnb/support/timers.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

/// Forward declared messages.
struct f1_setup_request_message;
struct rrc_ue_creation_message;

/// Interface to request SRB creations at the DU processor.
class du_processor_srb_interface
{
public:
  virtual ~du_processor_srb_interface() = default;

  /// \brief Instruct the DU processor to create a new SRB for a given UE. Depending on the config it creates all
  /// required intermediate objects (e.g. PDCP) and connects them with one another.
  /// \param[in] msg The UE index, SRB ID and config.
  virtual void create_srb(const srb_creation_message& msg) = 0;
};

struct ue_creation_message {
  nr_cell_global_id_t             cgi;
  uint32_t                        tac;
  asn1::unbounded_octstring<true> du_to_cu_rrc_container;
  rnti_t                          c_rnti;
};

/// Interface for an F1C notifier to communicate with the DU processor.
class du_processor_f1c_interface : public du_processor_srb_interface
{
public:
  virtual ~du_processor_f1c_interface() = default;

  /// \brief Get the DU index.
  /// \return The DU index.
  virtual du_index_t get_du_index() = 0;

  /// \brief Get the DU processor context.
  /// \return The DU processor context.
  virtual du_processor_context& get_context() = 0;

  /// \brief Handle the reception of a F1 Setup Request message and transmit the F1 Setup Response or F1 Setup Failure.
  /// \param[in] msg The received F1 Setup Request message.
  virtual void handle_f1_setup_request(const f1_setup_request_message& msg) = 0;

  /// \brief Create a new UE context.
  /// \param[in] msg The UE creation message.
  /// \return Returns a UE creation complete message containing the index of the created UE and its SRB notifiers.
  virtual ue_creation_complete_message handle_ue_creation_request(const ue_creation_message& msg) = 0;

  /// \brief Get the F1C message handler interface of the DU processor object.
  /// \return The F1C message handler interface of the DU processor object.
  virtual f1c_message_handler& get_f1c_message_handler() = 0;

  /// \brief Get the F1C UE context management handler interface of the DU processor object.
  /// \return The F1C UE context management handler interface of the DU processor object.
  virtual f1c_ue_context_manager& get_f1c_ue_context_manager() = 0;

  /// \brief Get the F1C statistics handler interface of the DU processor object.
  /// \return The F1C statistics handler interface of the DU processor object.
  virtual f1c_statistics_handler& get_f1c_statistics_handler() = 0;
};

/// Interface to notifiy UE context management procedures.
class du_processor_f1ap_ue_context_notifier
{
public:
  virtual ~du_processor_f1ap_ue_context_notifier() = default;

  /// Notify F1AP to establish the UE context.
  virtual async_task<f1ap_ue_context_setup_response>
  on_ue_context_setup_request(const f1ap_ue_context_setup_request& request) = 0;

  /// \brief Notify the F1AP to initiate the UE Context Release procedure.
  /// \param[in] msg The UE Context Release message to transmit.
  /// \return Returns the index of the released UE.
  virtual async_task<ue_index_t> on_ue_context_release_command(const f1ap_ue_context_release_command& msg) = 0;

  /// \brief Notify the F1AP to initiate the UE Context Modification procedure.
  /// \param[in] request The UE Context Modification message to transmit.
  /// \return Returns a cu_cp_ue_context_modification_response_message struct with the success member set to
  /// 'true' in case of a successful outcome, 'false' otherwise.
  virtual async_task<cu_cp_ue_context_modification_response>
  on_ue_context_modification_request(const cu_cp_ue_context_modification_request& request) = 0;
};

/// Interface for an RRC entity to communicate with the DU processor.
class du_processor_rrc_interface
{
public:
  virtual ~du_processor_rrc_interface() = default;

  /// \brief Get the RRC AMF connection handler interface of the DU processor object.
  /// \return The RRC AMF connection handler interface of the DU processor object.
  virtual rrc_amf_connection_handler& get_rrc_amf_connection_handler() = 0;
};

/// Interface to notifiy RRC DU about UE management procedures.
class du_processor_rrc_du_ue_notifier
{
public:
  virtual ~du_processor_rrc_du_ue_notifier() = default;

  /// \brief Notify RRC DU to create a UE.
  /// \param[in] msg The UE creation message.
  /// \return Returns a handle to the created UE.
  virtual rrc_ue_interface* on_ue_creation_request(rrc_ue_creation_message msg) = 0;

  /// \brief Notify the RRC DU to release a UE.
  /// \param[in] ue_index The index of the UE object to remove.
  virtual void on_ue_context_release_command(ue_index_t ue_index) = 0;

  /// Send RRC Release to all UEs connected to this DU.
  virtual void on_release_ues() = 0;
};

/// Interface for an RRC UE entity to communicate with the DU processor.
class du_processor_rrc_ue_interface : public du_processor_srb_interface
{
public:
  virtual ~du_processor_rrc_ue_interface() = default;

  /// \brief Handle a UE Context Release Command
  /// \param[in] cmd The UE Context Release Command.
  virtual void handle_ue_context_release_command(const cu_cp_ue_context_release_command& cmd) = 0;
};

/// Interface to notify an RRC UE about control messages.
class du_processor_rrc_ue_control_message_notifier
{
public:
  virtual ~du_processor_rrc_ue_control_message_notifier() = default;

  /// \brief Notify the RRC UE about an update of the GUAMI.
  /// \param[in] msg The new GUAMI.
  virtual void on_new_guami(const guami& msg) = 0;

  /// \brief Notify the RRC UE about an RRC Reconfiguration Request.
  /// \param[in] msg The new RRC Reconfiguration Request.
  /// \returns The result of the rrc reconfiguration.
  virtual async_task<bool> on_rrc_reconfiguration_request(const cu_cp_rrc_reconfiguration_procedure_request& msg) = 0;

  /// \brief Notify the RRC UE to Release an UE.
  virtual void on_rrc_ue_release() = 0;
};

/// Handler for an NGAP entity to communicate with the DU processor
class du_processor_ngap_interface
{
public:
  virtual ~du_processor_ngap_interface() = default;

  /// \brief Handle the reception of a new PDU Session Resource Setup List.
  virtual async_task<cu_cp_pdu_session_resource_setup_response>
  handle_new_pdu_session_resource_setup_request(const cu_cp_pdu_session_resource_setup_request& msg) = 0;

  /// \brief Handle a UE Context Release Command.
  /// \param[in] cmd The UE Context Release Command.
  virtual void handle_new_ue_context_release_command(const cu_cp_ue_context_release_command& cmd) = 0;
};

/// Interface to notify the E1AP about control messages.
class du_processor_e1ap_control_notifier
{
public:
  virtual ~du_processor_e1ap_control_notifier() = default;

  /// \brief Notify about the reception of a new Bearer Context Setup Request.
  virtual async_task<e1ap_bearer_context_setup_response>
  on_bearer_context_setup_request(const e1ap_bearer_context_setup_request& request) = 0;

  /// \brief Notify about the reception of a new Bearer Context Modification Request.
  virtual async_task<e1ap_bearer_context_modification_response>
  on_bearer_context_modification_request(const e1ap_bearer_context_modification_request& request) = 0;

  /// \brief Notify about the reception of a new Bearer Context Release Command.
  virtual async_task<void> on_bearer_context_release_command(const e1ap_bearer_context_release_command& cmd) = 0;
};

/// Interface to notify the F1AP about control messages.
class du_processor_f1c_control_notifier
{
public:
  virtual ~du_processor_f1c_control_notifier() = default;

  /// \brief Notify about the reception of a new PDU Session Resource Setup List.
  virtual async_task<cu_cp_ue_context_modification_response>
  on_new_pdu_session_resource_setup_request(cu_cp_ue_context_modification_request& msg) = 0;
};

/// \brief Schedules asynchronous tasks associated with an UE.
class du_processor_ue_task_scheduler
{
public:
  virtual ~du_processor_ue_task_scheduler() = default;
  virtual void           schedule_async_task(du_index_t du_index, ue_index_t ue_index, async_task<void>&& task) = 0;
  virtual unique_timer   make_unique_timer()                                                                    = 0;
  virtual timer_manager& get_timer_manager()                                                                    = 0;
};

/// \brief Handles incoming task scheduling requests associated with an UE.
class du_processor_ue_task_handler
{
public:
  virtual ~du_processor_ue_task_handler()                                                   = default;
  virtual void           handle_ue_async_task(ue_index_t ue_index, async_task<void>&& task) = 0;
  virtual unique_timer   make_unique_timer()                                                = 0;
  virtual timer_manager& get_timer_manager()                                                = 0;
};

/// Methods used by DU processor to notify about DU specific events.
class du_processor_cu_cp_notifier
{
public:
  virtual ~du_processor_cu_cp_notifier() = default;

  /// \brief Notifies the CU-CP about a new DU connection.
  virtual void on_new_du_connection() = 0;

  /// \brief Notifies about a successful RRC UE creation.
  /// \param[in] du_index The index of the DU the UE is connected to.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] rrc_ue_msg_handler The created RRC UE.
  virtual void on_rrc_ue_created(du_index_t du_index, ue_index_t ue_index, rrc_ue_interface* rrc_ue) = 0;
};

/// Methods to get statistics of the DU processor.
class du_processor_statistics_handler
{
public:
  virtual ~du_processor_statistics_handler() = default;

  /// \brief Returns the number of connected UEs at the DU processor
  /// \return The number of connected UEs.
  virtual size_t get_nof_ues() = 0;
};

class du_processor_interface : public du_processor_f1c_interface,
                               public du_processor_rrc_interface,
                               public du_processor_rrc_ue_interface,
                               public du_processor_ngap_interface,
                               public du_processor_ue_task_handler,
                               public du_processor_statistics_handler

{
public:
  virtual ~du_processor_interface() = default;
};

} // namespace srs_cu_cp
} // namespace srsgnb
