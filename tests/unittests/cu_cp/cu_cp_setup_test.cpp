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

#include "cu_cp_test_environment.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/f1ap/f1ap_test_messages.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_setup_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_setup_test() : cu_cp_test_environment(cu_cp_test_env_params{8, 8, 8192, create_mock_amf()})
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
  unsigned                           du_idx    = 0;
  unsigned                           cu_up_idx = 0;
  std::optional<gnb_cu_ue_f1ap_id_t> cu_ue_id;
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

TEST_F(cu_cp_setup_test, when_du_hangs_up_then_rrc_setup_procedure_is_cancelled)
{
  // Create UE by sending Initial UL RRC Message.
  gnb_du_ue_f1ap_id_t du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti         = to_rnti(0x4601);
  get_du(du_idx).push_ul_pdu(generate_init_ul_rrc_message_transfer(du_ue_f1ap_id, crnti));

  // Verify F1AP DL RRC Message is sent with RRC Setup.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));

  // DU hangs up.
  ASSERT_TRUE(drop_du_connection(du_idx));

  // Check UE is deleted.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 0);
}

TEST_F(cu_cp_setup_test, when_initial_ul_rrc_message_has_no_rrc_container_then_ue_is_rejected)
{
  // Event: DU sends Initial UL RRC Message without DU-to-CU-RRC container.
  gnb_du_ue_f1ap_id_t du_ue_f1ap_id = int_to_gnb_du_ue_f1ap_id(0);
  rnti_t              crnti         = to_rnti(0x4601);
  f1ap_message        init_rrc_msg  = generate_init_ul_rrc_message_transfer(du_ue_f1ap_id, crnti);
  init_rrc_msg.pdu.init_msg().value.init_ul_rrc_msg_transfer()->du_to_cu_rrc_container_present = false;
  get_du(du_idx).push_ul_pdu(init_rrc_msg);

  // TEST: CU-CP sends a UE Context Release command over SRB0.
  f1ap_message f1ap_pdu;
  ASSERT_TRUE(this->wait_for_f1ap_tx_pdu(du_idx, f1ap_pdu));
  ASSERT_EQ(f1ap_pdu.pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(f1ap_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_release_cmd);
  const auto& ue_rel = f1ap_pdu.pdu.init_msg().value.ue_context_release_cmd();
  ASSERT_TRUE(ue_rel->rrc_container_present);
  // check that the SRB ID is set if the RRC Container is included
  ASSERT_TRUE(ue_rel->srb_id_present);
  ASSERT_EQ(ue_rel->srb_id, 0);

  // TEST: UE Context Release command contains an RRC Reject.
  asn1::rrc_nr::dl_ccch_msg_s ccch;
  {
    asn1::cbit_ref bref{f1ap_pdu.pdu.init_msg().value.ue_context_release_cmd()->rrc_container};
    ASSERT_EQ(ccch.unpack(bref), asn1::SRSASN_SUCCESS);
  }
  ASSERT_EQ(ccch.msg.c1().type().value, asn1::rrc_nr::dl_ccch_msg_type_c::c1_c_::types_opts::rrc_reject);

  // TEST: UE is not destroyed in CU-CP until UE Context Release Complete is received.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_EQ(report.ues.size(), 1);
  ASSERT_EQ(report.ues[0].rnti, to_rnti(0x4601));

  // EVENT: DU sends F1AP UE Context Release Complete.
  auto rel_complete =
      generate_ue_context_release_complete(int_to_gnb_cu_ue_f1ap_id(ue_rel->gnb_cu_ue_f1ap_id), du_ue_f1ap_id);
  get_du(du_idx).push_ul_pdu(rel_complete);

  // TEST: UE context removed from CU-CP.
  report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_TRUE(report.ues.empty());
}

TEST_F(cu_cp_setup_test, when_rrc_setup_completes_then_initial_message_sent_to_amf)
{
  // Check no UEs.
  auto report = this->get_cu_cp().get_metrics_handler().request_metrics_report();
  ASSERT_TRUE(report.ues.empty());

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
  cu_ue_id               = int_to_gnb_cu_ue_f1ap_id(dl_rrc_msg->gnb_cu_ue_f1ap_id);

  // AMF still not notified of UE attach.
  ngap_message ngap_pdu;
  ASSERT_FALSE(this->get_amf().try_pop_rx_pdu(ngap_pdu));

  // UE sends UL RRC Message with RRC Setup Complete.
  f1ap_message ul_rrc_msg =
      generate_ul_rrc_message_transfer(cu_ue_id.value(), du_ue_f1ap_id, srb_id_t::srb1, generate_rrc_setup_complete());
  test_logger.info("Injecting UL RRC message (RRC Setup Complete)");
  get_du(du_idx).push_ul_pdu(ul_rrc_msg);

  // Verify AMF is notified of UE attach.
  ASSERT_TRUE(this->wait_for_ngap_tx_pdu(ngap_pdu));
  ASSERT_TRUE(is_pdu_type(ngap_pdu, asn1::ngap::ngap_elem_procs_o::init_msg_c::types::types_opts::init_ue_msg));
}
