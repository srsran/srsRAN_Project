/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/cu_cp/cu_cp_types.h"

namespace srsgnb {
namespace srs_cu_cp {

// Creates a PDU Session Resource Setup Request with a single PDU session and QoS flow
cu_cp_pdu_session_resource_setup_message fill_res_setup_request()
{
  cu_cp_pdu_session_resource_setup_message req;

  req.cu_cp_ue_id = uint_to_cu_cp_ue_id(0);

  cu_cp_pdu_session_res_setup_item item;
  item.pdu_session_id = 1;
  item.pdu_session_nas_pdu.resize(2);
  item.pdu_session_nas_pdu[0] = 0xaa;
  item.pdu_session_nas_pdu[1] = 0xbb;
  item.s_nssai.sst            = 1;

  item.pdu_session_aggregate_maximum_bit_rate_dl = 100;
  item.pdu_session_aggregate_maximum_bit_rate_ul = 100;
  item.ul_ngu_up_tnl_info                        = {transport_layer_address{"127.0.0.1"}, int_to_gtp_teid(0xdeadbeef)};
  item.pdu_session_type                          = "ipv4";

  qos_flow_setup_request_item qos_item;
  qos_item.qos_flow_id                                   = 1;
  qos_item.qos_characteristics.is_dynamic_5qi            = false;
  qos_item.qos_characteristics.five_qi                   = 9;
  qos_item.qos_characteristics.prio_level_arp            = 8;
  qos_item.qos_characteristics.pre_emption_cap           = "not-pre-emptable";
  qos_item.qos_characteristics.pre_emption_vulnerability = "not-pre-emptable";

  item.qos_flow_setup_request_items.push_back(qos_item);

  req.pdu_session_res_setup_items.push_back(item);

  return req;
};

} // namespace srs_cu_cp
} // namespace srsgnb
