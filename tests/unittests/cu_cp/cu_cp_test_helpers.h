/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/cu_cp/cu_cp.h"
#include "test_helpers.h"
#include "tests/unittests/e1ap/common/e1ap_cu_cp_test_messages.h"
#include "tests/unittests/e1ap/common/test_helpers.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "tests/unittests/f1ap/cu_cp/f1ap_cu_test_helpers.h"
#include "tests/unittests/ngap/ngap_test_messages.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Fixture class for CU-CP test
class cu_cp_test : public ::testing::Test
{
protected:
  cu_cp_test();
  ~cu_cp_test() override;

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  dummy_f1ap_pdu_notifier f1ap_pdu_notifier;
  dummy_e1ap_pdu_notifier e1ap_pdu_notifier;
  dummy_ngap_amf_notifier ngap_amf_notifier;

  manual_task_worker ctrl_worker{128};

  std::unique_ptr<cu_cp> cu_cp_obj;
};

} // namespace srs_cu_cp
} // namespace srsran
