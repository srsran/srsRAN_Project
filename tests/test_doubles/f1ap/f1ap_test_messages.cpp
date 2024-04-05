/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1ap_test_messages.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace asn1::f1ap;

f1ap_message srsran::test_helpers::create_f1_setup_response(const f1ap_message& f1_setup_request)
{
  srsran_assert(f1_setup_request.pdu.type().value == f1ap_pdu_c::types_opts::init_msg, "Expected F1 setup request");
  srsran_assert(f1_setup_request.pdu.init_msg().value.type().value ==
                    f1ap_elem_procs_o::init_msg_c::types_opts::f1_setup_request,
                "Expected F1 setup request");
  auto& req = f1_setup_request.pdu.init_msg().value.f1_setup_request();

  f1ap_message resp;
  resp.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  f1_setup_resp_s& f1_setup_resp = resp.pdu.successful_outcome().value.f1_setup_resp();

  f1_setup_resp->cells_to_be_activ_list_present = true;
  f1_setup_resp->cells_to_be_activ_list.resize(req->gnb_du_served_cells_list.size());
  for (unsigned i = 0; i != req->gnb_du_served_cells_list.size(); ++i) {
    auto& req_cell = req->gnb_du_served_cells_list[i]->gnb_du_served_cells_item();
    f1_setup_resp->cells_to_be_activ_list[i].load_info_obj(ASN1_F1AP_ID_CELLS_TO_BE_ACTIV_LIST_ITEM);
    auto& cell          = f1_setup_resp->cells_to_be_activ_list[i].value().cells_to_be_activ_list_item();
    cell.nr_cgi         = req_cell.served_cell_info.nr_cgi;
    cell.nr_pci_present = true;
    cell.nr_pci         = req_cell.served_cell_info.nr_pci;
  }

  return resp;
}

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

f1ap_message srsran::test_helpers::create_init_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                       rnti_t              crnti,
                                                                       byte_buffer         cell_group_cfg,
                                                                       byte_buffer         rrc_container)
{
  f1ap_message init_ul_rrc_msg;

  init_ul_rrc_msg.pdu.set_init_msg();
  init_ul_rrc_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER);

  init_ul_rrc_msg_transfer_s& init_ul_rrc = init_ul_rrc_msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
  init_ul_rrc->gnb_du_ue_f1ap_id          = (unsigned)du_ue_id;

  init_ul_rrc->nr_cgi.nr_cell_id.from_string("000000000000000000000001100110110000"); // 6576 in decimal
  init_ul_rrc->nr_cgi.plmn_id.from_string("00f110");
  init_ul_rrc->c_rnti = to_value(crnti);

  init_ul_rrc->sul_access_ind_present = true;
  init_ul_rrc->sul_access_ind.value   = asn1::f1ap::sul_access_ind_opts::options::true_value;

  if (rrc_container.empty()) {
    init_ul_rrc->rrc_container.from_string("1dec89d05766");
  } else {
    init_ul_rrc->rrc_container = std::move(rrc_container);
  }

  init_ul_rrc->du_to_cu_rrc_container_present = true;
  if (cell_group_cfg.empty()) {
    init_ul_rrc->du_to_cu_rrc_container.from_string(
        "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
        "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
        "c400");
  } else {
    init_ul_rrc->du_to_cu_rrc_container = std::move(cell_group_cfg);
  }

  return init_ul_rrc_msg;
}
