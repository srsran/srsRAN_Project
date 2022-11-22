/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "initial_cu_cp_setup_procedure.h"
#include "../ngc_asn1_helpers.h"
#include "srsgnb/asn1/ngap/ngap.h"

using namespace srsgnb;
using namespace srs_cu_cp;

initial_cu_cp_setup_procedure::initial_cu_cp_setup_procedure(const ngc_configuration&       ngc_config_,
                                                             ngc_connection_manager&        ngc_conn_mng_,
                                                             cu_cp_ngc_connection_notifier& cu_cp_ngc_ev_notifier_) :
  ngc_cfg(ngc_config_), ngc_conn_mng(ngc_conn_mng_), cu_cp_ngc_ev_notifier(cu_cp_ngc_ev_notifier_)
{
}

void initial_cu_cp_setup_procedure::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Initiate NG Setup.
  CORO_AWAIT_VALUE(response_msg, start_ng_setup_request());

  // Handle NG setup result.
  if (response_msg.success) {
    // In case of NG Setup Response.
    handle_ng_setup_response(response_msg.msg);
  }

  // TODO process response

  CORO_RETURN();
}

async_task<ng_setup_response_message> initial_cu_cp_setup_procedure::start_ng_setup_request()
{
  // Prepare request to send to ng.
  ng_setup_request_message request_msg = {};

  fill_asn1_ng_setup_request(request_msg.msg, ngc_cfg.gnb_id, ngc_cfg.ran_node_name, ngc_cfg.plmn_id, ngc_cfg.tac);

  // Initiate NG Setup Request.
  return ngc_conn_mng.handle_ng_setup_request(request_msg);
}

void initial_cu_cp_setup_procedure::handle_ng_setup_response(const asn1::ngap::ng_setup_resp_s& resp)
{
  cu_cp_ngc_ev_notifier.on_amf_connection();
  // TODO
}
