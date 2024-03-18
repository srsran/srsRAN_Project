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

#include "f1ap_du_test_helpers.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace srs_du;

class f1ap_du_ue_management_tester : public f1ap_du_test
{
protected:
  dummy_f1c_rx_sdu_notifier srb2_rx_sdu_notifier;
  dummy_f1u_rx_sdu_notifier drb1_rx_sdu_notifier;

  f1ap_ue_configuration_request create_ue_config_update_request(du_ue_index_t ue_index)
  {
    f1ap_ue_configuration_request req;
    req.ue_index = ue_index;
    // > SRB2
    req.f1c_bearers_to_add.emplace_back();
    req.f1c_bearers_to_add[0].srb_id          = srb_id_t::srb2;
    req.f1c_bearers_to_add[0].rx_sdu_notifier = &srb2_rx_sdu_notifier;
    return req;
  }
};

TEST_F(f1ap_du_ue_management_tester, f1ap_ue_config_update_returns_valid_response)
{
  du_ue_index_t ue_index = to_du_ue_index(0);
  // Run Test Preamble.
  run_f1_setup_procedure();
  run_f1ap_ue_create(ue_index);

  // Process F1AP UE configuration.
  f1ap_ue_configuration_request  req  = create_ue_config_update_request(ue_index);
  f1ap_ue_configuration_response resp = f1ap->handle_ue_configuration_request(req);
  ASSERT_EQ(resp.f1c_bearers_added.size(), 1);
  ASSERT_EQ(resp.f1c_bearers_added[0].srb_id, req.f1c_bearers_to_add[0].srb_id);
  ASSERT_NE(resp.f1c_bearers_added[0].bearer, nullptr);
}

TEST_F(f1ap_du_ue_management_tester, f1ap_created_bearers_forward_messages_to_notifiers)
{
  du_ue_index_t ue_index = to_du_ue_index(0);
  // Run Test Preamble.
  run_f1_setup_procedure();
  run_f1ap_ue_create(ue_index);

  // Process F1AP UE configuration.
  f1ap_ue_configuration_request  req  = create_ue_config_update_request(ue_index);
  f1ap_ue_configuration_response resp = f1ap->handle_ue_configuration_request(req);

  // Send DL data through created F1-C bearer.
  byte_buffer dl_srb_buf =
      byte_buffer::create(test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(3, 100))).value();
  ASSERT_TRUE(this->srb2_rx_sdu_notifier.last_pdu.empty());
  f1ap->handle_message(generate_f1ap_dl_rrc_message_transfer(srb_id_t::srb2, dl_srb_buf));
  ASSERT_EQ(dl_srb_buf, this->srb2_rx_sdu_notifier.last_pdu);

  // Send UL data through created F1-C bearer.
  byte_buffer ul_srb_buf =
      byte_buffer::create(test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(3, 100))).value();
  resp.f1c_bearers_added[0].bearer->handle_sdu(byte_buffer_chain::create(ul_srb_buf.copy()).value());
  const auto& ul_f1ap_msg = this->f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.ul_rrc_msg_transfer();
  ASSERT_EQ(ul_f1ap_msg->rrc_container, ul_srb_buf);
  ASSERT_EQ((srb_id_t)ul_f1ap_msg->srb_id, srb_id_t::srb2);
}

TEST_F(f1ap_du_ue_management_tester, f1ap_created_bearers_do_not_forward_invalid_dl_messages_to_notifiers)
{
  du_ue_index_t ue_index = to_du_ue_index(0);
  // Run Test Preamble.
  run_f1_setup_procedure();
  run_f1ap_ue_create(ue_index);

  // Process F1AP UE configuration.
  f1ap_ue_configuration_request  req  = create_ue_config_update_request(ue_index);
  f1ap_ue_configuration_response resp = f1ap->handle_ue_configuration_request(req);

  // Send DL data through created F1-C bearer.
  byte_buffer dl_srb_buf =
      byte_buffer::create(test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(1, 2))).value();
  ASSERT_TRUE(this->srb2_rx_sdu_notifier.last_pdu.empty());
  f1ap->handle_message(generate_f1ap_dl_rrc_message_transfer(srb_id_t::srb2, dl_srb_buf));
  ASSERT_TRUE(this->srb2_rx_sdu_notifier.last_pdu.empty());
}
