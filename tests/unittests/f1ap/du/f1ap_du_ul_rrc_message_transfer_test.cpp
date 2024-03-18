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
#include "lib/f1ap/du/ue_context/f1c_du_bearer_impl.h"
#include "srsran/asn1/f1ap/common.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

TEST_F(f1ap_du_test, when_sdu_is_received_then_sdu_is_forwarded_to_tx_pdu_notifier_as_ul_rrc_message_transfer)
{
  // Run Test Preamble.
  run_f1_setup_procedure();
  ue_test_context* ue = run_f1ap_ue_create(to_du_ue_index(0));
  run_ue_context_setup_procedure(ue->ue_index, generate_ue_context_setup_request({}));
  this->f1c_gw.last_tx_f1ap_pdu.pdu = {};

  std::vector<uint8_t> bytes = test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(1, 4000));
  byte_buffer          sdu   = byte_buffer::create(bytes).value();
  ue->f1c_bearers[1].bearer->handle_sdu(byte_buffer_chain::create(sdu.copy()).value());

  ASSERT_EQ(this->f1c_gw.last_tx_f1ap_pdu.pdu.type().value, asn1::f1ap::f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().proc_code, ASN1_F1AP_ID_UL_RRC_MSG_TRANSFER);
  const ul_rrc_msg_transfer_s& ul_rrc_msg = this->f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.ul_rrc_msg_transfer();
  ASSERT_EQ(ul_rrc_msg->srb_id, 1);
  ASSERT_EQ(ul_rrc_msg->rrc_container, sdu);
}
