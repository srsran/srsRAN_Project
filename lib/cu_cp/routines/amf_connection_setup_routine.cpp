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

#include "amf_connection_setup_routine.h"
#include "srsran/ngap/ngap_setup.h"

using namespace srsran;
using namespace srs_cu_cp;

amf_connection_setup_routine::amf_connection_setup_routine(const ngap_configuration&    ngap_config_,
                                                           cu_cp_ngap_control_notifier& ngap_ctrl_notifier_) :
  ngap_cfg(ngap_config_), ngap_ctrl_notifier(ngap_ctrl_notifier_)
{
}

void amf_connection_setup_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  // Initiate NG Setup.
  CORO_AWAIT_VALUE(result_msg, send_ng_setup_request());

  CORO_RETURN(variant_holds_alternative<ngap_ng_setup_response>(result_msg));
}

ngap_ng_setup_request amf_connection_setup_routine::fill_ng_setup_request()
{
  ngap_ng_setup_request request;

  // fill global ran node id
  request.global_ran_node_id.gnb_id  = ngap_cfg.gnb_id;
  request.global_ran_node_id.plmn_id = ngap_cfg.plmn;
  // fill ran node name
  request.ran_node_name = ngap_cfg.ran_node_name;
  // fill supported ta list
  // TODO: add support for more items
  ngap_supported_ta_item supported_ta_item;

  ngap_broadcast_plmn_item broadcast_plmn_item;
  broadcast_plmn_item.plmn_id = ngap_cfg.plmn;

  for (const auto& slice_config : ngap_cfg.slice_configurations) {
    slice_support_item_t slice_support_item;
    slice_support_item.s_nssai.sst = slice_config.sst;
    if (slice_config.sd.has_value()) {
      slice_support_item.s_nssai.sd = slice_config.sd.value();
    }
    broadcast_plmn_item.tai_slice_support_list.push_back(slice_support_item);
  }

  supported_ta_item.broadcast_plmn_list.push_back(broadcast_plmn_item);
  supported_ta_item.tac = ngap_cfg.tac;

  request.supported_ta_list.push_back(supported_ta_item);

  // fill paging drx
  request.default_paging_drx = 256;

  return request;
}

async_task<ngap_ng_setup_result> amf_connection_setup_routine::send_ng_setup_request()
{
  // Prepare request to send to ng.
  ngap_ng_setup_request request = fill_ng_setup_request();
  request.max_setup_retries     = 5;

  // Initiate NG Setup Request.
  return ngap_ctrl_notifier.on_ng_setup_request(request);
}
