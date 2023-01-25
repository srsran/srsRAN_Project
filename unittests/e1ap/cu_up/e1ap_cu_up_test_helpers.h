/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../common/e1ap_cu_up_test_messages.h"
#include "../common/test_helpers.h"
#include "srsgnb/e1/common/e1_common.h"
#include "srsgnb/e1/cu_up/e1_cu_up.h"
#include "srsgnb/e1/cu_up/e1_cu_up_factory.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_cu_up {

/// Fixture class for E1AP
class e1ap_cu_up_test : public ::testing::Test
{
protected:
  e1ap_cu_up_test();
  ~e1ap_cu_up_test() override;

  srslog::basic_logger& e1ap_logger = srslog::fetch_basic_logger("E1AP");
  srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");

  dummy_e1_pdu_notifier         msg_notifier;
  dummy_e1ap_cu_up_notifier     cu_up_notifier;
  std::unique_ptr<e1_interface> e1ap;
};

} // namespace srs_cu_up
} // namespace srsgnb
