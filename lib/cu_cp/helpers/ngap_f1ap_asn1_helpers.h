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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/asn1/f1ap/f1ap.h"
#include "srsgnb/asn1/ngap/ngap.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/f1c/cu_cp/f1c_cu.h"
#include <string>
#include <vector>

namespace srsgnb {
namespace srs_cu_cp {

inline void fill_f1ap_ue_context_modification_request(f1ap_ue_context_modification_request_message&  f1c_request,
                                                      const f1ap_pdu_session_resource_setup_message& msg)
{
  // drb to be setup mod list
  f1c_request.msg->drbs_to_be_setup_mod_list_present = true;

  asn1::protocol_ie_single_container_s<asn1::f1ap::drbs_to_be_setup_mod_item_ies_o> f1ap_setup_item;
  auto& f1ap_drb_to_setup_item = f1ap_setup_item->drbs_to_be_setup_mod_item();

  f1ap_drb_to_setup_item.drbid = msg.ngap_drb_setup_msg.drb_id;

  for (auto ngap_up_tnl_info_item : msg.ngap_drb_setup_msg.up_tnl_infos_list) {
    asn1::f1ap::uluptnl_info_to_be_setup_item_s uluptnl_item;
    uluptnl_item.uluptnl_info.set_gtp_tunnel();
    auto& gtp_tunnel    = uluptnl_item.uluptnl_info.gtp_tunnel();
    gtp_tunnel.gtp_teid = ngap_up_tnl_info_item.gtp_tunnel.gtp_teid;
    gtp_tunnel.transport_layer_address.from_number(
        ngap_up_tnl_info_item.gtp_tunnel.transport_layer_address.to_number());
    f1ap_drb_to_setup_item.uluptnl_info_to_be_setup_list.push_back(uluptnl_item);
  }

  // TODO: Add qos information and optional values

  f1c_request.msg->drbs_to_be_setup_mod_list.value.push_back(f1ap_setup_item);

  if (msg.ue_aggregate_maximum_bit_rate_ul.has_value()) {
    f1c_request.msg->gnb_du_ue_ambr_ul_present = true;
    f1c_request.msg->gnb_du_ue_ambr_ul.value   = msg.ue_aggregate_maximum_bit_rate_ul.value();
  }
}

} // namespace srs_cu_cp
} // namespace srsgnb
