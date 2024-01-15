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
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_du;
using namespace asn1::f1ap;

/// Unit test for the F1AP UE Context Setup procedure in the DU. Each starts with a F1 connection already established.
class f1ap_du_ue_context_setup_test : public f1ap_du_test
{
protected:
  f1ap_du_ue_context_setup_test() { run_f1_setup_procedure(); }

  /// \brief Called when it is the DU taking the initiative to create a UE in the F1AP (e.g. PRACH).
  void du_creates_f1_logical_connection()
  {
    du_ue_index_t ue_index = to_du_ue_index(test_rgen::uniform_int<unsigned>(0, MAX_DU_UE_INDEX));
    test_ue                = run_f1ap_ue_create(ue_index);
  }

  void start_procedure(const f1ap_message& msg)
  {
    const auto& ue_ctx_setup = *msg.pdu.init_msg().value.ue_context_setup_request();

    if (not ue_ctx_setup.gnb_du_ue_f1ap_id_present) {
      srsran_assert(test_ue == nullptr, "UE should be created as part of the procedure");
      du_ue_index_t ue_index = (du_ue_index_t)test_ues.find_first_empty();

      test_ues.emplace(ue_index);
      test_ue           = &test_ues[ue_index];
      test_ue->ue_index = ue_index;
      test_ue->crnti    = to_rnti(0x4601);
      test_ue->f1c_bearers.emplace(srb_id_to_uint(srb_id_t::srb1));
      test_ue->f1c_bearers[srb_id_to_uint(srb_id_t::srb1)].srb_id = srb_id_t::srb1;

      f1ap_du_cfg_handler.next_ue_creation_req.ue_index    = ue_index;
      f1ap_du_cfg_handler.next_ue_creation_req.pcell_index = to_du_cell_index(0);
      f1ap_du_cfg_handler.next_ue_creation_req.c_rnti      = test_ue->crnti;
      for (auto& f1c_bearer : test_ues[ue_index].f1c_bearers) {
        f1c_bearer_to_addmod b;
        b.srb_id          = f1c_bearer.srb_id;
        b.rx_sdu_notifier = &f1c_bearer.rx_sdu_notifier;
        f1ap_du_cfg_handler.next_ue_creation_req.f1c_bearers_to_add.push_back(b);
      }

      f1ap_du_cfg_handler.next_ue_context_creation_response.result = true;
      f1ap_du_cfg_handler.next_ue_context_creation_response.crnti  = to_rnti(0x4601);
    }

    this->f1ap_du_cfg_handler.next_ue_cfg_req.ue_index = test_ue->ue_index;
    this->f1ap_du_cfg_handler.next_ue_cfg_req.f1c_bearers_to_add.resize(1);
    this->f1ap_du_cfg_handler.next_ue_cfg_req.f1c_bearers_to_add[0].srb_id = srb_id_t::srb2;

    auto& du_to_f1_resp                  = this->f1ap_du_cfg_handler.next_ue_context_update_response;
    du_to_f1_resp.result                 = true;
    du_to_f1_resp.du_to_cu_rrc_container = {0x1, 0x2, 0x3};
    if (ue_ctx_setup.drbs_to_be_setup_list_present) {
      du_to_f1_resp.drbs_setup.resize(ue_ctx_setup.drbs_to_be_setup_list.size());
      for (size_t i = 0; i < ue_ctx_setup.drbs_to_be_setup_list.size(); ++i) {
        uint8_t drb_id                     = ue_ctx_setup.drbs_to_be_setup_list[i]->drbs_to_be_setup_item().drb_id;
        du_to_f1_resp.drbs_setup[i].drb_id = uint_to_drb_id(drb_id);
        du_to_f1_resp.drbs_setup[i].lcid   = uint_to_lcid((uint8_t)LCID_MIN_DRB + drb_id);
        du_to_f1_resp.drbs_setup[i].dluptnl_info_list.resize(1);
        du_to_f1_resp.drbs_setup[i].dluptnl_info_list[0] = up_transport_layer_info{{"127.0.0.1"}, int_to_gtpu_teid(1)};
      }
    }

    f1ap->handle_message(msg);
  }

  ue_test_context* test_ue = nullptr;
};

TEST_F(f1ap_du_ue_context_setup_test, when_f1ap_receives_request_then_f1ap_notifies_du_of_ue_context_update)
{
  du_creates_f1_logical_connection();
  start_procedure(generate_ue_context_setup_request({drb_id_t::drb1}));

  // DU manager receives UE Context Update Request.
  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ue_context_update_req.has_value());
  const f1ap_ue_context_update_request& req = *this->f1ap_du_cfg_handler.last_ue_context_update_req;
  ASSERT_EQ(req.ue_index, test_ue->ue_index);
  ASSERT_EQ(req.srbs_to_setup.size(), 1);
  ASSERT_EQ(req.srbs_to_setup[0], srb_id_t::srb2);
  ASSERT_EQ(req.drbs_to_setup.size(), 1);
  ASSERT_EQ(req.drbs_to_setup[0].drb_id, drb_id_t::drb1);
  ASSERT_FALSE(req.drbs_to_setup[0].lcid.has_value());
}

TEST_F(f1ap_du_ue_context_setup_test, when_f1ap_receives_request_then_f1ap_responds_back_with_ue_context_setup_response)
{
  du_creates_f1_logical_connection();
  auto msg = generate_ue_context_setup_request({drb_id_t::drb1});
  start_procedure(msg);

  // F1AP sends UE CONTEXT SETUP RESPONSE to CU-CP.
  ASSERT_EQ(this->f1c_gw.last_tx_f1ap_pdu.pdu.type().value, f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(this->f1c_gw.last_tx_f1ap_pdu.pdu.successful_outcome().value.type().value,
            f1ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_setup_resp);
  ue_context_setup_resp_s& resp = this->f1c_gw.last_tx_f1ap_pdu.pdu.successful_outcome().value.ue_context_setup_resp();
  ASSERT_EQ(resp->gnb_cu_ue_f1ap_id, msg.pdu.init_msg().value.ue_context_setup_request()->gnb_cu_ue_f1ap_id);
  ASSERT_FALSE(resp->c_rnti_present);
  ASSERT_FALSE(resp->drbs_failed_to_be_setup_list_present);
  ASSERT_TRUE(resp->srbs_setup_list_present);
  ASSERT_EQ(resp->srbs_setup_list.size(), 1);
  ASSERT_EQ(resp->srbs_setup_list[0]->srbs_setup_item().srb_id, 2);
  ASSERT_TRUE(resp->drbs_setup_list_present);
  ASSERT_EQ(resp->drbs_setup_list.size(), 1);
  auto& drb_setup = resp->drbs_setup_list[0].value().drbs_setup_item();
  ASSERT_EQ(drb_setup.drb_id, 1);
  ASSERT_TRUE(drb_setup.lcid_present);
  ASSERT_EQ(drb_setup.dl_up_tnl_info_to_be_setup_list.size(),
            this->f1ap_du_cfg_handler.next_ue_context_update_response.drbs_setup[0].dluptnl_info_list.size());
  ASSERT_EQ(drb_setup.dl_up_tnl_info_to_be_setup_list[0].dl_up_tnl_info.gtp_tunnel().gtp_teid.to_number(),
            this->f1ap_du_cfg_handler.next_ue_context_update_response.drbs_setup[0].dluptnl_info_list[0].gtp_teid);
  ASSERT_EQ(resp->du_to_cu_rrc_info.cell_group_cfg,
            this->f1ap_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container);
}

TEST_F(f1ap_du_ue_context_setup_test, when_f1ap_receives_request_then_the_rrc_container_is_sent_dl_via_srb1)
{
  du_creates_f1_logical_connection();
  f1ap_message msg = generate_ue_context_setup_request({drb_id_t::drb1});
  start_procedure(msg);

  // F1AP sends RRC Container present in UE CONTEXT SETUP REQUEST via SRB1.
  ASSERT_EQ(test_ue->f1c_bearers[1].rx_sdu_notifier.last_pdu,
            msg.pdu.init_msg().value.ue_context_setup_request()->rrc_container);
}

TEST_F(f1ap_du_ue_context_setup_test, when_f1ap_receives_request_then_new_srbs_become_active)
{
  du_creates_f1_logical_connection();
  run_ue_context_setup_procedure(test_ue->ue_index, generate_ue_context_setup_request({drb_id_t::drb1}));

  // UL data through created SRB2 reaches F1-C.
  ASSERT_EQ(this->f1ap_du_cfg_handler.last_ue_cfg_response->f1c_bearers_added.size(), 1);
  f1c_bearer* srb2 = this->f1ap_du_cfg_handler.last_ue_cfg_response->f1c_bearers_added[0].bearer;
  byte_buffer ul_rrc_msg{test_rgen::random_vector<uint8_t>(test_rgen::uniform_int<unsigned>(1, 100))};
  srb2->handle_sdu(byte_buffer_chain{ul_rrc_msg.copy()});
  ASSERT_EQ(this->f1c_gw.last_tx_f1ap_pdu.pdu.type().value, f1ap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(this->f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.type().value,
            f1ap_elem_procs_o::init_msg_c::types_opts::ul_rrc_msg_transfer);
  const ul_rrc_msg_transfer_s& ulmsg = this->f1c_gw.last_tx_f1ap_pdu.pdu.init_msg().value.ul_rrc_msg_transfer();
  ASSERT_EQ(ulmsg->rrc_container, ul_rrc_msg);
}

TEST_F(f1ap_du_ue_context_setup_test, when_f1ap_receives_request_without_gnb_du_ue_f1ap_id_then_ue_is_created)
{
  f1ap_message msg = generate_ue_context_setup_request({drb_id_t::drb1});
  msg.pdu.init_msg().value.ue_context_setup_request()->gnb_du_ue_f1ap_id_present = false;

  start_procedure(msg);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ue_context_creation_req.has_value());
  ASSERT_EQ(test_ue->ue_index, this->f1ap_du_cfg_handler.last_ue_context_creation_req->ue_index);
}

TEST_F(f1ap_du_ue_context_setup_test, when_f1ap_receives_request_without_gnb_du_ue_f1ap_id_then_ue_context_is_updated)
{
  f1ap_message msg = generate_ue_context_setup_request({drb_id_t::drb1});
  msg.pdu.init_msg().value.ue_context_setup_request()->gnb_du_ue_f1ap_id_present = false;

  start_procedure(msg);

  ASSERT_TRUE(this->f1ap_du_cfg_handler.last_ue_context_update_req.has_value());
  auto& request_to_du = *this->f1ap_du_cfg_handler.last_ue_context_update_req;
  ASSERT_EQ(test_ue->ue_index, request_to_du.ue_index);
  ASSERT_EQ(request_to_du.drbs_to_setup.size(), 1);
  ASSERT_EQ(request_to_du.drbs_to_setup[0].drb_id, drb_id_t::drb1);
}

TEST_F(
    f1ap_du_ue_context_setup_test,
    when_f1ap_receives_request_without_gnb_du_ue_f1ap_id_then_ue_context_setup_response_is_sent_to_cu_cp_with_crnti_ie)
{
  f1ap_message msg = generate_ue_context_setup_request({drb_id_t::drb1});
  msg.pdu.init_msg().value.ue_context_setup_request()->gnb_du_ue_f1ap_id_present = false;

  start_procedure(msg);

  // F1AP sends UE CONTEXT SETUP RESPONSE to CU-CP.
  ASSERT_EQ(this->f1c_gw.last_tx_f1ap_pdu.pdu.type().value, f1ap_pdu_c::types_opts::successful_outcome);
  ASSERT_EQ(this->f1c_gw.last_tx_f1ap_pdu.pdu.successful_outcome().value.type().value,
            f1ap_elem_procs_o::successful_outcome_c::types_opts::ue_context_setup_resp);
  ue_context_setup_resp_s& resp = this->f1c_gw.last_tx_f1ap_pdu.pdu.successful_outcome().value.ue_context_setup_resp();
  ASSERT_EQ(resp->gnb_cu_ue_f1ap_id, msg.pdu.init_msg().value.ue_context_setup_request()->gnb_cu_ue_f1ap_id);
  ASSERT_TRUE(resp->c_rnti_present)
      << "UE CONTEXT SETUP RESPONSE should contain C-RNTI IE if it created a UE in the process";
  ASSERT_EQ(resp->c_rnti, to_value(this->f1ap_du_cfg_handler.next_ue_context_creation_response.crnti));
  ASSERT_TRUE(resp->drbs_setup_list_present);
  ASSERT_EQ(resp->drbs_setup_list.size(), 1);
  auto& drb_setup = resp->drbs_setup_list[0].value().drbs_setup_item();
  ASSERT_EQ(drb_setup.drb_id, 1);
  ASSERT_TRUE(drb_setup.lcid_present);
  ASSERT_EQ(drb_setup.dl_up_tnl_info_to_be_setup_list.size(),
            this->f1ap_du_cfg_handler.next_ue_context_update_response.drbs_setup[0].dluptnl_info_list.size());
  ASSERT_EQ(drb_setup.dl_up_tnl_info_to_be_setup_list[0].dl_up_tnl_info.gtp_tunnel().gtp_teid.to_number(),
            this->f1ap_du_cfg_handler.next_ue_context_update_response.drbs_setup[0].dluptnl_info_list[0].gtp_teid);
}

TEST_F(f1ap_du_test, f1ap_handles_precanned_ue_context_setup_request_correctly)
{
  f1ap_message ue_ctxt_setup_req;
  {
    const uint8_t msg[] = {0x00, 0x05, 0x00, 0x44, 0x00, 0x00, 0x08, 0x00, 0x28, 0x00, 0x02, 0x00, 0x16, 0x00, 0x29,
                           0x40, 0x03, 0x40, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x09, 0x00, 0x02, 0xf8, 0x99, 0x00, 0x0b,
                           0xc6, 0x14, 0xe0, 0x00, 0x6b, 0x00, 0x01, 0x00, 0x00, 0x09, 0x00, 0x01, 0x00, 0x00, 0x4a,
                           0x00, 0x06, 0x00, 0x00, 0x49, 0x00, 0x01, 0x08, 0x00, 0x32, 0x40, 0x0a, 0x09, 0x00, 0x03,
                           0x20, 0x08, 0x08, 0x95, 0x75, 0x5b, 0x0c, 0x00, 0xb8, 0x40, 0x01, 0x00};
    // 000500440000080028000200160029400340a127003f00090002f899000bc614e0006b0001000009000100004a00060000490001080032400a09000320080895755b0c00b8400100

    byte_buffer    buf(msg);
    asn1::cbit_ref bref(buf);
    ASSERT_EQ(ue_ctxt_setup_req.pdu.unpack(bref), asn1::SRSASN_SUCCESS);
  }

  // Test Preamble.
  du_ue_index_t ue_index = to_du_ue_index(0);
  run_f1_setup_procedure();
  run_f1ap_ue_create(ue_index);
  run_ue_context_setup_procedure(ue_index, ue_ctxt_setup_req);

  // SRB2 created.
  ue_context_setup_resp_s& resp = this->f1c_gw.last_tx_f1ap_pdu.pdu.successful_outcome().value.ue_context_setup_resp();
  ASSERT_TRUE(resp->srbs_setup_list_present);
  ASSERT_EQ(resp->srbs_setup_list.size(), 1);
  ASSERT_EQ(resp->srbs_setup_list[0]->srbs_setup_item().srb_id, 2);

  // DUtoCURRCInformation included in response.
  ASSERT_EQ(resp->du_to_cu_rrc_info.cell_group_cfg,
            this->f1ap_du_cfg_handler.next_ue_context_update_response.du_to_cu_rrc_container);

  // F1AP sends RRC Container present in UE CONTEXT SETUP REQUEST via SRB1.
  ASSERT_EQ(test_ues[ue_index].f1c_bearers[1].rx_sdu_notifier.last_pdu,
            ue_ctxt_setup_req.pdu.init_msg().value.ue_context_setup_request()->rrc_container);
}