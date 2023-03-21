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

#include "../common/e1ap_cu_up_test_messages.h"
#include "../common/test_helpers.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_factory.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_up {

/// Fixture class for E1AP
class e1ap_cu_up_test : public ::testing::Test
{
protected:
  e1ap_cu_up_test();
  ~e1ap_cu_up_test() override;

  /// \brief Helper method to setup a bearer at the CU-UP
  void setup_bearer(unsigned int cu_cp_ue_e1ap_id);

  srslog::basic_logger& e1ap_logger = srslog::fetch_basic_logger("E1AP");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  dummy_e1ap_pdu_notifier         msg_notifier;
  dummy_e1ap_cu_up_notifier       cu_up_notifier;
  manual_task_worker              cu_up_worker{128};
  std::unique_ptr<e1ap_interface> e1ap;
};

} // namespace srs_cu_up
} // namespace srsran
