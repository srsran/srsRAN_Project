/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "cu_cp_test_environment.h"
#include "srsran/asn1/ngap/common.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/f1ap/f1ap_message.h"
#include "srsran/ngap/ngap_message.h"
#include "srsran/ran/cu_types.h"
#include <gtest/gtest.h>
#include <optional>

using namespace srsran;
using namespace srs_cu_cp;

class cu_cp_nrppa_test : public cu_cp_test_environment, public ::testing::Test
{
public:
  cu_cp_nrppa_test() : cu_cp_test_environment(cu_cp_test_env_params{})
  {
    // Run NG setup to completion.
    run_ng_setup();

    // Setup DU.
    std::optional<unsigned> ret = connect_new_du();
    EXPECT_TRUE(ret.has_value());
    du_idx = ret.value();
    EXPECT_TRUE(this->run_f1_setup(du_idx));

    // Setup CU-UP.
    ret = connect_new_cu_up();
    EXPECT_TRUE(ret.has_value());
    cu_up_idx = ret.value();
    EXPECT_TRUE(this->run_e1_setup(cu_up_idx));

    // Connect UE 0x4601.
    EXPECT_TRUE(attach_ue(du_idx, cu_up_idx, du_ue_id, crnti, amf_ue_id, cu_up_e1ap_id, psi, drb_id_t::drb1, qfi));
    ue_ctx = this->find_ue_context(du_idx, du_ue_id);

    EXPECT_NE(ue_ctx, nullptr);
  }

  unsigned du_idx    = 0;
  unsigned cu_up_idx = 0;

  gnb_du_ue_f1ap_id_t    du_ue_id      = gnb_du_ue_f1ap_id_t::min;
  rnti_t                 crnti         = to_rnti(0x4601);
  amf_ue_id_t            amf_ue_id     = amf_ue_id_t::min;
  gnb_cu_up_ue_e1ap_id_t cu_up_e1ap_id = gnb_cu_up_ue_e1ap_id_t::min;

  const ue_context* ue_ctx = nullptr;

  pdu_session_id_t psi = uint_to_pdu_session_id(1);
  qos_flow_id_t    qfi = uint_to_qos_flow_id(1);

  ngap_message ngap_pdu;
  f1ap_message f1ap_pdu;
  e1ap_message e1ap_pdu;

  [[nodiscard]] bool send_dl_non_ue_associated_nrppa_transport()
  {
    report_fatal_error_if_not(not this->get_amf().try_pop_rx_pdu(ngap_pdu),
                              "there are still NGAP messages to pop from AMF");
    report_fatal_error_if_not(not this->get_du(du_idx).try_pop_dl_pdu(f1ap_pdu),
                              "there are still F1AP DL messages to pop from DU");
    report_fatal_error_if_not(not this->get_cu_up(cu_up_idx).try_pop_rx_pdu(e1ap_pdu),
                              "there are still E1AP messages to pop from CU-UP");

    // Generate DL non UE associated NRPPA transport message.
    ngap_message ngap_msg;
    ngap_msg.pdu.set_init_msg();
    ngap_msg.pdu.init_msg().load_info_obj(ASN1_NGAP_ID_DL_NON_UE_ASSOCIATED_NRPPA_TRANSPORT);
    auto& ue_nrppa       = ngap_msg.pdu.init_msg().value.dl_non_ue_associated_nrppa_transport();
    ue_nrppa->routing_id = make_byte_buffer("30").value();
    ue_nrppa->nrppa_pdu =
        make_byte_buffer(
            "00060000006c000001000900654c000a000100000a000101000a000102000a000103000a000104000a000105000a000"
            "106000a000107000a000108000a000109000a00010a000a00010b000a00010c000a00010d000a00010e000a00010f00"
            "0a000110000a000111000a000112000a000113")
            .value();

    // Inject DL non UE associated NRPPA transport message.
    get_amf().push_tx_pdu(ngap_msg);

    return true;
  }
};

TEST_F(cu_cp_nrppa_test, when_dl_non_ue_nrppa_message_received_then_logging_it)
{
  // Inject DL non UE associated NRPPA transport message.
  ASSERT_TRUE(send_dl_non_ue_associated_nrppa_transport());

  // Make sure that no UL non UE associated NRPPA transport message is sent to the AMF.
  ASSERT_FALSE(this->wait_for_ngap_tx_pdu(ngap_pdu));
}
