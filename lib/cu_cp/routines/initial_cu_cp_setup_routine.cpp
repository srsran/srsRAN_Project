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

#include "initial_cu_cp_setup_routine.h"
#include "../../ngap/ngap_asn1_helpers.h"

using namespace srsran;
using namespace srs_cu_cp;

initial_cu_cp_setup_routine::initial_cu_cp_setup_routine(const ngap_configuration&       ngap_config_,
                                                         cu_cp_ngap_control_notifier&    ngap_ctrl_notifier_,
                                                         ngap_cu_cp_connection_notifier& cu_cp_ngap_ev_notifier_) :
  ngap_cfg(ngap_config_), ngap_ctrl_notifier(ngap_ctrl_notifier_), cu_cp_ngap_ev_notifier(cu_cp_ngap_ev_notifier_)
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

  fill_asn1_ng_setup_request(request_msg.msg, ngap_cfg);

  // Initiate NG Setup Request.
  return ngap_ctrl_notifier.on_ng_setup_request(request_msg);
}

void initial_cu_cp_setup_routine::handle_ng_setup_response(const asn1::ngap::ng_setup_resp_s& resp)
{
  cu_cp_ngap_ev_notifier.on_amf_connection();
  // TODO
}
