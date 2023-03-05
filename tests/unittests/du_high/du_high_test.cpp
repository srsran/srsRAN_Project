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
  // Reset the last sent F1AP PDU.
  pdu_handler.last_f1ap_msg.pdu = {};

  // Add UE
  du_obj.get_pdu_handler(to_du_cell_index(0))
      .handle_rx_data_indication(create_ccch_message(next_slot, to_rnti(0x4601)));

  const unsigned MAX_COUNT = 10000;
  for (uint32_t count = 0; count < MAX_COUNT; count++) {
    this->run_slot();
    if (pdu_handler.last_f1ap_msg.pdu.type() != asn1::f1ap::f1ap_pdu_c::types_opts::nulltype) {
      break;
    }
  }
  ASSERT_EQ(pdu_handler.last_f1ap_msg.pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(pdu_handler.last_f1ap_msg.pdu.init_msg().proc_code, ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER);
}
