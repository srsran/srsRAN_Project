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

#include "f1ap_test_messages.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/test_utils.h"

using namespace srsran;

static asn1::f1ap::drbs_to_be_setup_item_s generate_drb_am_setup_item(drb_id_t drbid)
{
  using namespace asn1::f1ap;

  drbs_to_be_setup_item_s drb;
  drb.drb_id = drb_id_to_uint(drbid);
  drb.qos_info.set_choice_ext().load_info_obj(ASN1_F1AP_ID_DRB_INFO);
  auto& drb_info                                                 = drb.qos_info.choice_ext()->drb_info();
  drb_info.drb_qos.qos_characteristics.set_non_dyn_5qi().five_qi = 9;
  drb_info.drb_qos.ngra_nalloc_retention_prio.prio_level         = 1;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_cap.value =
      pre_emption_cap_opts::shall_not_trigger_pre_emption;
  drb_info.drb_qos.ngra_nalloc_retention_prio.pre_emption_vulnerability.value =
      pre_emption_vulnerability_opts::not_pre_emptable;
  drb_info.drb_qos.reflective_qos_attribute_present = true;
  drb_info.drb_qos.reflective_qos_attribute.value =
      qos_flow_level_qos_params_s::reflective_qos_attribute_opts::subject_to;
  drb_info.snssai.sst.from_string("01");
  drb_info.snssai.sd.from_string("0027db");
  drb.rlc_mode.value = rlc_mode_opts::rlc_am;
  drb.ul_up_tnl_info_to_be_setup_list.resize(1);
  auto& gtp_tun = drb.ul_up_tnl_info_to_be_setup_list[0].ul_up_tnl_info.set_gtp_tunnel();
  auto  addr    = transport_layer_address::create_from_string("127.0.0.1");
  gtp_tun.transport_layer_address.from_string(addr.to_bitstring());
  gtp_tun.gtp_teid.from_number(1);

  return drb;
}

f1ap_message srsran::test_helpers::create_ue_context_setup_request(gnb_cu_ue_f1ap_id_t           cu_ue_id,
                                                                   optional<gnb_du_ue_f1ap_id_t> du_ue_id,
                                                                   const std::vector<drb_id_t>&  drbs_to_setup)
{
  using namespace asn1::f1ap;
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);
  ue_context_setup_request_s& dl_msg = msg.pdu.init_msg().value.ue_context_setup_request();

  dl_msg->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  if (du_ue_id.has_value()) {
    dl_msg->gnb_du_ue_f1ap_id_present = true;
    dl_msg->gnb_du_ue_f1ap_id         = (unsigned)*du_ue_id;
  }

  // SRB2.
  dl_msg->srbs_to_be_setup_list_present = true;
  dl_msg->srbs_to_be_setup_list.resize(1);
  dl_msg->srbs_to_be_setup_list[0].load_info_obj(ASN1_F1AP_ID_SRBS_SETUP_ITEM);
  srbs_to_be_setup_item_s& srb2 = dl_msg->srbs_to_be_setup_list[0]->srbs_to_be_setup_item();
  srb2.srb_id                   = 2;

  // drbs-to-be-setup
  dl_msg->drbs_to_be_setup_list_present = drbs_to_setup.size() > 0;
  dl_msg->drbs_to_be_setup_list.resize(drbs_to_setup.size());
  unsigned count = 0;
  for (drb_id_t drbid : drbs_to_setup) {
    dl_msg->drbs_to_be_setup_list[count].load_info_obj(ASN1_F1AP_ID_DRB_INFO);
    dl_msg->drbs_to_be_setup_list[count]->drbs_to_be_setup_item() = generate_drb_am_setup_item(drbid);
    ++count;
  }

  dl_msg->rrc_container_present = true;
  report_error_if_not(
      dl_msg->rrc_container.append(test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(3, 100))),
      "Failed to allocate RRC containe");

  return msg;
}
