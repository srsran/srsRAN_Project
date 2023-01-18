/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "initial_cu_cp_setup_routine.h"
#include "../../ngap/ngap_asn1_helpers.h"
#include "srsgnb/asn1/ngap/ngap.h"

using namespace srsgnb;
using namespace srs_cu_cp;

initial_cu_cp_setup_routine::initial_cu_cp_setup_routine(const ngc_configuration&        ngc_config_,
                                                         cu_cp_ngap_control_notifier&    ngc_ctrl_notifier_,
                                                         ngap_cu_cp_connection_notifier& cu_cp_ngap_ev_notifier_) :
  ngc_cfg(ngc_config_), ngc_ctrl_notifier(ngc_ctrl_notifier_), cu_cp_ngap_ev_notifier(cu_cp_ngap_ev_notifier_)
{
}

void initial_cu_cp_setup_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  // Initiate NG Setup.
  CORO_AWAIT_VALUE(response_msg, send_ng_setup_request());

  // Handle NG setup result.
  if (response_msg.success) {
    // In case of NG Setup Response.
    handle_ng_setup_response(response_msg.msg);
  }

  // TODO process response

  CORO_RETURN();
}

async_task<ng_setup_response> initial_cu_cp_setup_routine::send_ng_setup_request()
{
  // Prepare request to send to ng.
  ng_setup_request request_msg = {};

  fill_asn1_ng_setup_request(request_msg.msg, ngc_cfg.gnb_id, ngc_cfg.ran_node_name, ngc_cfg.plmn, ngc_cfg.tac);

  // Initiate NG Setup Request.
  return ngc_ctrl_notifier.on_ng_setup_request(request_msg);
}

void initial_cu_cp_setup_routine::handle_ng_setup_response(const asn1::ngap::ng_setup_resp_s& resp)
{
  cu_cp_ngap_ev_notifier.on_amf_connection();
  // TODO
}
