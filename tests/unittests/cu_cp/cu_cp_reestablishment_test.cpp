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
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include "tests/test_doubles/rrc/rrc_test_messages.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_reestablishment_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_reestablishment_test() : cu_cp_test_environment(cu_cp_test_env_params{8, 8, create_mock_amf()})
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

    // Connect UE.
    EXPECT_TRUE(connect_new_ue(du_idx, old_du_ue_id, old_crnti));
  }

  /// Run RRC Reestablishment.
  /// \return Returns true if Reestablishment is successful, and false if the gNB fallbacked to RRC Setup.
  bool run_rrc_reestablishment(gnb_du_ue_f1ap_id_t new_du_ue_id, rnti_t new_rnti, rnti_t old_rnti_, pci_t old_pci_)
  {
    // Generate RRC Reestablishment Request.
    byte_buffer rrc_container = pack_ul_ccch_msg(create_rrc_reestablishment_request(old_rnti_, old_pci_));

    // Send Initial UL RRC Message to CU-CP.
    f1ap_message f1ap_init_ul_rrc_msg =
        test_helpers::create_init_ul_rrc_message_transfer(new_du_ue_id, new_rnti, {}, std::move(rrc_container));
    get_du(du_idx).push_tx_pdu(f1ap_init_ul_rrc_msg);

    // Wait for DL RRC message transfer
    f1ap_message f1ap_pdu;
    bool         result = this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu, std::chrono::milliseconds{1000});
    EXPECT_TRUE(result);

    // Check that the received message is a DL RRC Message Transfer with RRC Reestablishment.
    EXPECT_EQ(f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
    EXPECT_EQ(f1ap_pdu.pdu.init_msg().value.type().value,
              asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::dl_rrc_msg_transfer);

    auto& dl_rrc_msg = *f1ap_pdu.pdu.init_msg().value.dl_rrc_msg_transfer();
    EXPECT_FALSE(dl_rrc_msg.rrc_container.empty());
    EXPECT_EQ(int_to_gnb_du_ue_f1ap_id(dl_rrc_msg.gnb_du_ue_f1ap_id), new_du_ue_id);
    EXPECT_LE(dl_rrc_msg.srb_id, 1);

    if (dl_rrc_msg.srb_id == 0) {
      // It is RRC Setup
      return false;
    }

    // It is RRC Reestablishment.
    EXPECT_TRUE(dl_rrc_msg.old_gnb_du_ue_f1ap_id_present);
    EXPECT_NE(dl_rrc_msg.old_gnb_du_ue_f1ap_id, dl_rrc_msg.old_gnb_du_ue_f1ap_id);

    return true;
  }

  unsigned du_idx    = 0;
  unsigned cu_up_idx = 0;

  gnb_du_ue_f1ap_id_t old_du_ue_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              old_crnti    = to_rnti(0x4601);
};

TEST_F(cu_cp_reestablishment_test, when_old_ue_does_not_exist_then_reestablishment_fails)
{
  ASSERT_FALSE(run_rrc_reestablishment(int_to_gnb_du_ue_f1ap_id(1), to_rnti(0x4602), to_rnti(0x4603), 0))
      << "RRC setup should have been sent";

  // old UE should not be removed at this stage.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 2);
}

TEST_F(cu_cp_reestablishment_test, when_old_ue_has_no_drb1_or_srb2_then_reestablishment_fails)
{
  ASSERT_FALSE(run_rrc_reestablishment(int_to_gnb_du_ue_f1ap_id(1), to_rnti(0x4602), to_rnti(0x4601), 0))
      << "RRC setup should have been sent";

  // old UE should be removed.
  // old UE should not be removed at this stage.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 2);
}
