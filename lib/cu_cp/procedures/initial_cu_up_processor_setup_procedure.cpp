/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "initial_cu_up_processor_setup_procedure.h"
#include "srsgnb/asn1/e1ap/e1ap.h"

using namespace srsgnb;
using namespace srs_cu_cp;

initial_cu_up_processor_setup_procedure::initial_cu_up_processor_setup_procedure(
    cu_up_processor_context&                   context_,
    e1_connection_manager&                     e1_conn_mng_,
    cu_up_processor_cu_up_management_notifier& cu_cp_notifier_) :
  context(context_), e1_conn_mng(e1_conn_mng_), cu_cp_notifier(cu_cp_notifier_)
{
}

void initial_cu_up_processor_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Initiate CU-CP E1 Setup.
  CORO_AWAIT_VALUE(response_msg, start_cu_cp_e1_setup_request());

  // Handle CU-CP E1 setup result.
  if (response_msg.success) {
    // In case of CU-CP E1 Setup Response.
    handle_cu_cp_e1_setup_response(response_msg.response);
  } else {
    cu_cp_notifier.on_cu_up_remove_request_received(context.cu_up_index);
  }

  CORO_RETURN();
}

async_task<cu_cp_e1_setup_response_message> initial_cu_up_processor_setup_procedure::start_cu_cp_e1_setup_request()
{
  // Prepare request to send to CU-CP E1 Setup Request message.
  cu_cp_e1_setup_request_message request_msg  = {};
  request_msg.request->gnb_cu_cp_name_present = true;
  request_msg.request->gnb_cu_cp_name.value.from_string(context.cu_cp_name);

  // Initiate CU-CP E1 Setup Request.
  return e1_conn_mng.handle_cu_cp_e1_setup_request(request_msg);
}

void initial_cu_up_processor_setup_procedure::handle_cu_cp_e1_setup_response(
    const asn1::e1ap::gnb_cu_cp_e1_setup_resp_s& resp)
{
  if (resp->gnb_cu_up_name_present) {
    context.cu_up_name = resp->gnb_cu_up_name.value.to_string();
  }
  context.id = resp->gnb_cu_up_id.value;

  // TODO: handle response
}
