/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1AP_CU_IMPL_H
#define SRSGNB_F1AP_CU_IMPL_H

#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/support/async/async_queue.h"
#include <memory>

namespace srsgnb {
namespace srs_cu_cp {

class f1ap_cu_impl final : public f1_interface
{
public:
  f1ap_cu_impl(f1c_message_notifier& f1c_pdu_notifier_, f1c_initiating_message_notifier& f1c_init_message_notifier_);
  ~f1ap_cu_impl();

  // f1ap connection manager functions

  void handle_f1ap_setup_response(const f1_setup_response_message& msg) override;

  // f1ap rrc message transfer procedure functions

  void handle_dl_rrc_message_transfer(const dl_rrc_message_transfer_message& msg) override;

  // f1ap ue context manager functions

  async_task<f1ap_ue_context_setup_response_message>
  handle_ue_context_setup_request(const f1ap_ue_context_setup_request_message& request) override;

  async_task<f1ap_ue_context_release_complete_message>
  handle_ue_context_release(const f1ap_ue_context_release_command_message& msg) override;

  async_task<f1ap_ue_context_modification_response_message>
  handle_ue_context_modification(const f1ap_ue_context_modification_request_message& request) override;

  // f1c message handler functions

  void handle_message(const asn1::f1ap::f1_ap_pdu_c& msg) override;

  void handle_connection_loss() override {}

private:
  class f1ap_event_manager;

  void handle_initiating_message(const asn1::f1ap::init_msg_s& msg);

  srslog::basic_logger&            logger;
  f1c_message_notifier&            pdu_notifier;
  f1c_initiating_message_notifier& init_message_notifier;

  std::unique_ptr<f1ap_event_manager> events;
};

} // namespace srs_cu_cp
} // namespace srsgnb

#endif // SRSGNB_F1AP_CU_IMPL_H
