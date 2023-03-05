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
#include "lib/gateways/sctp_network_gateway_impl.h"
#include "test_utils/du_high_test_bench.h"
#include "test_utils/dummy_test_components.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

class du_setup_tester : public du_high_test_bench, public testing::Test
{};

class dummy_network_gateway_notifier : public sctp_network_gateway_control_notifier,
                                       public network_gateway_data_notifier
{
public:
  dummy_network_gateway_notifier() : logger(srslog::fetch_basic_logger("TEST")){};

  void on_connection_loss() override { logger.info("Connection loss"); }
  void on_connection_established() override { logger.info("Connection established"); }
  void on_new_pdu(byte_buffer pdu) override { logger.info("Received PDU"); }

private:
  srslog::basic_logger& logger;
};

/// Test F1 setup over "local" connection to DU.
TEST_F(du_setup_tester, when_du_high_started_then_connection_to_cu_takes_place)
{
  // Starting the DU-high initiates the F1 Setup procedure.
  ASSERT_EQ(this->cu_notifier.last_f1ap_msg.pdu.type().value, f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->cu_notifier.last_f1ap_msg.pdu.init_msg().proc_code, ASN1_F1AP_ID_F1_SETUP);
}

class du_ue_create_tester : public du_high_test_bench, public testing::Test
{};

TEST_F(du_ue_create_tester, when_ccch_msg_is_received_then_ue_context_is_created)
{
  // Reset the last sent F1AP PDU.
  pdu_handler.last_f1ap_msg.pdu = {};

  // Add UE
  mac_rx_pdu_list lst;
  lst.push_back(mac_rx_pdu{to_rnti(0x4601), 0, 0, {0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}});
  du_obj.get_pdu_handler(to_du_cell_index(0))
      .handle_rx_data_indication(mac_rx_data_indication{slot_point{0, 0}, to_du_cell_index(0), std::move(lst)});

  slot_point sl_tx{0, 0};
  for (uint32_t count = 0; count < 10000; count++) {
    du_obj.get_slot_handler(to_du_cell_index(0)).handle_slot_indication(sl_tx++);
    if (pdu_handler.last_f1ap_msg.pdu.type() != asn1::f1ap::f1ap_pdu_c::types_opts::nulltype) {
      break;
    }
    workers.ctrl_worker.run_next_blocking();
  }
  ASSERT_EQ(pdu_handler.last_f1ap_msg.pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(pdu_handler.last_f1ap_msg.pdu.init_msg().proc_code, ASN1_F1AP_ID_INIT_UL_RRC_MSG_TRANSFER);
}
