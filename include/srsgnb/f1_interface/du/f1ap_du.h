
#pragma once

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/common/f1c_common.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/timers.h"

namespace srsgnb {
namespace srs_du {

struct f1ap_ue_create_request {
  du_ue_index_t ue_index;
};

struct f1ap_ue_create_response {
  bool result;
};

struct f1_rx_pdu {
  du_ue_index_t     ue_index;
  lcid_t            lcid;
  byte_buffer_slice pdu;
};

struct f1ap_initial_ul_rrc_msg {
  asn1::f1ap::init_ulrrc_msg_transfer_s init_msg;
};

struct f1ap_ul_rrc_msg {
  asn1::f1ap::ulrrc_msg_transfer_s ul_msg;
};

struct f1ap_rrc_delivery_report_msg {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  lcid_t          lcid;
  bool            rrc_delivery_status;
};

class f1ap_rrc_message_transfer_procedure_handler
{
public:
  virtual ~f1ap_rrc_message_transfer_procedure_handler() = default;

  /// \brief Packs and transmits the initial UL RRC message transfer as per TS 38.473 section 8.4.1.
  /// \param[in] msg The initial UL RRC message transfer message to transmit.
  virtual void handle_init_ul_rrc_message_transfer(const f1ap_initial_ul_rrc_msg& msg) = 0;

  /// \brief Packs and transmits the UL RRC message transfer as per TS 38.473 section 8.4.3.
  /// \param[in] msg The UL RRC message transfer message to transmit.
  virtual void handle_ul_rrc_message_transfer(const f1ap_ul_rrc_msg& msg) = 0;

  /// \brief Packs and transmits the RRC delivery report as per TS 38.473 section 8.4.4.
  /// \param[in] report The RRC delivery report message to transmit.
  virtual void handle_rrc_delivery_report(const f1ap_rrc_delivery_report_msg& report) = 0;

  //: TODO: remove this in favour of f1c_message_handler
  virtual void handle_pdu(f1_rx_pdu pdu) = 0;
};

struct du_setup_params {
  // mandatory
  uint64_t gnb_du_id;
  uint8_t  rrc_version;

  // optional
  std::string gnb_du_name;
};

struct f1_setup_request_message {
  asn1::f1ap::f1_setup_request_s msg;
  unsigned                       max_setup_retries = 5;
};

struct f1_setup_response_message {
  asn1::f1ap::f1_setup_resp_s msg;
  bool                        success;
};

/// Handle F1AP interface management procedures as defined in TS 38.473 section 8.2.
class f1ap_connection_manager
{
public:
  virtual ~f1ap_connection_manager() = default;

  /// \brief Initiates the F1 Setup procedure.
  /// \param[in] request The F1SetupRequest message to transmit.
  /// \return Returns a f1_setup_response_message struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise. \remark The DU transmits the F1SetupRequest as per TS 38.473 section 8.2.3
  /// and awaits the response. If a F1SetupFailure is received the F1AP will handle the failure.
  virtual async_task<f1_setup_response_message> handle_f1ap_setup_request(const f1_setup_request_message& request) = 0;
};

struct f1ap_ue_context_release_request_message {
  asn1::f1ap::ue_context_release_request_s msg;
};

struct f1ap_ue_context_modification_required_message {
  asn1::f1ap::ue_context_mod_required_s msg;
};

struct f1ap_ue_context_modification_response_message {
  asn1::f1ap::ue_context_mod_confirm_s confirm;
  asn1::f1ap::ue_context_mod_refuse_s  refuse;
  bool                                 success;
};

struct f1ap_ue_inactivity_notification_message {
  asn1::f1ap::ue_inactivity_notif_s msg;
};

struct f1ap_notify_message {
  asn1::f1ap::notify_s msg;
};

/// Handle F1AP UE context management procedures as defined in TS 38.473 section 8.3.
class f1ap_ue_context_manager
{
public:
  virtual ~f1ap_ue_context_manager() = default;

  /// Initiates creation of UE context in F1.
  virtual async_task<f1ap_ue_create_response> handle_ue_creation_request(const f1ap_ue_create_request& msg) = 0;

  /// \brief Initiates UE context release request procedure as per TS 38.473 section 8.3.2.
  /// \param[in] request The UE Context Release Request message to transmit.
  virtual void handle_ue_context_release_request(const f1ap_ue_context_release_request_message& request) = 0;

  /// \brief Initiates the UE Context Modification Required procedure as per TS 38.473 section 8.3.5.
  /// \param[in] msg The UE Context Modification Required message to transmit.
  /// \return Returns a f1ap_ue_context_modification_response_message struct with the success member set to 'true' in
  /// case of a successful outcome, 'false' otherwise.
  virtual async_task<f1ap_ue_context_modification_response_message>
  handle_ue_context_modification(const f1ap_ue_context_modification_required_message& msg) = 0;

  /// \brief Indicate an UE activity event as per TS 38.473 section 8.3.6
  /// \param[in] msg The UE Inactivity Nofication message to transmit.
  virtual void handle_ue_inactivity_notification(const f1ap_ue_inactivity_notification_message& msg) = 0;

  /// \brief Initiate the Notify procedure as per TS 38.473 section 8.3.7
  /// \param[in] msg The Notify message to transmit.
  virtual void handle_notify(const f1ap_notify_message& msg) = 0;
};

/// Handle scheduled tasks relative to a given UE.
class f1c_ue_task_scheduler
{
public:
  virtual ~f1c_ue_task_scheduler() = default;

  /// \brief Create timer for a given UE.
  virtual unique_timer create_timer() = 0;

  /// \brief Schedule Async Task respective to a given UE.
  virtual void schedule_async_task(async_task<void>&& task) = 0;
};

/// Class to manage scheduling of asynchronous F1c tasks and timers.
class f1c_task_scheduler
{
public:
  virtual ~f1c_task_scheduler() = default;

  virtual timer_manager& get_timer_manager() = 0;

  /// \brief Retrieve task scheduler specific to a given UE.
  virtual f1c_ue_task_scheduler& get_ue_task_scheduler(du_ue_index_t ue_index) = 0;
};

/// Combined entry point for F1C/U handling.
class f1_interface : public f1c_message_handler,
                     public f1c_event_handler,
                     public f1ap_rrc_message_transfer_procedure_handler,
                     public f1ap_connection_manager,
                     public f1ap_ue_context_manager
{
public:
  virtual ~f1_interface() = default;
};

} // namespace srs_du
} // namespace srsgnb
