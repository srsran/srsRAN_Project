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

#include "../ran/bcd_helpers.h"
#include "srsgnb/asn1/ngap/ngap.h"

namespace srsgnb {

namespace srs_cu_cp {

// TODO: pass params using struct
/// \brief Fills ASN.1 NGSetupRequest struct.
/// \param[out] request The NGSetupRequest ASN.1 struct to fill.
/// \param[in] global_gnb_id The global RAN node id
/// \param[in] ran_node_name The RAN node name
/// \param[in] plmn_id The BCD-encoded PLMN
/// \param[in] tac The TAC
void fill_asn1_ng_setup_request(asn1::ngap::ng_setup_request_s& request,
                                int                             global_gnb_id,
                                std::string                     ran_node_name,
                                std::string                     plmn_id,
                                int                             tac)
{
  // convert PLMN to BCD
  uint32_t plmn_bcd = plmn_string_to_bcd(plmn_id);

  // fill global ran node id
  request->global_ran_node_id.value.set_global_gnb_id();
  request->global_ran_node_id.value.global_gnb_id().gnb_id.set_gnb_id();
  request->global_ran_node_id.value.global_gnb_id().gnb_id.gnb_id().from_number(global_gnb_id);
  request->global_ran_node_id.value.global_gnb_id().plmn_id.from_number(plmn_bcd);

  // fill ran node name
  request->ran_node_name_present = true;
  request->ran_node_name.value.from_string(ran_node_name);

  // fill supported ta list
  // TODO: add support for more items
  request->supported_ta_list.id   = ASN1_NGAP_ID_SUPPORTED_TA_LIST;
  request->supported_ta_list.crit = asn1::crit_opts::reject;

  asn1::ngap::supported_ta_item_s supported_ta_item = {};

  asn1::ngap::broadcast_plmn_item_s broadcast_plmn_item = {};
  broadcast_plmn_item.plmn_id.from_number(plmn_bcd);

  asn1::ngap::slice_support_item_s slice_support_item = {};
  slice_support_item.s_nssai.sst.from_number(1);
  broadcast_plmn_item.tai_slice_support_list.push_back(slice_support_item);

  supported_ta_item.broadcast_plmn_list.push_back(broadcast_plmn_item);
  supported_ta_item.tac.from_number(tac);

  request->supported_ta_list.value.push_back(supported_ta_item);

  // fill paging drx
  request->default_paging_drx.value.value = asn1::ngap::paging_drx_opts::v256;
}

} // namespace srs_cu_cp

} // namespace srsgnb
