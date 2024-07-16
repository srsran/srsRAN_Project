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
#include "../pdcp/pdcp_pdu_generator.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/asn1/f1ap/f1ap_ies.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace asn1::f1ap;

static byte_buffer generate_rrc_container(uint32_t pdcp_sn, unsigned pdu_len)
{
  return test_helpers::create_pdcp_pdu(
      pdcp_sn_size::size12bits, true, pdcp_sn, pdu_len, test_rgen::uniform_int<uint8_t>());
}

gnb_du_served_cells_item_s
srsran::test_helpers::generate_served_cells_item(nr_cell_identity nci, pci_t nrpci, unsigned tac)
{
  gnb_du_served_cells_item_s served_cells_item;
  served_cells_item.served_cell_info.nr_cgi.plmn_id.from_string("00f110");
  served_cells_item.served_cell_info.nr_cgi.nr_cell_id.from_number(nci.value());
  served_cells_item.served_cell_info.nr_pci              = nrpci;
  served_cells_item.served_cell_info.five_gs_tac_present = true;
  served_cells_item.served_cell_info.five_gs_tac.from_number(tac);

  served_plmns_item_s served_plmn;
  served_plmn.plmn_id.from_string("00f110");
  slice_support_item_s slice_support_item;
  slice_support_item.snssai.sst.from_number(1);
  served_plmn.ie_exts.tai_slice_support_list_present = true;
  served_plmn.ie_exts.tai_slice_support_list.push_back(slice_support_item);
  served_cells_item.served_cell_info.served_plmns.push_back(served_plmn);

  served_cells_item.served_cell_info.nr_mode_info.set_tdd();
  served_cells_item.served_cell_info.nr_mode_info.tdd().nr_freq_info.nr_arfcn = 626748;
  freq_band_nr_item_s freq_band_nr_item;
  freq_band_nr_item.freq_band_ind_nr = 78;
  served_cells_item.served_cell_info.nr_mode_info.tdd().nr_freq_info.freq_band_list_nr.push_back(freq_band_nr_item);
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nr_scs.value = nr_scs_opts::scs30;
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nr_nrb.value = nr_nrb_opts::nrb51;
  served_cells_item.served_cell_info.meas_timing_cfg.from_string("101105af4084");

  served_cells_item.gnb_du_sys_info_present = true;
  served_cells_item.gnb_du_sys_info.mib_msg.from_string("01c586");
  served_cells_item.gnb_du_sys_info.sib1_msg.from_string(
      "92002808241099000001000000000a4213407800008c98d6d8d7f616e0804000020107e28180008000088a0dc7008000088a0007141a22"
      "81c874cc00020000232d5c6b6c65462001ec4cc5fc9c0493946a98d4d1e99355c00a1aba010580ec024646f62180");

  return served_cells_item;
}

f1ap_message
srsran::test_helpers::generate_f1_setup_request(gnb_du_id_t gnb_du_id, nr_cell_identity nci, pci_t pci, unsigned tac)
{
  f1ap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_req                = msg.pdu.init_msg().value.f1_setup_request();
  setup_req->transaction_id      = 99;
  setup_req->gnb_du_id           = (uint64_t)gnb_du_id;
  setup_req->gnb_du_name_present = true;
  setup_req->gnb_du_name.from_string("srsDU");
  setup_req->gnb_du_rrc_version.latest_rrc_version.from_number(1);
  setup_req->gnb_du_served_cells_list_present = true;
  setup_req->gnb_du_served_cells_list.resize(1);
  setup_req->gnb_du_served_cells_list[0].load_info_obj(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM);
  setup_req->gnb_du_served_cells_list[0].value().gnb_du_served_cells_item() = generate_served_cells_item(nci, pci, tac);

  return msg;
}

f1ap_message srsran::test_helpers::generate_f1_setup_response(const f1ap_message& f1_setup_request)
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

f1ap_message srsran::test_helpers::generate_f1_removal_request(unsigned transaction_id)
{
  f1ap_message req;
  req.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_F1_REMOVAL);
  f1_removal_request_s& ie = req.pdu.init_msg().value.f1_removal_request();

  ie->resize(1);
  (*ie)[0].load_info_obj(ASN1_F1AP_ID_TRANSACTION_ID);
  (*ie)[0]->transaction_id() = transaction_id;

  return req;
}

f1ap_message srsran::test_helpers::generate_f1_removal_response(const f1ap_message& f1_removal_request)
{
  srsran_assert(f1_removal_request.pdu.type().value == f1ap_pdu_c::types_opts::init_msg, "Expected F1 removal request");
  srsran_assert(f1_removal_request.pdu.init_msg().value.type().value ==
                    f1ap_elem_procs_o::init_msg_c::types_opts::f1_removal_request,
                "Expected F1 removal request");

  f1ap_message resp;
  resp.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_REMOVAL);
  f1_removal_resp_s& ie = resp.pdu.successful_outcome().value.f1_removal_resp();
  ie->transaction_id    = (*f1_removal_request.pdu.init_msg().value.f1_removal_request())[0]->transaction_id();

  return resp;
}

static drbs_to_be_setup_item_s generate_drb_am_setup_item(drb_id_t drbid)
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
  drb.rlc_mode.value         = rlc_mode_opts::rlc_am;
  drb.ie_exts_present        = true;
  drb.ie_exts.dl_pdcp_sn_len = pdcp_sn_len_opts::twelve_bits;
  drb.ul_up_tnl_info_to_be_setup_list.resize(1);
  auto& gtp_tun = drb.ul_up_tnl_info_to_be_setup_list[0].ul_up_tnl_info.set_gtp_tunnel();
  auto  addr    = transport_layer_address::create_from_string("127.0.0.1");
  gtp_tun.transport_layer_address.from_string(addr.to_bitstring());
  gtp_tun.gtp_teid.from_number(1);

  return drb;
}

f1ap_message srsran::test_helpers::create_ue_context_setup_request(gnb_cu_ue_f1ap_id_t                cu_ue_id,
                                                                   std::optional<gnb_du_ue_f1ap_id_t> du_ue_id,
                                                                   uint32_t                     rrc_container_pdcp_sn,
                                                                   const std::vector<drb_id_t>& drbs_to_setup)
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
  bool success                  = dl_msg->rrc_container.append(
      generate_rrc_container(rrc_container_pdcp_sn, test_rgen::uniform_int<unsigned>(3, 100)));
  report_error_if_not(success, "Failed to allocate RRC container");

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

  nr_cell_identity nci = nr_cell_identity::create(gnb_id_t{411, 22}, 0).value();
  init_ul_rrc->nr_cgi.nr_cell_id.from_number(nci.value());
  init_ul_rrc->nr_cgi.plmn_id.from_string("00f110");
  init_ul_rrc->c_rnti = to_value(crnti);

  init_ul_rrc->sul_access_ind_present = true;
  init_ul_rrc->sul_access_ind.value   = sul_access_ind_opts::options::true_value;

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

f1ap_message srsran::test_helpers::create_dl_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                  gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                  srb_id_t            srb_id,
                                                                  byte_buffer         rrc_container)
{
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_DL_RRC_MSG_TRANSFER);
  auto& dlmsg = *msg.pdu.init_msg().value.dl_rrc_msg_transfer();

  dlmsg.gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  dlmsg.gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  dlmsg.srb_id            = (uint8_t)srb_id;
  dlmsg.rrc_container     = std::move(rrc_container);

  return msg;
}

f1ap_message srsran::test_helpers::create_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                  gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                  srb_id_t            srb_id,
                                                                  byte_buffer         rrc_container)
{
  f1ap_message msg;

  msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UL_RRC_MSG_TRANSFER);
  ul_rrc_msg_transfer_s& ulmsg = msg.pdu.init_msg().value.ul_rrc_msg_transfer();

  ulmsg->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  ulmsg->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  ulmsg->srb_id            = (uint8_t)srb_id;
  ulmsg->rrc_container     = std::move(rrc_container);

  return msg;
}

f1ap_message srsran::test_helpers::create_ul_rrc_message_transfer(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                  gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                  srb_id_t            srb_id,
                                                                  uint32_t            pdcp_sn,
                                                                  byte_buffer         ul_dcch_msg)
{
  // > Prepend PDCP header and append MAC.
  std::array<uint8_t, 2> pdcp_header{static_cast<uint8_t>((pdcp_sn >> 8U) & 0x0fU),
                                     static_cast<uint8_t>(pdcp_sn & 0xffU)};
  report_fatal_error_if_not(ul_dcch_msg.prepend(pdcp_header), "bad alloc");

  // > Append MAC
  report_fatal_error_if_not(ul_dcch_msg.append(std::array<uint8_t, 4>{}), "bad alloc");

  return create_ul_rrc_message_transfer(du_ue_id, cu_ue_id, srb_id, std::move(ul_dcch_msg));
}

f1ap_message srsran::test_helpers::generate_ue_context_release_request(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                       gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message msg;
  msg.pdu.set_init_msg();
  msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE_REQUEST);

  auto& release_req              = msg.pdu.init_msg().value.ue_context_release_request();
  release_req->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  release_req->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;
  release_req->cause.set_radio_network();
  release_req->cause.radio_network().value = cause_radio_network_e::rl_fail_others;

  return msg;
}

f1ap_message srsran::test_helpers::generate_ue_context_release_complete(const f1ap_message& ue_ctxt_release_cmd)
{
  srsran_assert(ue_ctxt_release_cmd.pdu.type().value == f1ap_pdu_c::types_opts::init_msg, "Invalid argument message");
  srsran_assert(ue_ctxt_release_cmd.pdu.init_msg().value.type().value ==
                    f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd,
                "Invalid argument message");

  const ue_context_release_cmd_s& cmd = ue_ctxt_release_cmd.pdu.init_msg().value.ue_context_release_cmd();

  return generate_ue_context_release_complete(int_to_gnb_cu_ue_f1ap_id(cmd->gnb_cu_ue_f1ap_id),
                                              int_to_gnb_du_ue_f1ap_id(cmd->gnb_du_ue_f1ap_id));
}

f1ap_message srsran::test_helpers::generate_ue_context_release_complete(gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                        gnb_du_ue_f1ap_id_t du_ue_id)
{
  f1ap_message ue_ctxt_rel_complete_msg = {};
  ue_ctxt_rel_complete_msg.pdu.set_successful_outcome();
  ue_ctxt_rel_complete_msg.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  ue_context_release_complete_s& rel_complete_msg =
      ue_ctxt_rel_complete_msg.pdu.successful_outcome().value.ue_context_release_complete();

  rel_complete_msg->gnb_cu_ue_f1ap_id = (unsigned)cu_ue_id;
  rel_complete_msg->gnb_du_ue_f1ap_id = (unsigned)du_ue_id;

  return ue_ctxt_rel_complete_msg;
}

f1ap_message srsran::test_helpers::generate_ue_context_modification_response(gnb_du_ue_f1ap_id_t du_ue_id,
                                                                             gnb_cu_ue_f1ap_id_t cu_ue_id,
                                                                             rnti_t              crnti)
{
  f1ap_message pdu = {};

  pdu.pdu.set_successful_outcome();
  pdu.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);

  auto& ue_context_mod_resp                        = pdu.pdu.successful_outcome().value.ue_context_mod_resp();
  ue_context_mod_resp->gnb_cu_ue_f1ap_id           = (unsigned)cu_ue_id;
  ue_context_mod_resp->gnb_du_ue_f1ap_id           = (unsigned)du_ue_id;
  ue_context_mod_resp->c_rnti_present              = true;
  ue_context_mod_resp->c_rnti                      = (unsigned)crnti;
  ue_context_mod_resp->drbs_setup_mod_list_present = true;

  ue_context_mod_resp->drbs_setup_mod_list.push_back({});
  ue_context_mod_resp->drbs_setup_mod_list.back().load_info_obj(ASN1_F1AP_ID_DRBS_SETUP_MOD_ITEM);
  ue_context_mod_resp->drbs_setup_mod_list.back().value().drbs_setup_mod_item().drb_id = 1;

  return pdu;
}

byte_buffer srsran::test_helpers::extract_dl_dcch_msg(const byte_buffer& rrc_container)
{
  byte_buffer pdu = rrc_container.deep_copy().value();
  report_fatal_error_if_not(pdu.length() >= 7, "Invalid RRC container");

  // Remove PDCP header.
  pdu.trim_head(2);

  // Remove MAC.
  pdu.trim_tail(4);

  return pdu;
}
