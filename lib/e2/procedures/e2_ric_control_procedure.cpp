/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2_ric_control_procedure.h"

using namespace srsran;
using namespace asn1::e2ap;
using namespace asn1::e2sm_rc;

e2_ric_control_procedure::e2_ric_control_procedure(const e2_ric_control_request& request_,
                                                   e2_message_notifier&          notif_,
                                                   e2sm_manager&                 e2sm_mng_,
                                                   timer_factory                 timers_,
                                                   srslog::basic_logger&         logger_) :
  logger(logger_), ric_notif(notif_), e2sm_mng(e2sm_mng_), timers(timers_), request(request_)
{
}

void e2_ric_control_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  ri_cctrl_request_s ctrl_req   = request.request;
  e2sm_interface*    e2sm_iface = e2sm_mng.get_e2sm_interface(ctrl_req->ra_nfunction_id.value);
  CORO_BEGIN(ctx);
  ctrl_config_request = process_request();
  CORO_AWAIT_VALUE(ctrl_config_response,
                   e2sm_iface->get_param_configurator()->configure_scheduler(ctrl_config_request));
  if (compare_ric_control_configs(ctrl_config_request, ctrl_config_response)) {
    send_e2_ric_control_acknowledge(ctrl_config_request, ctrl_config_response);
  } else {
    send_e2_ric_control_failure(ctrl_config_request, ctrl_config_response);
  }
  CORO_RETURN();
}

ric_control_config e2_ric_control_procedure::process_request()
{
  ri_cctrl_request_s      ctrl_req = request.request;
  e2_sm_rc_ctrl_outcome_s outcome;
  e2sm_interface*         e2sm_iface = e2sm_mng.get_e2sm_interface(ctrl_req->ra_nfunction_id.value);
  if (!e2sm_iface) {
    logger.error("RAN function ID not supported");
    return {};
  }

  ric_control_config ctrl_config;

  if (!(ctrl_req->ri_cctrl_hdr.value.to_number() == 0)) {
    e2sm_iface->process_control_header(ctrl_req->ri_cctrl_hdr.value, ctrl_config);
  } else {
    logger.warning("Control header not present");
    ctrl_config.ue_id = 1;
  }
  e2sm_iface->process_control_message(ctrl_req->ri_cctrl_msg.value, ctrl_config);
  return ctrl_config;
}

void e2_ric_control_procedure::send_e2_ric_control_acknowledge(ric_control_config ctrl_request,
                                                               ric_control_config ctrl_response)
{
  e2_message msg;
  msg.pdu.set_successful_outcome();
  msg.pdu.successful_outcome().load_info_obj(ASN1_E2AP_ID_RI_CCTRL);
  ri_cctrl_ack_s& ack              = msg.pdu.successful_outcome().value.ri_cctrl_ack();
  ack->ra_nfunction_id             = request.request->ra_nfunction_id;
  ack->ri_crequest_id              = request.request->ri_crequest_id;
  ack->ri_ccall_process_id_present = false;
  ack->ri_cctrl_outcome_present    = false;
  ric_notif.on_new_message(msg);
}

void e2_ric_control_procedure::send_e2_ric_control_failure(ric_control_config ctrl_request,
                                                           ric_control_config ctrl_response)
{
  e2_message msg;
  msg.pdu.set_unsuccessful_outcome();
  msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_E2AP_ID_RI_CCTRL);
  ri_cctrl_fail_s& fail             = msg.pdu.unsuccessful_outcome().value.ri_cctrl_fail();
  fail->ra_nfunction_id             = request.request->ra_nfunction_id;
  fail->ri_crequest_id              = request.request->ri_crequest_id;
  fail->ri_ccall_process_id_present = false;
  fail->ri_cctrl_outcome_present    = false;
  ric_notif.on_new_message(msg);
}

bool e2_ric_control_procedure::compare_ric_control_configs(const ric_control_config& config1,
                                                           const ric_control_config& config2)
{
  if (config1.ue_id != config2.ue_id) {
    return false;
  }
  if (config1.num_harq_processes != config2.num_harq_processes) {
    return false;
  }
  if (config1.num_harq_retransmissions != config2.num_harq_retransmissions) {
    return false;
  }
  if (config1.max_prb_idx != config2.max_prb_idx) {
    return false;
  }
  if (config1.min_prb_idx != config2.min_prb_idx) {
    return false;
  }
  return true;
}
