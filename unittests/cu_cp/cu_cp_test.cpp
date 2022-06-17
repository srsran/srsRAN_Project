/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../lib/cu_cp/cu_cp.h"
#include "srsgnb/support/test_utils.h"
#include "unittests/f1_interface/common/test_helpers.h"

using namespace srsgnb;
using namespace srs_cu_cp;

/// Test the f1 setup procedure
void test_f1_setup()
{
  test_delimit_logger   delimiter{"Test F1 setup procedure in CU-CP"};
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  // create worker thread and executer
  task_worker                    task_worker("thread", 1, false, os_thread_realtime_priority::MAX_PRIO);
  std::unique_ptr<task_executor> task_executor = make_task_executor(task_worker);

  dummy_f1c_pdu_notifier f1c_pdu_notifier(nullptr);

  // create CU-CP config
  cu_cp_configuration cfg;
  cfg.cu_executor = task_executor.get();
  cfg.f1c_notifier = &f1c_pdu_notifier;

  // create and start DUT
  cu_cp cu_cp_obj(cfg);
  cu_cp_obj.start();

  // Handling of F1SetupRequest
  {
    asn1::f1ap::f1_ap_pdu_c pdu;

    pdu.set_init_msg();
    pdu.init_msg().load_info_obj(ASN1_F1AP_ID_F1_SETUP);

    auto& setup_req                 = pdu.init_msg().value.f1_setup_request();
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
        "92002808241099000001000000000a4213407800008c98d6d8d7f616e0804000020107e28180008000088a0dc7008000088a0007141a22"
        "81c874cc00020000232d5c6b6c65462001ec4cc5fc9c0493946a98d4d1e99355c00a1aba010580ec024646f62180");

    setup_req->gnb_du_served_cells_list.value.push_back(served_cells_item_container);

    // Pass PDU to CU-CP
    test_logger.info("Injecting F1SetupRequest");
    cu_cp_obj.get_f1c_message_handler().handle_message(pdu);

    // check that DU has been added
    TESTASSERT_EQ(1, cu_cp_obj.get_nof_dus());
  }

  // Check response is F1SetupResponse
  TESTASSERT_EQ(asn1::f1ap::f1_ap_pdu_c::types_opts::options::successful_outcome, f1c_pdu_notifier.last_pdu.type());
  TESTASSERT_EQ(asn1::f1ap::f1_ap_elem_procs_o::successful_outcome_c::types_opts::options::f1_setup_resp,
                f1c_pdu_notifier.last_pdu.successful_outcome().value.type());

  return;
}

void test_initial_ul_rrc_message_transfer()
{
  test_delimit_logger   delimiter{"Test F1 setup procedure in CU-CP"};
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  // create worker thread and executer
  task_worker                    task_worker("thread", 1, false, os_thread_realtime_priority::MAX_PRIO);
  std::unique_ptr<task_executor> task_executor = make_task_executor(task_worker);

  dummy_f1c_pdu_notifier f1c_pdu_notifier(nullptr);

  // create CU-CP config
  cu_cp_configuration cfg;
  cfg.cu_executor  = task_executor.get();
  cfg.f1c_notifier = &f1c_pdu_notifier;

  // create and start DUT
  cu_cp cu_cp_obj(cfg);
  cu_cp_obj.start();

  // Handling of Initial UL RRC message transfer
  {
    asn1::f1ap::f1_ap_pdu_c pdu;

    pdu.set_init_msg();
    pdu.init_msg().load_info_obj(ASN1_F1AP_ID_INIT_ULRRC_MSG_TRANSFER);

    auto& init_ul_rrc                     = pdu.init_msg().value.init_ulrrc_msg_transfer();
    init_ul_rrc->gnb_du_ue_f1_ap_id.value = 41255; // same as C-RNTI

    init_ul_rrc->nrcgi.value.nrcell_id.from_string("000000000000101111000110000101001110"); // 12345678 in decimal
    init_ul_rrc->nrcgi.value.plmn_id.from_string("02f899");
    init_ul_rrc->c_rnti.value = 41255;

    init_ul_rrc->rrc_container.value.from_string("1dec89d05766");
    init_ul_rrc->duto_currc_container_present = true;
    init_ul_rrc->duto_currc_container.value.from_string(
        "5c00b001117aec701061e0007c20408d07810020a2090480ca8000f800000000008370842000088165000048200002069a06aa49880002"
        "00204000400d008013b64b1814400e468acf120000096070820f177e060870000000e25038000040bde802000400000000028201950300"
        "c400");

    // Pass PDU to CU-CP
    test_logger.info("Injecting Initial UL RRC message");
    cu_cp_obj.get_f1c_message_handler().handle_message(pdu);

    // TODO: add checks
  }
}

int main()
{
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);

  srslog::init();

  test_f1_setup();
  test_initial_ul_rrc_message_transfer();

  return 0;
}