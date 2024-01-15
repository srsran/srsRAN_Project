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

#include "../common/f1ap_cu_test_messages.h"
#include "lib/f1ap/cu_cp/f1ap_asn1_helpers.h"
#include <gtest/gtest.h>

using namespace asn1;
using namespace srsran;
using namespace srs_cu_cp;

#define JSON_OUTPUT 1

class f1ap_cu_msg_filler_test : public ::testing::Test
{
protected:
  void SetUp() override
  {
    srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("ASN1").set_hex_dump_max_size(-1);

    test_logger.set_level(srslog::basic_levels::debug);
    test_logger.set_hex_dump_max_size(-1);

    srslog::init();

    // Start the log backend.
    srslog::init();
  }

  void TearDown() override
  {
    // flush logger after each test
    srslog::flush();
  }

  void verify_f1ap_ue_context_modification(const asn1::f1ap::ue_context_mod_request_s& msg)
  {
    // set required fields to have full PDU
    asn1::f1ap::f1ap_pdu_c pdu;
    pdu.set_init_msg();
    pdu.init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
    pdu.init_msg().value.ue_context_mod_request()                    = msg;
    pdu.init_msg().value.ue_context_mod_request()->gnb_cu_ue_f1ap_id = 1;
    pdu.init_msg().value.ue_context_mod_request()->gnb_du_ue_f1ap_id = 2;

    // verify succesful packing
    srsran::byte_buffer tx_buffer;
    asn1::bit_ref       bref(tx_buffer);
    ASSERT_EQ(pdu.pack(bref), asn1::SRSASN_SUCCESS);

    std::vector<uint8_t> bytes{tx_buffer.begin(), tx_buffer.end()};
#if JSON_OUTPUT
    asn1::json_writer json_writer1;
    pdu.to_json(json_writer1);
    test_logger.info(bytes.data(), bytes.size(), "PDU unpacked ({} B): \n {}", bytes.size(), json_writer1.to_string());
#endif

    ASSERT_EQ(test_pack_unpack_consistency(pdu), asn1::SRSASN_SUCCESS);
  }

  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
};

// Test correct filling and generation of F1AP ASN1 messages for UE context modficication request.
TEST_F(f1ap_cu_msg_filler_test, when_context_mod_req_valid_then_valid_asn1_msg_generated)
{
  f1ap_ue_context_modification_request msg = generate_ue_context_modification_request(uint_to_ue_index(1));

  asn1::f1ap::ue_context_mod_request_s f1ap_ue_context_mod_request = {};
  fill_asn1_ue_context_modification_request(f1ap_ue_context_mod_request, msg);

  // pack full F1AP PDU and verify correctnes
  verify_f1ap_ue_context_modification(f1ap_ue_context_mod_request);
}
