/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1AP_DU_IMPL_H
#define SRSGNB_F1AP_DU_IMPL_H

#include "f1ap_du_context.h"
#include "handlers/f1c_du_packet_handler.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/support/timers.h"
#include <memory>

namespace srsgnb {
namespace srs_du {

class f1ap_du_impl final : public f1_interface
{
public:
  f1ap_du_impl(timer_manager& timers_, f1c_message_handler& f1c_handler);
  ~f1ap_du_impl();

  async_task<f1_setup_response_message> handle_f1ap_setup_request(const f1_setup_request_message& request) override;

  async_task<f1ap_ue_create_response> handle_ue_creation_request(const f1ap_ue_create_request& msg) override;

  void ul_rrc_message_delivery_report(const ul_rrc_message_delivery_status& report) override {}

  void handle_pdu(f1_rx_pdu pdu) override;

  void handle_message(const asn1::f1ap::f1_ap_pdu_c& msg) override;

  void handle_connection_loss() override {}

  async_task<f1_setup_response_message> handle_f1_setup_failure(const f1_setup_request_message&    request,
                                                                const asn1::f1ap::f1_setup_fail_s& failure);

private:
  class f1ap_du_event_manager;

  void handle_initiating_message(const asn1::f1ap::init_msg_s& msg);
  void handle_successful_outcome(const asn1::f1ap::successful_outcome_s& outcome);
  void handle_unsuccessful_outcome(const asn1::f1ap::unsuccessful_outcome_s& outcome);

  srslog::basic_logger& logger;
  timer_manager&        timers;
  f1c_message_handler&  f1c;

  unique_timer f1c_setup_timer;

  f1ap_du_context ctxt;

  std::unique_ptr<f1ap_du_event_manager> events;

  int f1_setup_retry_no = 0;
};

} // namespace srs_du
} // namespace srsgnb

#endif // SRSGNB_F1AP_DU_IMPL_H
