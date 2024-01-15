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

#include "e2_ric_control_procedure.h"

using namespace srsran;
using namespace asn1::e2ap;
using namespace asn1::e2sm_rc;

e2_ric_control_procedure::e2_ric_control_procedure(const e2_ric_control_request& request_,
                                                   e2_message_notifier&          notif_,
                                                   e2sm_manager&                 e2sm_mng_,
                                                   srslog::basic_logger&         logger_) :
  logger(logger_), ric_notif(notif_), e2sm_mng(e2sm_mng_), e2_request(request_)
{
}

void e2_ric_control_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);
  e2sm_iface = e2sm_mng.get_e2sm_interface(e2_request.request->ra_nfunction_id.value);

  if (!e2sm_iface) {
    logger.error("RAN function ID not supported");
    CORO_EARLY_RETURN();
  }

  ric_ctrl_req    = e2sm_iface->get_e2sm_packer().handle_packed_ric_control_request(e2_request.request);
  control_service = e2sm_iface->get_e2sm_control_service(ric_ctrl_req);

  if (!control_service) {
    logger.error("RIC Control Service not supported");
    CORO_EARLY_RETURN();
  }

  if (!control_service->control_request_supported(ric_ctrl_req)) {
    logger.error("RIC Control Request not supported");
    CORO_EARLY_RETURN();
  }

  CORO_AWAIT_VALUE(e2sm_response, control_service->execute_control_request(ric_ctrl_req));
  if (ric_ctrl_req.ric_ctrl_ack_request_present and ric_ctrl_req.ric_ctrl_ack_request) {
    e2_response = e2sm_iface->get_e2sm_packer().pack_ric_control_response(e2sm_response);
    if (e2_response.success) {
      send_e2_ric_control_acknowledge(e2_request, e2_response);
    } else {
      send_e2_ric_control_failure(e2_request, e2_response);
    }
  }
  CORO_RETURN();
}

void e2_ric_control_procedure::send_e2_ric_control_acknowledge(const e2_ric_control_request&  ctrl_request,
                                                               const e2_ric_control_response& ctrl_response)
{
  e2_message msg;
  msg.pdu.set_successful_outcome();
  logger.info("Sending E2 RIC Control Acknowledge");
  msg.pdu.successful_outcome().load_info_obj(ASN1_E2AP_ID_RI_CCTRL);
  ri_cctrl_ack_s& ack              = msg.pdu.successful_outcome().value.ri_cctrl_ack();
  ack->ri_crequest_id              = ctrl_request.request->ri_crequest_id;
  ack->ra_nfunction_id             = ctrl_request.request->ra_nfunction_id;
  ack->ri_ccall_process_id_present = false;
  if (ctrl_request.request->ri_ccall_process_id_present) {
    ack->ri_ccall_process_id_present = true;
    ack->ri_ccall_process_id.value   = ctrl_request.request->ri_ccall_process_id.value;
  }
  ack->ri_cctrl_outcome_present = false;
  if (ctrl_response.ack->ri_cctrl_outcome_present) {
    ack->ri_cctrl_outcome_present = true;
    ack->ri_cctrl_outcome         = ctrl_response.ack->ri_cctrl_outcome;
  }
  ric_notif.on_new_message(msg);
}

void e2_ric_control_procedure::send_e2_ric_control_failure(const e2_ric_control_request&  ctrl_request,
                                                           const e2_ric_control_response& ctrl_response)
{
  e2_message msg;
  msg.pdu.set_unsuccessful_outcome();
  logger.info("Sending E2 RIC Control Failure");
  msg.pdu.unsuccessful_outcome().load_info_obj(ASN1_E2AP_ID_RI_CCTRL);
  ri_cctrl_fail_s& fail = msg.pdu.unsuccessful_outcome().value.ri_cctrl_fail();
  fail->ri_crequest_id  = ctrl_request.request->ri_crequest_id;
  fail->ra_nfunction_id = ctrl_request.request->ra_nfunction_id;
  if (ctrl_request.request->ri_ccall_process_id_present) {
    fail->ri_ccall_process_id_present = true;
    fail->ri_ccall_process_id.value   = ctrl_request.request->ri_ccall_process_id.value;
  }
  fail->cause                    = ctrl_response.failure->cause;
  fail->ri_cctrl_outcome_present = false;
  if (ctrl_response.ack->ri_cctrl_outcome_present) {
    fail->ri_cctrl_outcome_present = true;
    fail->ri_cctrl_outcome         = ctrl_response.ack->ri_cctrl_outcome;
  }
  ric_notif.on_new_message(msg);
}
