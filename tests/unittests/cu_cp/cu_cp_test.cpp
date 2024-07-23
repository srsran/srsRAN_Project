/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../rrc/rrc_ue_test_messages.h"
#include "cu_cp_test_helpers.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents_ue.h"
#include "srsran/asn1/ngap/ngap_pdu_contents.h"
#include "srsran/ngap/ngap_types.h"
#include "srsran/ran/cu_types.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;
using namespace asn1::f1ap;

//////////////////////////////////////////////////////////////////////////////////////
/* Handover Request handling                                                        */
//////////////////////////////////////////////////////////////////////////////////////

TEST_F(cu_cp_test, when_handover_request_received_then_handover_notify_is_sent)
{
  // Test preamble
  du_index_t du_index = uint_to_du_index(0);
  pci_t      pci      = 0;
  test_preamble_all_connected(du_index, pci);

  amf_ue_id_t amf_ue_id = uint_to_amf_ue_id(
      test_rgen::uniform_int<uint64_t>(amf_ue_id_to_uint(amf_ue_id_t::min), amf_ue_id_to_uint(amf_ue_id_t::max)));

  // Inject Handover Request
  cu_cp_obj->get_ngap_message_handler().handle_message(generate_valid_handover_request(amf_ue_id));

  // Check that the Bearer Context Setup Request Message was sent to the CU-UP
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_setup_request);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.bearer_context_setup_request()->gnb_cu_cp_ue_e1ap_id,
            0);

  // Inject E1AP Bearer Context Setup Response
  e1ap_message bearer_ctxt_setup_resp =
      generate_bearer_context_setup_response(int_to_gnb_cu_cp_ue_e1ap_id(0), int_to_gnb_cu_up_ue_e1ap_id(0));
  e1ap_gw.get_cu_up(0).on_new_message(bearer_ctxt_setup_resp);

  // Check that the UE Context Setup Request Message was sent to the DU
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.type(), asn1::f1ap::f1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::f1ap::f1ap_elem_procs_o::init_msg_c::types_opts::ue_context_setup_request);
  ASSERT_EQ(f1c_gw.last_tx_pdus(0).back().pdu.init_msg().value.ue_context_setup_request()->gnb_cu_ue_f1ap_id, 0);

  // Inject F1AP UE Context Setup Response
  f1ap_message ue_ctxt_setup_resp =
      generate_ue_context_setup_response(int_to_gnb_cu_ue_f1ap_id(0), int_to_gnb_du_ue_f1ap_id(0), to_rnti(0x4601));
  f1c_gw.get_du(du_index).on_new_message(ue_ctxt_setup_resp);

  // Check that the Bearer Context Modification Request Message was sent to the CU-UP
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.type(), asn1::e1ap::e1ap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.type().value,
            asn1::e1ap::e1ap_elem_procs_o::init_msg_c::types_opts::bearer_context_mod_request);
  ASSERT_EQ(e1ap_gw.last_tx_pdus(0).back().pdu.init_msg().value.bearer_context_mod_request()->gnb_cu_cp_ue_e1ap_id, 0);

  // Inject E1AP Bearer Context Modification Response
  e1ap_message bearer_ctxt_mod_resp =
      generate_bearer_context_modification_response(int_to_gnb_cu_cp_ue_e1ap_id(0), int_to_gnb_cu_up_ue_e1ap_id(0));
  e1ap_gw.get_cu_up(0).on_new_message(bearer_ctxt_mod_resp);

  // Check that the Handover Request Ack was sent to the AMF
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::successful_outcome);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.successful_outcome().value.type().value,
            asn1::ngap::ngap_elem_procs_o::successful_outcome_c::types_opts::ho_request_ack);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.successful_outcome().value.ho_request_ack()->amf_ue_ngap_id,
            amf_ue_id_to_uint(amf_ue_id));

  // Inject RRC Reconfiguration Complete with transaction_id=0
  f1ap_message rrc_recfg_complete = generate_ul_rrc_message_transfer(int_to_gnb_cu_ue_f1ap_id(0),
                                                                     int_to_gnb_du_ue_f1ap_id(0),
                                                                     srb_id_t::srb1,
                                                                     make_byte_buffer("800008004e17dae3").value());
  f1c_gw.get_du(du_index).on_new_message(rrc_recfg_complete);

  // Check that the Handover Notify was sent to the AMF
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.type(), asn1::ngap::ngap_pdu_c::types_opts::options::init_msg);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ho_notify);
  ASSERT_EQ(n2_gw.last_ngap_msgs.back().pdu.init_msg().value.ho_notify()->amf_ue_ngap_id, amf_ue_id_to_uint(amf_ue_id));
}
