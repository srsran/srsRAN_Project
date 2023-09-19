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

#include "e2_ric_control_procedure.h"

using namespace srsran;
using namespace asn1::e2ap;
using namespace asn1::e2sm_rc;

e2_ric_control_procedure::e2_ric_control_procedure(const e2_ric_control_request& request_,
                                                   e2_message_notifier&          notif_,
                                                   e2sm_manager&                 e2sm_mng_,
                                                   srslog::basic_logger&         logger_) :
  logger(logger_), ric_notif(notif_), e2sm_mng(e2sm_mng_), request(request_)
{
}

void e2_ric_control_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  ri_cctrl_request_s ctrl_req   = request.request;
  e2sm_interface*    e2sm_iface = e2sm_mng.get_e2sm_interface(ctrl_req->ra_nfunction_id.value);
  CORO_BEGIN(ctx);
  ctrl_config_request = process_request();
  CORO_AWAIT_VALUE(ctrl_config_response,
                   e2sm_iface->get_param_configurator()->configure_ue_mac_scheduler(ctrl_config_request));
  if (ctrl_config_response.harq_processes_result and ctrl_config_response.max_prb_alloc_result and
      ctrl_config_response.min_prb_alloc_result) {
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

void e2_ric_control_procedure::send_e2_ric_control_acknowledge(ric_control_config          ctrl_request,
                                                               ric_control_config_response ctrl_response)
{
  e2_message msg;
  msg.pdu.set_successful_outcome();
  logger.info("Sending E2 RIC Control Acknowledge");
  msg.pdu.successful_outcome().load_info_obj(ASN1_E2AP_ID_RI_CCTRL);
  ri_cctrl_ack_s& ack              = msg.pdu.successful_outcome().value.ri_cctrl_ack();
  ack->ra_nfunction_id             = request.request->ra_nfunction_id;
  ack->ri_crequest_id              = request.request->ri_crequest_id;
  ack->ri_ccall_process_id_present = false;
  ack->ri_cctrl_outcome_present    = false;
  ric_notif.on_new_message(msg);
}

void e2_ric_control_procedure::send_e2_ric_control_failure(ric_control_config          ctrl_request,
                                                           ric_control_config_response ctrl_response)
{
  e2_message msg;
  msg.pdu.set_unsuccessful_outcome();
  logger.info("Sending E2 RIC Control Failure");
  msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_E2AP_ID_RI_CCTRL);
  ri_cctrl_fail_s& fail             = msg.pdu.unsuccessful_outcome().value.ri_cctrl_fail();
  fail->ra_nfunction_id             = request.request->ra_nfunction_id;
  fail->ri_crequest_id              = request.request->ri_crequest_id;
  fail->ri_ccall_process_id_present = false;
  fail->ri_cctrl_outcome_present    = false;
  ric_notif.on_new_message(msg);
}
