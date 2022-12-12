/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1_du_test_helpers.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace srs_du;

class f1ap_du_ue_management_tester : public f1ap_du_test
{
protected:
  dummy_f1c_tx_pdu_notifier srb2_tx_pdu_notifier;
  dummy_f1u_rx_sdu_notifier drb1_tx_pdu_notifier;

  f1ap_ue_configuration_request create_ue_config_update_request(du_ue_index_t ue_index)
  {
    f1ap_ue_configuration_request req;
    req.ue_index = ue_index;
    // > SRB2
    req.f1c_bearers_to_add.emplace_back();
    req.f1c_bearers_to_add[0].srb_id       = srb_id_t::srb2;
    req.f1c_bearers_to_add[0].tx_pdu_notif = &srb2_tx_pdu_notifier;
    // > DRB1
    req.f1u_bearers_to_add.emplace_back();
    req.f1u_bearers_to_add[0].drb_id          = drb_id_t::drb1;
    req.f1u_bearers_to_add[0].rx_sdu_notifier = &drb1_tx_pdu_notifier;
    return req;
  }
};

TEST_F(f1ap_du_ue_management_tester, f1ap_ue_config_update_returns_valid_response)
{
  du_ue_index_t ue_index = to_du_ue_index(0);
  // Run Test Preamble.
  run_f1_setup_procedure();
  run_f1_ue_create(ue_index);

  // Process F1AP UE configuration.
  f1ap_ue_configuration_request  req  = create_ue_config_update_request(ue_index);
  f1ap_ue_configuration_response resp = f1ap->handle_ue_configuration_request(req);
  ASSERT_EQ(resp.f1c_bearers_added.size(), 1);
  ASSERT_EQ(resp.f1c_bearers_added[0].srb_id, req.f1c_bearers_to_add[0].srb_id);
  ASSERT_NE(resp.f1c_bearers_added[0].bearer, nullptr);
  ASSERT_EQ(resp.f1u_bearers_added.size(), 1);
  ASSERT_EQ(resp.f1u_bearers_added[0].drb_id, req.f1u_bearers_to_add[0].drb_id);
  ASSERT_NE(resp.f1u_bearers_added[0].bearer, nullptr);
}

TEST_F(f1ap_du_ue_management_tester, f1ap_created_bearers_forward_messages_to_notifiers)
{
  du_ue_index_t ue_index = to_du_ue_index(0);
  // Run Test Preamble.
  run_f1_setup_procedure();
  run_f1_ue_create(ue_index);

  // Process F1AP UE configuration.
  f1ap_ue_configuration_request  req  = create_ue_config_update_request(ue_index);
  f1ap_ue_configuration_response resp = f1ap->handle_ue_configuration_request(req);

  // Send DL data through created F1-C bearer.
  byte_buffer dl_srb_buf{test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(1, 100))};
  ASSERT_TRUE(this->srb2_tx_pdu_notifier.last_pdu.empty());
  f1ap->handle_message(generate_f1_dl_rrc_message_transfer(srb_id_t::srb2, dl_srb_buf));
  ASSERT_EQ(dl_srb_buf, this->srb2_tx_pdu_notifier.last_pdu);

  // Send UL data through created F1-C bearer.
  byte_buffer ul_srb_buf{test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(1, 100))};
  resp.f1c_bearers_added[0].bearer->handle_pdu(byte_buffer_slice_chain{ul_srb_buf.copy()});
  const auto& ul_f1c_msg = this->msg_notifier.last_f1c_msg.pdu.init_msg().value.ulrrc_msg_transfer();
  ASSERT_EQ(ul_f1c_msg->rrc_container.value, ul_srb_buf);
  ASSERT_EQ((srb_id_t)ul_f1c_msg->srbid->value, srb_id_t::srb2);

  // Send DL data through created F1-U bearer.
  byte_buffer dl_drb_buf{test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(1, 100))};
  ASSERT_TRUE(this->drb1_tx_pdu_notifier.last_pdu.empty());
  nru_dl_message pdu = {};
  pdu.t_pdu          = dl_drb_buf.copy();
  pdu.pdcp_count     = 7;
  resp.f1u_bearers_added[0].bearer->get_rx_pdu_handler().handle_pdu(std::move(pdu));
  ASSERT_FALSE(this->drb1_tx_pdu_notifier.last_pdu.empty());
  // TODO: Enable this check once F1-U bearer is implemented.
  //  ASSERT_EQ(dl_drb_buf, this->drb1_tx_pdu_notifier.last_pdu);

  // Send UL data through created F1-U bearer.
  // TODO
}
