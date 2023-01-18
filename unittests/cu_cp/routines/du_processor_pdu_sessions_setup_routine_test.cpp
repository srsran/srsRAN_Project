/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../du_processor_test_helpers.h"
#include "../pdu_session_setup_helpers.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/async/async_test_utils.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace srs_cu_cp;

/// Note: Check if UE ID is valid is done by caller. Injection of invalid cu_cp_ue_id results in assertion.

#if 0
/// Test handling of PDU session setup request when a UE connected.
TEST_F(du_processor_test, when_ue_connected_setup_succeeds)
{
  attach_ue();

  // Prepare args
  cu_cp_pdu_session_resource_setup_message msg = fill_res_setup_request();

  // Trigger PDU session setup
  async_task<cu_cp_pdu_session_resource_setup_response_message> t =
      du_processor_obj->handle_new_pdu_session_resource_setup_request(msg);
  lazy_task_launcher<cu_cp_pdu_session_resource_setup_response_message> t_launcher(t);

  ASSERT_FALSE(t.ready());

  // Receive Reconfig complete
  receive_rrc_reconfig_complete();

  ASSERT_TRUE(t.ready());

  // PDU session has been set up
  ASSERT_EQ(t.get().pdu_session_res_setup_response_items.size(), 1);
  ASSERT_EQ(t.get().pdu_session_res_setup_response_items[uint_to_pdu_session_id(1)].pdu_session_id,
            uint_to_pdu_session_id(1));

  // nothing has failed to setup
  ASSERT_EQ(t.get().pdu_session_res_failed_to_setup_items.size(), 0);
}
#endif

/// Test handling of PDU session setup request without any setup item.
TEST_F(du_processor_test, when_empty_pdu_session_setup_is_sent_response_is_sent)
{
  attach_ue();

  // Prepare args
  cu_cp_pdu_session_resource_setup_message msg = {}; // empty message
  msg.cu_cp_ue_id                              = uint_to_cu_cp_ue_id(0);

  // Trigger PDU session setup
  async_task<cu_cp_pdu_session_resource_setup_response_message> t =
      du_processor_obj->handle_new_pdu_session_resource_setup_request(msg);
  lazy_task_launcher<cu_cp_pdu_session_resource_setup_response_message> t_launcher(t);

  // it should be ready immediately
  ASSERT_TRUE(t.ready());

  // Nothing has been set up or failed
  ASSERT_EQ(t.get().pdu_session_res_setup_response_items.size(), 0);
  ASSERT_EQ(t.get().pdu_session_res_failed_to_setup_items.size(), 0);
}
