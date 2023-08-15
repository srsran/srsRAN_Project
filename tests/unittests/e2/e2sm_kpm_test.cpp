/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/e2/common/e2ap_asn1_packer.h"
#include "lib/e2/common/e2ap_asn1_utils.h"
#include "tests/unittests/e2/common/e2_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

#define PCAP_OUTPUT 0

class e2sm_kpm_indication : public e2_test_base
{
  void SetUp() override
  {
    srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
    srslog::init();

    cfg                  = srsran::config_helpers::make_default_e2ap_config();
    cfg.e2sm_kpm_enabled = true;

    du_metrics       = std::make_unique<dummy_e2_du_metrics>();
    du_meas_provider = std::make_unique<dummy_e2sm_kpm_du_meas_provider>();
    e2sm_packer      = std::make_unique<e2sm_kpm_asn1_packer>(*du_meas_provider);
    e2sm_iface       = std::make_unique<e2sm_kpm_impl>(test_logger, *e2sm_packer, *du_meas_provider);
    gw               = std::make_unique<dummy_network_gateway_data_handler>();
    pcap             = std::make_unique<dummy_e2ap_pcap>();
    packer           = std::make_unique<srsran::e2ap_asn1_packer>(*gw, *e2, *pcap);
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
    pcap->close();
  }
};

#if PCAP_OUTPUT
std::unique_ptr<dlt_pcap> g_pcap = std::make_unique<dlt_pcap_impl>(PCAP_E2AP_DLT, "E2AP");

inline void save_msg_pcap(const byte_buffer& last_pdu)
{
  if (not g_pcap->is_write_enabled()) {
    g_pcap->open("e2sm_kpm_test.pcap");
  }
  g_pcap->push_pdu(last_pdu.copy());
  usleep(200);
}
#endif

#if PCAP_OUTPUT
TEST_F(e2_entity_test, e2sm_kpm_generates_ran_func_desc)
{
  // We need this test to generate E2 Setup Request, so Wireshark can decode the following RIC indication messages.
  test_logger.info("Launch e2 setup request procedure with task worker...");
  e2->start();

  save_msg_pcap(gw->last_pdu);
}
#endif

TEST_F(e2sm_kpm_indication, e2sm_kpm_generates_ric_indication_style1)
{
  // Define E2SM_KPM action format 1.
  e2_sm_kpm_action_definition_s action_def;
  action_def.ric_style_type = 1;
  e2_sm_kpm_action_definition_format1_s& action_def_f1 =
      action_def.action_definition_formats.set_action_definition_format1();
  action_def_f1.cell_global_id_present = false;
  action_def_f1.granul_period          = 100;

  meas_info_item_s meas_info_item;
  meas_info_item.meas_type.set_meas_name().from_string("DRB.UEThpDl");
  label_info_item_s label_info_item;
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_opts::true_value;
  meas_info_item.label_info_list.push_back(label_info_item);
  action_def_f1.meas_info_list.push_back(meas_info_item);

  asn1::e2ap::ri_caction_to_be_setup_item_s ric_action = generate_e2sm_kpm_ric_action(action_def);

#if PCAP_OUTPUT
  // Save E2 Subscription Request.
  e2_message e2_subscript_req = generate_e2sm_kpm_subscription_request(ric_action);
  packer->handle_message(e2_subscript_req);
  save_msg_pcap(gw->last_pdu);
#endif

  ASSERT_TRUE(e2sm_iface->action_supported(ric_action));
  auto report_service = e2sm_iface->get_e2sm_report_service(ric_action.ric_action_definition);

  // Trigger measurement collection.
  report_service->collect_measurements();

  // Get RIC indication msg content.
  byte_buffer ind_hdr_bytes = report_service->get_indication_header();
  byte_buffer ind_msg_bytes = report_service->get_indication_message();

  // Decode RIC Indication and check the content.
  e2_sm_kpm_ind_msg_s ric_ind_msg;
  asn1::cbit_ref      ric_ind_bref(ind_msg_bytes);
  if (ric_ind_msg.unpack(ric_ind_bref) != asn1::SRSASN_SUCCESS) {
    test_logger.debug("e2sm_kpm: RIC indication msg could not be unpacked");
    return;
  }

  TESTASSERT_EQ(1, ric_ind_msg.ind_msg_formats.ind_msg_format1().meas_data.size());
  TESTASSERT_EQ(1, ric_ind_msg.ind_msg_formats.ind_msg_format1().meas_data[0].meas_record.size());

#if PCAP_OUTPUT
  e2_message e2_msg = generate_e2_ind_msg(ind_hdr_bytes, ind_msg_bytes);
  packer->handle_message(e2_msg);
  save_msg_pcap(gw->last_pdu);
#endif
}

TEST_F(e2sm_kpm_indication, e2sm_kpm_generates_ric_indication_style2)
{
  // Define E2SM_KPM action format 2.
  e2_sm_kpm_action_definition_s action_def;
  action_def.ric_style_type = 2;
  e2_sm_kpm_action_definition_format2_s& action_def_f2 =
      action_def.action_definition_formats.set_action_definition_format2();

  ueid_c& ueid           = action_def_f2.ue_id;
  ueid.set_gnb_du_ueid() = generate_ueid_gnb_du(1);

  action_def_f2.subscript_info.cell_global_id_present = false;
  action_def_f2.subscript_info.granul_period          = 100;

  meas_info_item_s meas_info_item;
  meas_info_item.meas_type.set_meas_name().from_string("DRB.UEThpDl");
  label_info_item_s label_info_item;
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_opts::true_value;
  meas_info_item.label_info_list.push_back(label_info_item);
  action_def_f2.subscript_info.meas_info_list.push_back(meas_info_item);

  asn1::e2ap::ri_caction_to_be_setup_item_s ric_action = generate_e2sm_kpm_ric_action(action_def);

#if PCAP_OUTPUT
  // Save E2 Subscription Request.
  e2_message e2_subscript_req = generate_e2sm_kpm_subscription_request(ric_action);
  packer->handle_message(e2_subscript_req);
  save_msg_pcap(gw->last_pdu);
#endif

  ASSERT_TRUE(e2sm_iface->action_supported(ric_action));
  auto report_service = e2sm_iface->get_e2sm_report_service(ric_action.ric_action_definition);

  // Trigger measurement collection.
  report_service->collect_measurements();

  // Get RIC indication msg content.
  byte_buffer ind_hdr_bytes = report_service->get_indication_header();
  byte_buffer ind_msg_bytes = report_service->get_indication_message();

  // Decode RIC Indication and check the content.
  e2_sm_kpm_ind_msg_s ric_ind_msg;
  asn1::cbit_ref      ric_ind_bref(ind_msg_bytes);
  if (ric_ind_msg.unpack(ric_ind_bref) != asn1::SRSASN_SUCCESS) {
    test_logger.debug("e2sm_kpm: RIC indication msg could not be unpacked");
    return;
  }

  TESTASSERT_EQ(1, ric_ind_msg.ind_msg_formats.ind_msg_format1().meas_data.size());
  TESTASSERT_EQ(1, ric_ind_msg.ind_msg_formats.ind_msg_format1().meas_data[0].meas_record.size());

#if PCAP_OUTPUT
  e2_message e2_msg = generate_e2_ind_msg(ind_hdr_bytes, ind_msg_bytes);
  packer->handle_message(e2_msg);
  save_msg_pcap(gw->last_pdu);
#endif
}

TEST_F(e2sm_kpm_indication, e2sm_kpm_generates_ric_indication_style3)
{
  // Define E2SM_KPM action format 3.
  e2_sm_kpm_action_definition_s action_def;
  action_def.ric_style_type = 3;
  e2_sm_kpm_action_definition_format3_s& action_def_f3 =
      action_def.action_definition_formats.set_action_definition_format3();

  meas_cond_item_s meas_cond_item;
  meas_cond_item.meas_type.set_meas_name().from_string("DRB.UEThpDl");

  // Report UEThpDl for all UEs with RSRP > -110 and RSRP < -50.
  // Add conditions, order is important. Labels has to be first.
  matching_cond_item_s matching_cond_item1;
  meas_label_s         meas_label1;
  meas_label1.five_qi_present                               = true;
  meas_label1.five_qi                                       = 1;
  matching_cond_item1.lc_or_present                         = true; // if false use OR, if true then use AND
  matching_cond_item1.lc_or                                 = lc_or_opts::true_value;
  matching_cond_item1.matching_cond_choice.set_meas_label() = meas_label1;
  meas_cond_item.matching_cond.push_back(matching_cond_item1);

  matching_cond_item_s matching_cond_item2;
  test_cond_info_s     test_cond_info1;
  test_cond_info1.test_type.set_ul_r_srp().value = test_cond_type_c::ul_r_srp_opts::true_value;
  test_cond_info1.test_expr_present              = true;
  test_cond_info1.test_expr                      = test_cond_expression_opts::greaterthan;
  // TODO: seems that asn1 does not suppport negative numbers
  test_cond_info1.test_value_present                            = true;
  test_cond_info1.test_value.set_value_int()                    = 50;
  matching_cond_item2.lc_or_present                             = true; // if false use OR, if true then use AND
  matching_cond_item2.lc_or                                     = lc_or_opts::true_value;
  matching_cond_item2.matching_cond_choice.set_test_cond_info() = test_cond_info1;
  meas_cond_item.matching_cond.push_back(matching_cond_item2);

  matching_cond_item_s matching_cond_item3;
  test_cond_info_s     test_cond_info2;
  test_cond_info2.test_type.set_ul_r_srp().value = test_cond_type_c::ul_r_srp_opts::true_value;
  test_cond_info2.test_expr_present              = true;
  test_cond_info2.test_value_present             = true;
  test_cond_info2.test_expr                      = test_cond_expression_opts::lessthan;
  // TODO: seems that asn1 does not suppport negative numbers
  test_cond_info2.test_value.set_value_int()                    = 110;
  matching_cond_item3.lc_or_present                             = false; // if false use OR, if true then use AND
  matching_cond_item3.matching_cond_choice.set_test_cond_info() = test_cond_info2;
  meas_cond_item.matching_cond.push_back(matching_cond_item3);

  // Put all conditions together for a single metric.
  action_def_f3.meas_cond_list.push_back(meas_cond_item);
  action_def_f3.cell_global_id_present = false;
  action_def_f3.granul_period          = 100;

  asn1::e2ap::ri_caction_to_be_setup_item_s ric_action = generate_e2sm_kpm_ric_action(action_def);

#if PCAP_OUTPUT
  // Save E2 Subscription Request.
  e2_message e2_subscript_req = generate_e2sm_kpm_subscription_request(ric_action);
  packer->handle_message(e2_subscript_req);
  save_msg_pcap(gw->last_pdu);
#endif

  ASSERT_TRUE(e2sm_iface->action_supported(ric_action));
  auto report_service = e2sm_iface->get_e2sm_report_service(ric_action.ric_action_definition);

  // Trigger measurement collection.
  report_service->collect_measurements();

  // Get RIC indication msg content.
  byte_buffer ind_hdr_bytes = report_service->get_indication_header();
  byte_buffer ind_msg_bytes = report_service->get_indication_message();

  // Decode RIC Indication and check the content.
  e2_sm_kpm_ind_msg_s ric_ind_msg;
  asn1::cbit_ref      ric_ind_bref(ind_msg_bytes);
  if (ric_ind_msg.unpack(ric_ind_bref) != asn1::SRSASN_SUCCESS) {
    test_logger.debug("e2sm_kpm: RIC indication msg could not be unpacked");
    return;
  }

  // TESTASSERT_EQ(1, ric_ind_msg.ind_msg_formats.ind_msg_format2().meas_data.size());
  // TESTASSERT_EQ(1, ric_ind_msg.ind_msg_formats.ind_msg_format2().meas_data[0].meas_record.size());

#if PCAP_OUTPUT
  e2_message e2_msg = generate_e2_ind_msg(ind_hdr_bytes, ind_msg_bytes);
  packer->handle_message(e2_msg);
  save_msg_pcap(gw->last_pdu);
#endif
}

TEST_F(e2sm_kpm_indication, e2sm_kpm_generates_ric_indication_style4)
{
  // Define E2SM_KPM action format 4.
  e2_sm_kpm_action_definition_s action_def;
  action_def.ric_style_type = 4;
  e2_sm_kpm_action_definition_format4_s& action_def_f4 =
      action_def.action_definition_formats.set_action_definition_format4();

  // Report UEThpDl for all UEs with rsrp > -110 and rsrp < -50.
  matching_ue_cond_per_sub_item_s matching_ue_cond_item1;
  matching_ue_cond_item1.test_cond_info.test_type.set_ul_r_srp().value = test_cond_type_c::ul_r_srp_opts::true_value;
  matching_ue_cond_item1.test_cond_info.test_expr_present              = true;
  matching_ue_cond_item1.test_cond_info.test_value_present             = true;
  matching_ue_cond_item1.test_cond_info.test_expr                      = test_cond_expression_opts::greaterthan;
  // TODO: seems that asn1 does not suppport negative numbers
  matching_ue_cond_item1.test_cond_info.test_value.set_value_int() = 50;
  matching_ue_cond_item1.lc_or_present                             = false; // if false use OR, if true then use AND
  action_def_f4.matching_ue_cond_list.push_back(matching_ue_cond_item1);

  matching_ue_cond_per_sub_item_s matching_ue_cond_item2;
  matching_ue_cond_item2.test_cond_info.test_type.set_ul_r_srp().value = test_cond_type_c::ul_r_srp_opts::true_value;
  matching_ue_cond_item2.test_cond_info.test_expr_present              = true;
  matching_ue_cond_item2.test_cond_info.test_value_present             = true;
  matching_ue_cond_item2.test_cond_info.test_expr                      = test_cond_expression_opts::lessthan;
  // TODO: seems that asn1 does not suppport negative numbers
  matching_ue_cond_item2.test_cond_info.test_value.set_value_int() = 110;
  action_def_f4.matching_ue_cond_list.push_back(matching_ue_cond_item2);

  e2_sm_kpm_action_definition_format1_s& subscription_info = action_def_f4.subscription_info;
  subscription_info.cell_global_id_present                 = false;
  subscription_info.granul_period                          = 100;

  meas_info_item_s meas_info_item;
  meas_info_item.meas_type.set_meas_name().from_string("DRB.UEThpDl");
  label_info_item_s label_info_item;
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_opts::true_value;
  meas_info_item.label_info_list.push_back(label_info_item);
  subscription_info.meas_info_list.push_back(meas_info_item);

  asn1::e2ap::ri_caction_to_be_setup_item_s ric_action = generate_e2sm_kpm_ric_action(action_def);

#if PCAP_OUTPUT
  // Save E2 Subscription Request.
  e2_message e2_subscript_req = generate_e2sm_kpm_subscription_request(ric_action);
  packer->handle_message(e2_subscript_req);
  save_msg_pcap(gw->last_pdu);
#endif

  ASSERT_TRUE(e2sm_iface->action_supported(ric_action));
  auto report_service = e2sm_iface->get_e2sm_report_service(ric_action.ric_action_definition);

  // Trigger measurement collection.
  report_service->collect_measurements();

  // Get RIC indication msg content.
  byte_buffer ind_hdr_bytes = report_service->get_indication_header();
  byte_buffer ind_msg_bytes = report_service->get_indication_message();

  // Decode RIC Indication and check the content.
  e2_sm_kpm_ind_msg_s ric_ind_msg;
  asn1::cbit_ref      ric_ind_bref(ind_msg_bytes);
  if (ric_ind_msg.unpack(ric_ind_bref) != asn1::SRSASN_SUCCESS) {
    test_logger.debug("e2sm_kpm: RIC indication msg could not be unpacked");
    return;
  }

  // TESTASSERT_EQ(1, ric_ind_msg.ind_msg_formats.ind_msg_format2().meas_data.size());
  // TESTASSERT_EQ(1, ric_ind_msg.ind_msg_formats.ind_msg_format2().meas_data[0].meas_record.size());

#if PCAP_OUTPUT
  e2_message e2_msg = generate_e2_ind_msg(ind_hdr_bytes, ind_msg_bytes);
  packer->handle_message(e2_msg);
  save_msg_pcap(gw->last_pdu);
#endif
}

TEST_F(e2sm_kpm_indication, e2sm_kpm_generates_ric_indication_style5)
{
  // Define E2SM_KPM action format 5.
  e2_sm_kpm_action_definition_s action_def;
  action_def.ric_style_type = 5;
  e2_sm_kpm_action_definition_format5_s& action_def_f5 =
      action_def.action_definition_formats.set_action_definition_format5();

  action_def_f5.matching_ueid_list.resize(2);
  action_def_f5.matching_ueid_list[0].ue_id.set_gnb_du_ueid() = generate_ueid_gnb_du(1);
  action_def_f5.matching_ueid_list[1].ue_id.set_gnb_du_ueid() = generate_ueid_gnb_du(2);

  e2_sm_kpm_action_definition_format1_s& subscript_info = action_def_f5.subscription_info;
  subscript_info.cell_global_id_present                 = false;
  subscript_info.granul_period                          = 100;

  meas_info_item_s meas_info_item;
  meas_info_item.meas_type.set_meas_name().from_string("DRB.UEThpDl");
  label_info_item_s label_info_item;
  label_info_item.meas_label.no_label_present = true;
  label_info_item.meas_label.no_label         = meas_label_s::no_label_opts::true_value;
  meas_info_item.label_info_list.push_back(label_info_item);

  subscript_info.meas_info_list.push_back(meas_info_item);

  asn1::e2ap::ri_caction_to_be_setup_item_s ric_action = generate_e2sm_kpm_ric_action(action_def);

#if PCAP_OUTPUT
  // Save E2 Subscription Request.
  e2_message e2_subscript_req = generate_e2sm_kpm_subscription_request(ric_action);
  packer->handle_message(e2_subscript_req);
  save_msg_pcap(gw->last_pdu);
#endif

  ASSERT_TRUE(e2sm_iface->action_supported(ric_action));
  auto report_service = e2sm_iface->get_e2sm_report_service(ric_action.ric_action_definition);

  // Trigger measurement collection.
  report_service->collect_measurements();

  // Get RIC indication msg content.
  byte_buffer ind_hdr_bytes = report_service->get_indication_header();
  byte_buffer ind_msg_bytes = report_service->get_indication_message();

  // Decode RIC Indication and check the content.
  e2_sm_kpm_ind_msg_s ric_ind_msg;
  asn1::cbit_ref      ric_ind_bref(ind_msg_bytes);
  if (ric_ind_msg.unpack(ric_ind_bref) != asn1::SRSASN_SUCCESS) {
    test_logger.debug("e2sm_kpm: RIC indication msg could not be unpacked");
    return;
  }

  // TESTASSERT_EQ(1, ric_ind_msg.ind_msg_formats.ind_msg_format2().meas_data.size());
  // TESTASSERT_EQ(1, ric_ind_msg.ind_msg_formats.ind_msg_format2().meas_data[0].meas_record.size());

#if PCAP_OUTPUT
  e2_message e2_msg = generate_e2_ind_msg(ind_hdr_bytes, ind_msg_bytes);
  packer->handle_message(e2_msg);
  save_msg_pcap(gw->last_pdu);
#endif
}