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

TEST_F(f1ap_du_test,
       when_dl_rrc_message_transfer_contains_cu_ue_id_that_does_not_match_then_error_indication_is_sent_to_cu)
{
  // Run Test Preamble.
  run_f1_setup_procedure();
  // > create UE1
  ue_test_context* ue1 = run_f1ap_ue_create(to_du_ue_index(0));
  ASSERT_NE(ue1, nullptr);
  // > run DL rrc message for UE1 that will set the gnb-cu-ue-f1ap-id.
  byte_buffer  test_rrc_msg = byte_buffer::create({0x1, 0x2, 0x3}).value();
  f1ap_message msg          = generate_f1ap_dl_rrc_message_transfer(srb_id_t::srb1, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id = 0;
  ASSERT_TRUE(ue1->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  f1ap->handle_message(msg);
  ASSERT_FALSE(ue1->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  // > send DL RRC Message with invalid gNB CU UE F1AP ID.
  ue1->f1c_bearers[1].rx_sdu_notifier.last_pdu.clear();
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id = 1;
  f1ap->handle_message(msg);

  // Test Section

  // > No RRC container is forwarded.
  ASSERT_TRUE(ue1->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  // > Error Indication is sent to CU.
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::error_ind);
  const auto& err_ind = f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.error_ind();
  ASSERT_TRUE(err_ind->gnb_du_ue_f1ap_id_present);
  ASSERT_EQ(err_ind->gnb_du_ue_f1ap_id, 0);
  ASSERT_TRUE(err_ind->cause_present);
  ASSERT_EQ(err_ind->cause.radio_network().value,
            asn1::f1ap::cause_radio_network_opts::unknown_or_inconsistent_pair_of_ue_f1ap_id);
}

TEST_F(f1ap_du_test,
       when_dl_rrc_message_transfer_with_old_gnb_du_ue_id_then_du_is_notified_and_rrc_container_is_forwarded)
{
  // Run Test Preamble.
  run_f1_setup_procedure();
  // > create UE1
  ue_test_context* ue1 = run_f1ap_ue_create(to_du_ue_index(0));
  ASSERT_NE(ue1, nullptr);
  // > run DL rrc message for UE1 that will set the gnb-cu-ue-f1ap-id.
  byte_buffer  test_rrc_msg = byte_buffer::create({0x1, 0x2, 0x3}).value();
  f1ap_message msg          = generate_f1ap_dl_rrc_message_transfer(srb_id_t::srb0, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id = 0;
  f1ap->handle_message(msg);
  // > create UE2
  ue_test_context* ue2 = run_f1ap_ue_create(to_du_ue_index(1));
  ASSERT_NE(ue2, nullptr);
  // > Send DL RRC Message Transfer for UE2 (containing old gNB DU UE ID).
  msg = generate_f1ap_dl_rrc_message_transfer(srb_id_t::srb1, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_du_ue_f1ap_id             = 1;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id             = 1;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->old_gnb_du_ue_f1ap_id_present = true;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->old_gnb_du_ue_f1ap_id         = 0;
  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.has_value());
  f1ap->handle_message(msg);

  // Test Section.

  // > DU was notified of reestablishment.
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.value().first, to_du_ue_index(1));
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.value().second, to_du_ue_index(0));

  // > RRC container is forwarded.
  ASSERT_FALSE(ue2->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  ASSERT_EQ(ue2->f1c_bearers[1].rx_sdu_notifier.last_pdu, test_rrc_msg);

  // > gNB-CU-UE-F1AP-Id is updated.
  ue2->f1c_bearers[1].bearer->handle_sdu(byte_buffer_chain::create(test_rrc_msg.copy()).value());
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_cu_ue_f1ap_id,
            msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id)
      << "Invalid gNB-CU UE F1AP ID";
}

TEST_F(f1ap_du_test, when_dl_rrc_message_transfer_has_duplicate_cu_ue_id_and_old_gnb_du_ue_id_then_cu_ue_id_is_updated)
{
  // Run Test Preamble.
  run_f1_setup_procedure();
  // > create UE1
  ue_test_context* ue1 = run_f1ap_ue_create(to_du_ue_index(0));
  ASSERT_NE(ue1, nullptr);
  // > run DL rrc message for UE1 that will set the gnb-cu-ue-f1ap-id.
  byte_buffer  test_rrc_msg = byte_buffer::create({0x1, 0x2, 0x3}).value();
  f1ap_message msg          = generate_f1ap_dl_rrc_message_transfer(srb_id_t::srb0, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id = 5;
  f1ap->handle_message(msg);
  // > create UE2
  ue_test_context* ue2 = run_f1ap_ue_create(to_du_ue_index(1));
  ASSERT_NE(ue2, nullptr);
  // > Send DL RRC Message Transfer for UE2 (containing old gNB DU UE ID).
  msg = generate_f1ap_dl_rrc_message_transfer(srb_id_t::srb1, test_rrc_msg);
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_du_ue_f1ap_id             = 1;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id             = 5;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->old_gnb_du_ue_f1ap_id_present = true;
  msg.pdu.init_msg().value.dl_rrc_msg_transfer()->old_gnb_du_ue_f1ap_id         = 0;
  ASSERT_FALSE(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.has_value());
  f1ap->handle_message(msg);

  // Test Section.

  // > DU was notified of reestablishment.
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.has_value());
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.value().first, to_du_ue_index(1));
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_reestablishment_ue_indexes.value().second, to_du_ue_index(0));
  // > RRC container is forwarded.
  ASSERT_FALSE(ue2->f1c_bearers[1].rx_sdu_notifier.last_pdu.empty());
  ASSERT_EQ(ue2->f1c_bearers[1].rx_sdu_notifier.last_pdu, test_rrc_msg);
  // > gNB-CU-UE-F1AP-Id is updated.
  ue2->f1c_bearers[1].bearer->handle_sdu(byte_buffer_chain::create(test_rrc_msg.copy()).value());
  ASSERT_EQ(f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.ul_rrc_msg_transfer()->gnb_cu_ue_f1ap_id,
            msg.pdu.init_msg().value.dl_rrc_msg_transfer()->gnb_cu_ue_f1ap_id)
      << "Invalid gNB-CU UE F1AP ID";
}