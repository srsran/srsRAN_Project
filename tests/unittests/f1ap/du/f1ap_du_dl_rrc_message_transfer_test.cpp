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
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srsran::srs_du;

TEST_F(f1ap_du_test, when_dl_rrc_message_transfer_is_received_lower_layer_srb_notifier_receives_rrc_pdu)
{
  // Run Test Preamble.
  run_f1_setup_procedure();
  ue_test_context* ue = run_f1ap_ue_create(to_du_ue_index(0));
  ASSERT_TRUE(ue->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());

  // Test section
  byte_buffer  test_rrc_msg = byte_buffer::create({0x1, 0x2, 0x3}).value();
  f1ap_message msg          = generate_f1ap_dl_rrc_message_transfer(srb_id_t::srb1, test_rrc_msg);
  f1ap->handle_message(msg);
  ASSERT_FALSE(ue->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  ASSERT_EQ(ue->f1c_bearers[1].rx_sdu_notifier.last_pdu, test_rrc_msg);
}

TEST_F(f1ap_du_test, when_dl_rrc_message_transfer_is_received_gnb_cu_ue_f1ap_id_is_updated)
{
  // Run Test Preamble.
  run_f1_setup_procedure();
  ue_test_context* ue = run_f1ap_ue_create(to_du_ue_index(0));
  ASSERT_TRUE(ue->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());

  // Test Section.
  byte_buffer  test_rrc_msg = byte_buffer::create({0x1, 0x2, 0x3}).value();
  f1ap_message msg          = generate_f1ap_dl_rrc_message_transfer(srb_id_t::srb1, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id = 5;
  // The DL RRC Message Transfer should update the gNB-CU UE F1AP ID.
  f1ap->handle_message(msg);
  test_ues[0].f1c_bearers[1].bearer->handle_sdu(byte_buffer_chain::create(test_rrc_msg.copy()).value());
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_cu_ue_f1ap_id,
            msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id)
      << "Invalid gNB-CU UE F1AP ID";
}
