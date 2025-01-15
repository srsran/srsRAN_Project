/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../test_helpers.h"
#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "tests/unittests/ngap/test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>
#include <memory>

namespace srsran {
namespace srs_cu_cp {

/// Fixture class for UE manager creation
class ue_manager_test : public ::testing::Test
{
protected:
  ue_manager_test();
  ~ue_manager_test() override;

  srslog::basic_logger& test_logger   = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& ue_mng_logger = srslog::fetch_basic_logger("CU-UEMNG");

  unsigned ues_per_du = 1024;

  timer_manager       timers;
  manual_task_worker  cu_worker{128};
  cu_cp_configuration cu_cp_cfg;

  ue_manager ue_mng{cu_cp_cfg};

  // DU processor to RRC UE adapters
  dummy_ngap_rrc_ue_notifier rrc_ue_pdu_notifier;
};

} // namespace srs_cu_cp
} // namespace srsran
