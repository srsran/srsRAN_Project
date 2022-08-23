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
#include "srsgnb/f1_interface/cu/f1ap_cu.h"
#include "srsgnb/f1_interface/cu/f1ap_cu_factory.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_cu_cp {

/// Fixture class for F1AP
class f1ap_cu_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    f1c_pdu_notifier      = std::make_unique<dummy_f1c_pdu_notifier>(nullptr);
    du_processor_notifier = std::make_unique<dummy_f1c_du_processor_notifier>();
    f1c_du_mgmt_notifier  = std::make_unique<dummy_f1c_du_management_notifier>(nullptr);

    f1ap = create_f1ap(*f1c_pdu_notifier, *du_processor_notifier, *f1c_du_mgmt_notifier);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  std::unique_ptr<f1_interface>                     f1ap;
  std::unique_ptr<dummy_f1c_pdu_notifier>           f1c_pdu_notifier;
  std::unique_ptr<dummy_f1c_du_processor_notifier>  du_processor_notifier;
  std::unique_ptr<dummy_f1c_du_management_notifier> f1c_du_mgmt_notifier;
  srslog::basic_logger&                             test_logger = srslog::fetch_basic_logger("TEST");
};

f1c_message generate_f1_setup_request_base()
{
  f1c_message f1_setup_request_base = {};

  f1_setup_request_base.pdu.set_init_msg();
  f1_setup_request_base.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

  auto& setup_req                 = f1_setup_request_base.pdu.init_msg().value.f1_setup_request();
  setup_req->transaction_id.value = 99;
  setup_req->gnb_du_id.value      = 0x11;
  setup_req->gnb_du_name_present  = true;
  setup_req->gnb_du_name.value.from_string("srsDU");
  setup_req->gnb_du_rrc_version.value.latest_rrc_version.from_number(1);

  return f1_setup_request_base;
}

asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_du_served_cells_item_ies_o>
generate_served_cells_item(unsigned nrcell_id, unsigned nrpci)
{
  asn1::protocol_ie_single_container_s<asn1::f1ap::gnb_du_served_cells_item_ies_o> served_cells_item_container = {};
  served_cells_item_container.set_item(ASN1_F1AP_ID_GNB_DU_SERVED_CELLS_ITEM);

  auto& served_cells_item = served_cells_item_container.value().gnb_du_served_cells_item();
  served_cells_item.served_cell_info.nrcgi.plmn_id.from_string("208991");
  served_cells_item.served_cell_info.nrcgi.nrcell_id.from_number(nrcell_id);
  served_cells_item.served_cell_info.nrpci               = nrpci;
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

  return served_cells_item_container;
}

f1c_message generate_valid_f1_setup_request()
{
  f1c_message f1_setup_request = generate_f1_setup_request_base();

  auto& setup_req                             = f1_setup_request.pdu.init_msg().value.f1_setup_request();
  setup_req->gnb_du_served_cells_list_present = true;
  setup_req->gnb_du_served_cells_list.id      = ASN1_F1AP_ID_G_NB_DU_SERVED_CELLS_LIST;
  setup_req->gnb_du_served_cells_list.crit    = asn1::crit_opts::reject;

  setup_req->gnb_du_served_cells_list.value.push_back(generate_served_cells_item(12345678, 0));

  return f1_setup_request;
}

f1c_message generate_f1_init_ul_rrc_msg_base(unsigned int c_rnti)
{
  f1c_message init_ul_rrc_msg = {};

  init_ul_rrc_msg.pdu.set_init_msg();
  init_ul_rrc_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_INIT_ULRRC_MSG_TRANSFER);

  auto& init_ul_rrc                     = init_ul_rrc_msg.pdu.init_msg().value.init_ulrrc_msg_transfer();
  init_ul_rrc->gnb_du_ue_f1_ap_id.value = c_rnti; // same as C-RNTI

  init_ul_rrc->nrcgi.value.nrcell_id.from_string("000000000000101111000110000101001110"); // 12345678 in decimal
  init_ul_rrc->nrcgi.value.plmn_id.from_string("02f899");
  init_ul_rrc->c_rnti.value = c_rnti;

  init_ul_rrc->sul_access_ind_present     = true;
  init_ul_rrc->sul_access_ind.value.value = asn1::f1ap::sul_access_ind_opts::options::true_value;

  init_ul_rrc->rrc_container.value.from_string("1dec89d05766");

  return init_ul_rrc_msg;
}

f1c_message generate_valid_f1_init_ul_rrc_msg(unsigned int c_rnti)
{
  f1c_message init_ul_rrc_msg = generate_f1_init_ul_rrc_msg_base(c_rnti);

  auto& init_ul_rrc                         = init_ul_rrc_msg.pdu.init_msg().value.init_ulrrc_msg_transfer();
  init_ul_rrc->duto_currc_container_present = true;
  init_ul_rrc->duto_currc_container.value.from_string(
      "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
      "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
      "c400");

  return init_ul_rrc_msg;
}

f1c_message generate_valid_rrc_setup_complete_msg(unsigned int cu_ue_id, unsigned int c_rnti)
{
  f1c_message ul_rrc_msg = {};

  ul_rrc_msg.pdu.set_init_msg();
  ul_rrc_msg.pdu.init_msg().load_info_obj(ASN1_F1AP_ID_ULRRC_MSG_TRANSFER);

  auto& ul_rrc                     = ul_rrc_msg.pdu.init_msg().value.ulrrc_msg_transfer();
  ul_rrc->gnb_du_ue_f1_ap_id.value = c_rnti; // same as C-RNTI

  ul_rrc->gnb_cu_ue_f1_ap_id.value = cu_ue_id;
  ul_rrc->srbid.value              = 1;
  ul_rrc->rrc_container.value.from_string(
      "000010c01000082727e01c3ff100c047e004139000bf202f8998000410000000f2e04f070f0707100517e004139000bf202f899800041000"
      "0000f1001032e04f070f0702f1b08010027db00000000080101b669000000000801000001000000005202f8990000011707f070c0401980b"
      "018010174000090530101000000000");

  return ul_rrc_msg;
}

f1c_message generate_f1_ue_context_release_complete_msg(unsigned int cu_ue_id, unsigned int c_rnti)
{
  f1c_message ue_ctxt_rel_complete_msg = {};
  ue_ctxt_rel_complete_msg.pdu.set_successful_outcome();
  ue_ctxt_rel_complete_msg.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  auto& rel_complete_msg = ue_ctxt_rel_complete_msg.pdu.successful_outcome().value.ue_context_release_complete();
  rel_complete_msg->gnb_cu_ue_f1_ap_id.value = cu_ue_id;
  rel_complete_msg->gnb_du_ue_f1_ap_id.value = c_rnti;

  return ue_ctxt_rel_complete_msg;
}

f1c_message generate_ue_context_setup_response_msg(unsigned int cu_ue_id, unsigned int c_rnti)
{
  f1c_message ue_context_setup_response = {};

  ue_context_setup_response.pdu.set_successful_outcome();
  ue_context_setup_response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);

  auto& ue_context_setup_resp = ue_context_setup_response.pdu.successful_outcome().value.ue_context_setup_resp();
  ue_context_setup_resp->gnb_cu_ue_f1_ap_id.value = cu_ue_id;
  ue_context_setup_resp->gnb_du_ue_f1_ap_id.value = c_rnti;
  ue_context_setup_resp->c_rnti_present           = true;
  ue_context_setup_resp->c_rnti.value             = c_rnti;

  return ue_context_setup_response;
}

f1c_message generate_ue_context_setup_failure_msg(unsigned int cu_ue_id, unsigned int c_rnti)
{
  f1c_message ue_context_setup_failure = {};

  ue_context_setup_failure.pdu.set_unsuccessful_outcome();
  ue_context_setup_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_SETUP);

  auto& ue_context_setup_fail = ue_context_setup_failure.pdu.unsuccessful_outcome().value.ue_context_setup_fail();
  ue_context_setup_fail->gnb_cu_ue_f1_ap_id.value = cu_ue_id;
  ue_context_setup_fail->gnb_du_ue_f1_ap_id.value = c_rnti;
  ue_context_setup_fail->cause.value.set_radio_network();
  ue_context_setup_fail->cause.value.radio_network() =
      asn1::f1ap::cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;
  ;

  return ue_context_setup_failure;
}

f1c_message generate_ue_context_modification_response_msg(unsigned int cu_ue_id, unsigned int c_rnti)
{
  f1c_message ue_context_modification_response = {};

  ue_context_modification_response.pdu.set_successful_outcome();
  ue_context_modification_response.pdu.successful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);

  auto& ue_context_mod_resp = ue_context_modification_response.pdu.successful_outcome().value.ue_context_mod_resp();
  ue_context_mod_resp->gnb_cu_ue_f1_ap_id.value = cu_ue_id;
  ue_context_mod_resp->gnb_du_ue_f1_ap_id.value = c_rnti;
  ue_context_mod_resp->c_rnti_present           = true;
  ue_context_mod_resp->c_rnti.value             = c_rnti;

  return ue_context_modification_response;
}

f1c_message generate_ue_context_modification_failure_msg(unsigned int cu_ue_id, unsigned int c_rnti)
{
  f1c_message ue_context_modification_failure = {};

  ue_context_modification_failure.pdu.set_unsuccessful_outcome();
  ue_context_modification_failure.pdu.unsuccessful_outcome().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);

  auto& ue_context_mod_fail = ue_context_modification_failure.pdu.unsuccessful_outcome().value.ue_context_mod_fail();
  ue_context_mod_fail->gnb_cu_ue_f1_ap_id.value = cu_ue_id;
  ue_context_mod_fail->gnb_du_ue_f1_ap_id.value = c_rnti;
  ue_context_mod_fail->cause.value.set_radio_network();
  ue_context_mod_fail->cause.value.radio_network() =
      asn1::f1ap::cause_radio_network_opts::options::unknown_or_already_allocated_gnb_cu_ue_f1ap_id;
  ;

  return ue_context_modification_failure;
}

} // namespace srs_cu_cp
} // namespace srsgnb
