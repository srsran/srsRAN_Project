/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "amf_connection_setup_routine.h"
#include "srsran/ngap/ngap_setup.h"

using namespace srsran;
using namespace srs_cu_cp;

amf_connection_setup_routine::amf_connection_setup_routine(const cu_cp_configuration& cu_cp_cfg_,
                                                           ngap_connection_manager&   ngap_conn_mng_) :
  cu_cp_cfg(cu_cp_cfg_), ngap_conn_mng(ngap_conn_mng_)
{
}

void amf_connection_setup_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  if (not ngap_conn_mng.handle_amf_tnl_connection_request()) {
    CORO_EARLY_RETURN(false);
  }

  // Initiate NG Setup.
  CORO_AWAIT_VALUE(result_msg, send_ng_setup_request());

  CORO_RETURN(std::holds_alternative<ngap_ng_setup_response>(result_msg));
}

ngap_ng_setup_request amf_connection_setup_routine::fill_ng_setup_request()
{
  ngap_ng_setup_request request;

  // fill global ran node id
  request.global_ran_node_id.gnb_id = cu_cp_cfg.node.gnb_id;
  // TODO: Which PLMN do we need to use here?
  request.global_ran_node_id.plmn_id = cu_cp_cfg.node.supported_tas.front().plmn;
  // fill ran node name
  request.ran_node_name = cu_cp_cfg.node.ran_node_name;
  // fill supported ta list
  for (const auto& supported_ta : cu_cp_cfg.node.supported_tas) {
    // TODO: add support for more items
    ngap_supported_ta_item supported_ta_item;

    ngap_broadcast_plmn_item broadcast_plmn_item;
    broadcast_plmn_item.plmn_id = supported_ta.plmn;

    for (const auto& slice_config : supported_ta.supported_slices) {
      slice_support_item_t slice_support_item;
      slice_support_item.s_nssai.sst = slice_config.sst;
      if (slice_config.sd.has_value()) {
        slice_support_item.s_nssai.sd = slice_config.sd.value();
      }
      broadcast_plmn_item.tai_slice_support_list.push_back(slice_support_item);
    }

    supported_ta_item.broadcast_plmn_list.push_back(broadcast_plmn_item);
    supported_ta_item.tac = supported_ta.tac;

    request.supported_ta_list.push_back(supported_ta_item);
  }

  // fill paging drx
  request.default_paging_drx = 256;

  return request;
}

async_task<ngap_ng_setup_result> amf_connection_setup_routine::send_ng_setup_request()
{
  // Prepare request to send to ng.
  ngap_ng_setup_request request = fill_ng_setup_request();
  request.max_setup_retries     = 1;

  // Initiate NG Setup Request.
  return ngap_conn_mng.handle_ng_setup_request(request);
}
