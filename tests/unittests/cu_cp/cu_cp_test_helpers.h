/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

  bool check_minimal_paging_result();
  bool check_paging_result();

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
