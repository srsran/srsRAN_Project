/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "e2ap_asn1_utils.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/e2/e2.h"
#include "srsran/e2/e2ap_configuration.h"
#include "srsran/ran/bcd_helpers.h"
#include "srsran/security/security.h"
#include <string>
#include <vector>

namespace srsran {

inline void fill_asn1_e2ap_setup_request(asn1::e2ap::e2setup_request_s& setup, const e2ap_configuration& e2ap_config)
{
  using namespace asn1::e2ap;
  e2_message  e2_msg;
  init_msg_s& initmsg = e2_msg.pdu.set_init_msg();
  initmsg.load_info_obj(ASN1_E2AP_ID_E2SETUP);
  setup = initmsg.value.e2setup_request();

  //  Transaction ID
  setup->transaction_id.crit        = asn1::crit_opts::reject;
  setup->transaction_id.value.value = 1;

  // Global e2 node ID
  setup->global_e2node_id.crit = asn1::crit_opts::reject;
  auto& gnb_id                 = setup->global_e2node_id.value.set_gnb();
  gnb_id.global_g_nb_id.gnb_id.gnb_id().from_number(e2ap_config.gnb_id, 28);
  // convert PLMN to BCD
  uint32_t plmn_bcd = plmn_string_to_bcd(e2ap_config.plmn);
  gnb_id.global_g_nb_id.plmn_id.from_number(plmn_bcd);

  // RAN functions added
  setup->ra_nfunctions_added.crit = asn1::crit_opts::reject;
  setup->ra_nfunctions_added.id   = ASN1_E2AP_ID_RA_NFUNCTIONS_ADDED;
  asn1::protocol_ie_single_container_s<ra_nfunction_item_ies_o> ran_func_item;
  ran_func_item.load_info_obj(ASN1_E2AP_ID_RA_NFUNCTION_ITEM);
  ran_func_item.value().ra_nfunction_item().ran_function_id = 147;
  ran_func_item.value().ra_nfunction_item().ran_function_definition.from_string(
      "40304f52414e2d4532534d2d4b504d000018312e332e362e312e342e312e35333134382e312e322e322e3205004b504d204d6f6e69746f72"
      "0001010700506572696f646963207265706f72740101");
  ran_func_item.value().ra_nfunction_item().ran_function_oid.resize(30);
  ran_func_item.value().ra_nfunction_item().ran_function_oid.from_string("1.3.6.1.4.1.53148.1.2.2.2");
  setup->ra_nfunctions_added.value.push_back(ran_func_item);

  // E2 node component config
  setup->e2node_component_cfg_addition.crit = asn1::crit_opts::reject;
  auto& list                                = setup->e2node_component_cfg_addition.value;
  list.resize(1);
  list[0].load_info_obj(ASN1_E2AP_ID_E2NODE_COMPONENT_CFG_ADDITION_ITEM);
  e2node_component_cfg_addition_item_s& e2node_cfg_item = list[0].value().e2node_component_cfg_addition_item();
  e2node_cfg_item.e2node_component_interface_type       = e2node_component_interface_type_opts::ng;
  e2node_cfg_item.e2node_component_id.set_e2node_component_interface_type_ng().amf_name.from_string("nginterf");
  e2node_cfg_item.e2node_component_cfg.e2node_component_request_part.from_string("72657170617274");
  e2node_cfg_item.e2node_component_cfg.e2node_component_resp_part.from_string("72657370617274");
}

} // namespace srsran