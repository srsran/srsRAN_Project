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
#include "srsgnb/f1_interface/common/f1c_common.h"

namespace srsgnb {
namespace srs_cu_cp {

f1c_msg generate_f1_setup_request_base()
{
  f1c_msg f1c_msg = {};

  f1c_msg.pdu.set_init_msg();
  f1c_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_req                 = f1c_msg.pdu.init_msg().value.f1_setup_request();
  setup_req->transaction_id.value = 99;
  setup_req->gnb_du_id.value      = 0x11;
  setup_req->gnb_du_name_present  = true;
  setup_req->gnb_du_name.value.from_string("srsDU");
  setup_req->gnb_du_rrc_version.value.latest_rrc_version.from_number(1);

  return f1c_msg;
}

f1c_msg generate_valid_f1_setup_request()
{
  f1c_msg f1c_msg = generate_f1_setup_request_base();

  auto& setup_req                             = f1c_msg.pdu.init_msg().value.f1_setup_request();
  setup_req->gnb_du_served_cells_list_present = true;
  setup_req->gnb_du_served_cells_list.id      = ASN1_F1AP_ID_G_NB_DU_SERVED_CELLS_LIST;
  setup_req->gnb_du_served_cells_list.crit    = asn1::crit_opts::reject;

  asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_du_served_cells_item_ies_o> served_cells_item_container = {};
  served_cells_item_container.set_item(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM);

  auto& served_cells_item = served_cells_item_container.value().gnb_du_served_cells_item();
  served_cells_item.served_cell_info.nrcgi.plmn_id.from_string("208991");
  served_cells_item.served_cell_info.nrcgi.nrcell_id.from_number(12345678);
  served_cells_item.served_cell_info.nrpci               = 0;
  served_cells_item.served_cell_info.five_gs_tac_present = true;
  served_cells_item.served_cell_info.five_gs_tac.from_number(1);

  asn1::f1ap::served_plmns_item_s served_plmn;
  served_plmn.plmn_id.from_string("208991");
  asn1::protocol_ext_field_s<asn1::f1ap::served_plmns_item_ext_ies_o> plmn_ext_container = {};
  plmn_ext_container.set_item(ASN1_F1AP_ID_TAI_SLICE_SUPPORT_LIST);
  auto&                            tai_slice_support_list = plmn_ext_container.value().tai_slice_support_list();
  asn1::f1ap::slice_support_item_s slice_support_item;
  slice_support_item.snssai.sst.from_number(1);
  tai_slice_support_list.push_back(slice_support_item);
  served_plmn.ie_exts.push_back(plmn_ext_container);
  served_cells_item.served_cell_info.served_plmns.push_back(served_plmn);

  served_cells_item.served_cell_info.nr_mode_info.set_tdd();
  served_cells_item.served_cell_info.nr_mode_info.tdd().nrfreq_info.nrarfcn = 626748;
  asn1::f1ap::freq_band_nr_item_s freq_band_nr_item;
  freq_band_nr_item.freq_band_ind_nr = 78;
  served_cells_item.served_cell_info.nr_mode_info.tdd().nrfreq_info.freq_band_list_nr.push_back(freq_band_nr_item);
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nrscs.value = asn1::f1ap::nrscs_opts::scs30;
  served_cells_item.served_cell_info.nr_mode_info.tdd().tx_bw.nrnrb.value = asn1::f1ap::nrnrb_opts::nrb51;
  served_cells_item.served_cell_info.meas_timing_cfg.from_string("30");

  served_cells_item.gnb_du_sys_info_present = true;
  served_cells_item.gnb_du_sys_info.mib_msg.from_string("01c586");
  served_cells_item.gnb_du_sys_info.sib1_msg.from_string(
      "92002808241099000001000000000a4213407800008c98d6d8d7f616e0804000020107e28180008000088a0dc7008000088a0007141a22"
      "81c874cc00020000232d5c6b6c65462001ec4cc5fc9c0493946a98d4d1e99355c00a1aba010580ec024646f62180");

  setup_req->gnb_du_served_cells_list.value.push_back(served_cells_item_container);

  return f1c_msg;
}

f1c_msg generate_valid_f1_init_ul_rrc_msg(unsigned int c_rnti)
{
  f1c_msg init_ul_rrc_msg = {};

  init_ul_rrc_msg.pdu.set_init_msg();
  init_ul_rrc_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_INIT_ULRRC_MSG_TRANSFER);

  auto& init_ul_rrc                     = init_ul_rrc_msg.pdu.init_msg().value.init_ulrrc_msg_transfer();
  init_ul_rrc->gnb_du_ue_f1_ap_id.value = c_rnti; // same as C-RNTI

  init_ul_rrc->nrcgi.value.nrcell_id.from_string("000000000000101111000110000101001110"); // 12345678 in decimal
  init_ul_rrc->nrcgi.value.plmn_id.from_string("02f899");
  init_ul_rrc->c_rnti.value = c_rnti;

  init_ul_rrc->rrc_container.value.from_string("1dec89d05766");
  init_ul_rrc->duto_currc_container_present = true;
  init_ul_rrc->duto_currc_container.value.from_string(
      "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
      "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
      "c400");

  return init_ul_rrc_msg;
}

f1c_msg generate_f1_ue_context_release_complete_msg(unsigned int cu_ue_id, unsigned int c_rnti)
{
  f1c_msg ue_ctxt_rel_complete_msg = {};
  ue_ctxt_rel_complete_msg.pdu.set_successful_outcome();
  ue_ctxt_rel_complete_msg.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  auto& rel_complete_msg = ue_ctxt_rel_complete_msg.pdu.successful_outcome().value.ue_context_release_complete();
  rel_complete_msg->gnb_cu_ue_f1_ap_id.value = cu_ue_id;
  rel_complete_msg->gnb_du_ue_f1_ap_id.value = c_rnti;

  return ue_ctxt_rel_complete_msg;
}

} // namespace srs_cu_cp
} // namespace srsgnb
