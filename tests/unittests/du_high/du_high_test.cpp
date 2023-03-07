/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "lib/f1ap/common/f1ap_asn1_packer.h"
#include "test_utils/du_high_test_bench.h"
#include "tests/unittests/f1ap/du/f1ap_du_test_helpers.h"
#include "tests/unittests/gateways/test_helpers.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

class du_high_tester : public du_high_test_bench, public testing::Test
{};

/// Test F1 setup over "local" connection to DU.
TEST_F(du_high_tester, when_du_high_started_then_connection_to_cu_takes_place)
{
  // Starting the DU-high initiates the F1 Setup procedure.
  ASSERT_EQ(this->cu_notifier.last_f1ap_msg.pdu.type().value, f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->cu_notifier.last_f1ap_msg.pdu.init_msg().proc_code, ASN1_F1AP_ID_F1_SETUP);
}

TEST_F(du_high_tester, when_ccch_msg_is_received_then_ue_context_is_created)
{
  // Reset the last sent F1AP PDU (e.g. F1 setup).
  cu_notifier.last_f1ap_msg.pdu = {};

  // Add UE
  du_obj.get_pdu_handler(to_du_cell_index(0))
      .handle_rx_data_indication(create_ccch_message(next_slot, to_rnti(0x4601)));

  const unsigned MAX_COUNT = 1000;
  for (uint32_t count = 0; count < MAX_COUNT; count++) {
    this->run_slot();
    if (cu_notifier.last_f1ap_msg.pdu.type() != asn1::f1ap::f1ap_pdu_c::types_opts::nulltype) {
      break;
    }
  }
  ASSERT_EQ(cu_notifier.last_f1ap_msg.pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(cu_notifier.last_f1ap_msg.pdu.init_msg().proc_code, ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER);
  const init_ul_rrc_msg_transfer_s& msg = cu_notifier.last_f1ap_msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
  ASSERT_EQ(msg->c_rnti->value, 0x4601);
}

TEST_F(du_high_tester, when_ue_context_release_received_then_ue_gets_deleted)
{
  // Create UE.
  ASSERT_TRUE(add_ue(to_rnti(0x4601)));

  // Receive UE Context Release Command.
  cu_notifier.last_f1ap_msg.pdu       = {};
  f1ap_message                    msg = generate_ue_context_release_command();
  const ue_context_release_cmd_s& cmd = msg.pdu.init_msg().value.ue_context_release_cmd();
  this->du_obj.get_f1ap_message_handler().handle_message(msg);

  const unsigned MAX_COUNT = 1000;
  for (unsigned i = 0; i != MAX_COUNT; ++i) {
    this->run_slot();

    // Handle PUCCH of the SRB message containing RRC Release.
    for (const pucch_info& pucch : this->phy.cell.last_ul_res->ul_res->pucchs) {
      if (pucch.format == srsran::pucch_format::FORMAT_1 and pucch.format_1.harq_ack_nof_bits > 0) {
        mac_uci_indication_message uci_msg;
        uci_msg.sl_rx = this->phy.cell.last_ul_res->slot;
        uci_msg.ucis  = {mac_uci_pdu{
             .rnti = to_rnti(0x4601),
             .pdu  = mac_uci_pdu::pucch_f0_or_f1_type{.harq_info = mac_uci_pdu::pucch_f0_or_f1_type::harq_information{
                                                          .harqs = {uci_pucch_f0_or_f1_harq_values::ack}}}}};
        this->du_obj.get_control_information_handler(to_du_cell_index(0)).handle_uci(uci_msg);
      }
    }

    // Stop loop, When UE Context Release complete has been sent to CU.
    if (cu_notifier.last_f1ap_msg.pdu.type().value != asn1::f1ap::f1ap_pdu_c::types_opts::nulltype) {
      break;
    }
  }
  ASSERT_EQ(cu_notifier.last_f1ap_msg.pdu.type().value, f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(cu_notifier.last_f1ap_msg.pdu.successful_outcome().proc_code, ASN1_F1AP_ID_UE_CONTEXT_RELEASE);
  const ue_context_release_complete_s& rel_complete =
      cu_notifier.last_f1ap_msg.pdu.successful_outcome().value.ue_context_release_complete();
  ASSERT_EQ(rel_complete->gnb_du_ue_f1ap_id->value, cmd->gnb_du_ue_f1ap_id->value);
  ASSERT_EQ(rel_complete->gnb_cu_ue_f1ap_id->value, cmd->gnb_cu_ue_f1ap_id->value);
}
