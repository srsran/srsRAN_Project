/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "../../e1ap/common/test_helpers.h"
#include "du_processor_routine_manager_test_helpers.h"
#include "lib/e1ap/cu_cp/e1ap_cu_cp_asn1_helpers.h"
#include "lib/f1ap/cu_cp/f1ap_asn1_helpers.h"
#include "srsran/f1ap/common/f1ap_message.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

// Helper function to verify that two lists are identical.
template <typename T>
void VERIFY_EQUAL(const T& result, const T& expected)
{
  ASSERT_EQ(result.size(), expected.size());
  ASSERT_TRUE(std::equal(result.begin(), result.end(), expected.begin()));
}

class pdu_session_resource_routine_test : public du_processor_routine_manager_test
{
protected:
  void is_valid_f1ap_message(const f1ap_ue_context_modification_request& request)
  {
    f1ap_message f1ap_msg;
    f1ap_msg.pdu.set_init_msg().load_info_obj(ASN1_F1AP_ID_UE_CONTEXT_MOD);
    auto& ctx_mod = f1ap_msg.pdu.init_msg().value.ue_context_mod_request();

    fill_asn1_ue_context_modification_request(ctx_mod, request);

    ctx_mod->gnb_du_ue_f1ap_id = 0;
    ctx_mod->gnb_cu_ue_f1ap_id = 1;

    byte_buffer   packed_pdu;
    asn1::bit_ref bref(packed_pdu);
    ASSERT_EQ(f1ap_msg.pdu.pack(bref), asn1::SRSASN_SUCCESS);
  }

  void is_valid_e1ap_message(const e1ap_bearer_context_modification_request& request)
  {
    e1ap_message e1ap_msg;
    e1ap_msg.pdu.set_init_msg();
    e1ap_msg.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_MOD);

    auto& bearer_context_mod_request = e1ap_msg.pdu.init_msg().value.bearer_context_mod_request();
    bearer_context_mod_request->gnb_cu_cp_ue_e1ap_id =
        gnb_cu_cp_ue_e1ap_id_to_uint(generate_random_gnb_cu_cp_ue_e1ap_id());
    bearer_context_mod_request->gnb_cu_up_ue_e1ap_id =
        gnb_cu_up_ue_e1ap_id_to_uint(generate_random_gnb_cu_up_ue_e1ap_id());

    fill_asn1_bearer_context_modification_request(bearer_context_mod_request, request);

    byte_buffer   packed_pdu;
    asn1::bit_ref bref(packed_pdu);
    ASSERT_EQ(e1ap_msg.pdu.pack(bref), asn1::SRSASN_SUCCESS);
  }

  void is_valid_e1ap_message(const e1ap_bearer_context_setup_request& request)
  {
    e1ap_message e1ap_msg;
    e1ap_msg.pdu.set_init_msg();
    e1ap_msg.pdu.init_msg().load_info_obj(ASN1_E1AP_ID_BEARER_CONTEXT_SETUP);

    auto& bearer_context_setup_request = e1ap_msg.pdu.init_msg().value.bearer_context_setup_request();
    bearer_context_setup_request->gnb_cu_cp_ue_e1ap_id =
        gnb_cu_cp_ue_e1ap_id_to_uint(generate_random_gnb_cu_cp_ue_e1ap_id());
    bearer_context_setup_request->gnb_cu_up_ue_e1ap_id =
        gnb_cu_up_ue_e1ap_id_to_uint(generate_random_gnb_cu_up_ue_e1ap_id());

    fill_asn1_bearer_context_setup_request(bearer_context_setup_request, request);

    byte_buffer   packed_pdu;
    asn1::bit_ref bref(packed_pdu);
    ASSERT_EQ(e1ap_msg.pdu.pack(bref), asn1::SRSASN_SUCCESS);
  }
};
