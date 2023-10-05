/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_test_helpers.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Test successful handover preparation procedure
TEST_F(ngap_test, when_ue_missing_then_handover_preparation_procedure_fails)
{
  ngap_handover_preparation_request request = {};
  request.ue_index                          = uint_to_ue_index(0);

  // Action 1: Launch HO preparation procedure
  test_logger.info("Launch source NGAP handover preparation procedure");
  async_task<ngap_handover_preparation_response>         t = ngap->handle_handover_preparation_request(request);
  lazy_task_launcher<ngap_handover_preparation_response> t_launcher(t);

  // Status: should have failed already, as there is no UE.
  ASSERT_TRUE(t.ready());

  // Procedure should have failed.
  ASSERT_FALSE(t.get().success);

  // Make sure no NGAP pdu was sent
  ASSERT_TRUE(msg_notifier.last_ngap_msg.pdu.type() == asn1::ngap::ngap_pdu_c::types_opts::nulltype);
}

/// Test successful handover preparation procedure
TEST_F(ngap_test, when_source_gnb_handover_preparation_triggered_then_ho_command_received)
{
  // Setup UE context
  ue_index_t ue_index = create_ue();
  run_dl_nas_transport(ue_index); // needed to allocate AMF UE id.

  ngap_ue_source_handover_context ho_context;
  ho_context.pdu_sessions = {{uint_to_pdu_session_id(1), {qos_flow_id_t{}}}}; // manually set existing PDU sessions

  auto& ue = test_ues.at(ue_index);
  ue.rrc_ue_notifier.set_handover_context(ho_context);

  ngap_handover_preparation_request request = {};
  request.ue_index                          = ue_index;
  request.gnb_id                            = 1;
  request.nci                               = 1;

  // Action 1: Launch HO preparation procedure
  test_logger.info("Launch source NGAP handover preparation procedure");
  async_task<ngap_handover_preparation_response>         t = ngap->handle_handover_preparation_request(request);
  lazy_task_launcher<ngap_handover_preparation_response> t_launcher(t);

  // Status: AMF received Handover Required.
  ASSERT_EQ(msg_notifier.last_ngap_msg.pdu.type().value, asn1::ngap::ngap_pdu_c::types_opts::init_msg);
  ASSERT_EQ(msg_notifier.last_ngap_msg.pdu.init_msg().value.type().value,
            asn1::ngap::ngap_elem_procs_o::init_msg_c::types_opts::ho_required);

  ASSERT_FALSE(t.ready());

  // Inject Handover Command
  ngap_message ho_cmd = generate_valid_handover_command(ue.amf_ue_id.value(), ue.ran_ue_id.value());
  ngap->handle_message(ho_cmd);

  // Procedure should have succeeded.
  ASSERT_TRUE(t.ready());
  ASSERT_TRUE(t.get().success);
}
