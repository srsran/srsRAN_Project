/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_processor_routine_manager_test_helpers.h"
#include "inter_ngran_node_n2_handover_routine_test_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace srs_cu_cp;

TEST_F(inter_ngran_node_n2_handover_routine_test, when_ue_invalid_procedure_fails)
{
  // Test Preamble.
  cu_cp_inter_ngran_node_n2_handover_request request = {};

  // it should be ready immediately
  start_procedure(request);
}
