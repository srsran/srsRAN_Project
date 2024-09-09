/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngap_test_helpers.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "srsran/ngap/ngap_handover.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/lcid.h"
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
  ASSERT_TRUE(n2_gw.last_ngap_msgs.empty());
}
