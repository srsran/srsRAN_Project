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

f1c_msg generate_f1_setup_request()
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
      "92002808241099000001000000000a4213407800008c98d6d8d7f616e0804000020107e28180008000088a0dc7008000088a0007141a"
      "22"
      "81c874cc00020000232d5c6b6c65462001ec4cc5fc9c0493946a98d4d1e99355c00a1aba010580ec024646f62180");

  setup_req->gnb_du_served_cells_list.value.push_back(served_cells_item_container);

  return f1c_msg;
}

} // namespace srs_cu_cp
} // namespace srsgnb
