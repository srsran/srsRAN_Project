/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../cu_cp_test_helpers.h"
#include "lib/cu_cp/du_processor/du_processor_config.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "tests/unittests/f1ap/cu_cp/f1ap_cu_test_helpers.h"
#include "tests/unittests/rrc/test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Fixture class for CU-CP mobility tests
class mobility_test : public cu_cp_test
{
protected:
  mobility_test();
  ~mobility_test() override;

  ue_manager* get_ue_manager() { return &ue_mng; }

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  manual_task_worker  ctrl_worker{128};
  timer_manager       timers;
  cu_cp_configuration cu_cp_cfg;

  ue_manager                                  ue_mng{cu_cp_cfg};
  dummy_ngap_ue_context_removal_handler       ngap_ue_removal_handler;
  dummy_cu_cp_ue_context_manipulation_handler cu_cp_handler;
};

} // namespace srs_cu_cp
} // namespace srsran
