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

#include "ngap_types.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {

namespace srs_cu_cp {

struct ngap_message {
  asn1::ngap::ngap_pdu_c pdu;
};

/// This interface is used to push NGAP messages to the NGAP interface.
class ngap_message_handler
{
public:
  virtual ~ngap_message_handler() = default;

  /// Handle the incoming NGAP message.
  virtual void handle_message(const ngap_message& msg) = 0;
};

/// Interface used by NGAP to inform about events.
class ngap_event_handler
{
public:
  virtual ~ngap_event_handler()         = default;
  virtual void handle_connection_loss() = 0;
};

/// This interface notifies the reception of new NGAP messages over the NGAP interface.
class ngap_message_notifier
{
public:
  virtual ~ngap_message_notifier() = default;

  /// This callback is invoked on each received NGAP message.
  virtual void on_new_message(const ngap_message& msg) = 0;
};

struct ng_setup_request {
  asn1::ngap::ng_setup_request_s msg;
  unsigned                       max_setup_retries = 5;
};

struct ng_setup_response {
  asn1::ngap::ng_setup_resp_s msg;
  bool                        success = false;
};

/// Handle NGAP interface management procedures as defined in TS 38.413 section 8.7
class ngap_connection_manager
{
public:
  virtual ~ngap_connection_manager() = default;

  /// \brief Initiates the NG Setup procedure.
  /// \param[in] request The NGSetupRequest message to transmit.
  /// \return Returns a ng_setup_response struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise.
  /// \remark The CU transmits the NGSetupRequest as per TS 38.413 section 8.7.1
  /// and awaits the response. If a NGSetupFailure is received the NGAP will handle the failure.
  virtual async_task<ng_setup_response> handle_ng_setup_request(const ng_setup_request& request) = 0;
};

/// Interface to notify about NGAP connections to the CU-CP
class ngap_cu_cp_connection_notifier
{
public:
  virtual ~ngap_cu_cp_connection_notifier() = default;

  /// \brief Notifies the CU-CP about a successful AMF connection.
  virtual void on_amf_connection() = 0;

  /// \brief Notifies the CU-CP about a dropped AMF connection.
  virtual void on_amf_connection_drop() = 0;
};

struct ngap_initial_ue_message {
  ue_index_t                               ue_index = ue_index_t::invalid;
  byte_buffer                              nas_pdu;
  asn1::ngap::rrc_establishment_cause_opts establishment_cause;
  asn1::ngap::nr_cgi_s                     nr_cgi;
  uint32_t                                 tac;
};

struct ngap_ul_nas_transport_message {
  ue_index_t           ue_index = ue_index_t::invalid;
  byte_buffer          nas_pdu;
  asn1::ngap::nr_cgi_s nr_cgi;
  uint32_t             tac;
};

/// Handle NGAP NAS Message procedures as defined in TS 38.413 section 8.6.
class ngap_nas_message_handler
{
public:
  virtual ~ngap_nas_message_handler() = default;

  /// \brief Initiates Initial UE message procedure as per TS 38.413 section 8.6.1.
  /// \param[in] msg The initial UE message to transmit.
  virtual void handle_initial_ue_message(const ngap_initial_ue_message& msg) = 0;

  /// \brief Initiates Uplink NAS transport procedure as per TS 38.413 section 8.6.3.
  /// \param[in] msg The ul nas transfer message to transmit.
  virtual void handle_ul_nas_transport_message(const ngap_ul_nas_transport_message& msg) = 0;
};

struct ngap_pdu_session_res_item {
  pdu_session_id_t pdu_session_id = pdu_session_id_t::invalid;
  byte_buffer      pdu_session_res;
};
using ngap_pdu_session_res_list = std::vector<ngap_pdu_session_res_item>;

/// Interface to notify about NAS PDUs and messages.
class ngap_rrc_ue_pdu_notifier
{
public:
  virtual ~ngap_rrc_ue_pdu_notifier() = default;

  /// \brief Notify about the a new nas pdu.
  /// \param [in] nas_pdu The nas pdu.
  virtual void on_new_pdu(byte_buffer nas_pdu) = 0;
};

/// Interface to notify the RRC UE about control messages.
class ngap_rrc_ue_control_notifier
{
public:
  virtual ~ngap_rrc_ue_control_notifier() = default;

  /// \brief Notify about the reception of new security capabilities and key.
  virtual async_task<bool> on_new_security_context(const asn1::ngap::ue_security_cap_s&           caps,
                                                   const asn1::fixed_bitstring<256, false, true>& key) = 0;
};

/// Interface to notify the DU Processor about control messages.
class ngap_du_processor_control_notifier
{
public:
  virtual ~ngap_du_processor_control_notifier() = default;

  /// \brief Notify about the reception of a new PDU Session Resource Setup Request.
  virtual async_task<cu_cp_pdu_session_resource_setup_response>
  on_new_pdu_session_resource_setup_request(cu_cp_pdu_session_resource_setup_request& request) = 0;

  /// \brief Notify about the reception of a new UE Context Release Command.
  virtual void on_new_ue_context_release_command(cu_cp_ue_context_release_command& command) = 0;
};

/// Interface to control the NGAP.
class ngap_ue_control_manager
{
public:
  virtual ~ngap_ue_control_manager() = default;

  /// \brief Creates a NGAP UE.
  /// \param[in] ue_index The index of the UE.
  /// \param[in] rrc_ue_pdu_notifier The pdu notifier to the RRC UE.
  /// \param[in] rrc_ue_ctrl_notifier The control notifier to the RRC UE.
  /// \param[in] du_processor_ctrl_notifier The pdu notifier to the DU processor.
  virtual void create_ngap_ue(ue_index_t                          ue_index,
                              ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier,
                              ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier,
                              ngap_du_processor_control_notifier& du_processor_ctrl_notifier) = 0;
};

/// \brief Schedules asynchronous tasks associated with an UE.
class ngap_ue_task_scheduler
{
public:
  virtual ~ngap_ue_task_scheduler()                                                        = default;
  virtual void           schedule_async_task(ue_index_t ue_index, async_task<void>&& task) = 0;
  virtual unique_timer   make_unique_timer()                                               = 0;
  virtual timer_manager& get_timer_manager()                                               = 0;
};

/// \brief Interface to query statistics from the NGAP interface.
class ngap_statistic_interface
{
public:
  virtual ~ngap_statistic_interface() = default;

  /// \brief Get the number of UEs registered at the NGAP.
  /// \return The number of UEs.
  virtual size_t get_nof_ues() const = 0;
};

/// Combined entry point for the NGAP object.
class ngap_interface : public ngap_message_handler,
                       public ngap_event_handler,
                       public ngap_connection_manager,
                       public ngap_nas_message_handler,
                       public ngap_ue_control_manager,
                       public ngap_statistic_interface
{
public:
  virtual ~ngap_interface() = default;
};

} // namespace srs_cu_cp

} // namespace srsgnb