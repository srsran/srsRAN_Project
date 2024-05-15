/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_setup_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_setup_test() : cu_cp_test_environment(cu_cp_test_env_params{8, 8, create_mock_amf()})
  {
    // Run NG setup to completion.
    run_ng_setup();

    // Setup DU.
    auto ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    EXPECT_TRUE(this->run_f1_setup(du_idx));

    // Setup CU-UP.
    ret = connect_new_cu_up();
    EXPECT_TRUE(ret.has_value());
    cu_up_idx = ret.value();
    EXPECT_TRUE(this->run_e1_setup(cu_up_idx));
  }

protected:
  unsigned                      du_idx    = 0;
  unsigned                      cu_up_idx = 0;
  optional<gnb_cu_ue_f1ap_id_t> cu_ue_id;
};

TEST_F(cu_cp_setup_test, when_new_ue_sends_rrc_setup_request_then_dl_rrc_message_sent_with_rrc_setup)
{
  // Create UE by sending Initial UL RRC Message.
  gnb_du_ue_f1ap_id_t du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti         = to_rnti(0x4601);
  get_du(du_idx).push_ul_pdu(generate_init_ul_rrc_message_transfer(du_ue_f1ap_id, crnti));

  // Verify F1AP DL RRC Message is sent with RRC Setup.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));

  // Check if DL RRC Message Transfer (containing RRC Setup) is valid.
  ASSERT_TRUE(test_helpers::is_valid_dl_rrc_message_transfer_with_msg4(f1ap_pdu));
  const auto& dl_rrc_msg = f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer();
  ASSERT_EQ(int_to_gnb_du_ue_f1ap_id(dl_rrc_msg->gnb_du_ue_f1ap_id), du_ue_f1ap_id);
  ASSERT_EQ(int_to_srb_id(dl_rrc_msg->srb_id), srb_id_t::srb0);
  cu_ue_id = int_to_gnb_cu_ue_f1ap_id(dl_rrc_msg->gnb_cu_ue_f1ap_id);

  // Check UE is created.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1);
  ASSERT_EQ(report.ues[0].rnti, crnti);
  ASSERT_EQ(report.ues[0].du_id, report.dus[0].id);
  ASSERT_EQ(report.ues[0].pci, report.dus[0].cells[0].pci);
}

TEST_F(cu_cp_setup_test, when_cu_cp_receives_f1_removal_request_then_rrc_setup_procedure_is_cancelled)
{
  // Create UE by sending Initial UL RRC Message.
  gnb_du_ue_f1ap_id_t du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti         = to_rnti(0x4601);
  get_du(du_idx).push_ul_pdu(generate_init_ul_rrc_message_transfer(du_ue_f1ap_id, crnti));

  // Verify F1AP DL RRC Message is sent with RRC Setup.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));

  // DU sends F1 Removal Request.
  get_du(du_idx).push_ul_pdu(test_helpers::generate_f1_removal_request(0));

  // Await F1 Removal Response.
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));

  // Check UE is deleted.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0);
}
