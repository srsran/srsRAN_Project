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
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

/// Test successful handover preparation procedure
TEST_F(ngap_test, when_source_gnb_handover_preperation_triggered_then_ho_command_received)
{
  // Action 1: Launch HO preparation procedure
  test_logger.info("Launch source NGAP handover preparation procedure");
  async_task<cu_cp_ngap_handover_preperation_response>         t = ngap->handle_handover_preparation_request();
  lazy_task_launcher<cu_cp_ngap_handover_preperation_response> t_launcher(t);
}
