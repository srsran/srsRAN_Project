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

#include "../common/du_processor_test_messages.h"
#include "../common/test_helpers.h"
#include "du_processor_test_helpers.h"
#include "lib/cu_cp/ue_manager_impl.h"
#include "lib/f1ap/common/asn1_helpers.h"
#include "unittests/f1ap/common/test_helpers.h"
#include "unittests/f1ap/cu_cp/f1ap_cu_test_helpers.h"
#include "unittests/rrc/test_helpers.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/du_processor.h"
#include "srsgnb/cu_cp/du_processor_factory.h"
#include "srsgnb/rrc/rrc.h"
#include "srsgnb/support/executors/manual_task_worker.h"
#include "srsgnb/support/test_utils.h"
#include <gtest/gtest.h>

namespace srsgnb {
namespace srs_cu_cp {

/// Fixture class for DU processor creation
class du_processor_test : public ::testing::Test
{
protected:
  du_processor_test();
  ~du_processor_test() override;

  void attach_ue();

  void receive_rrc_reconfig_complete();

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  timer_manager                                         timers;
  ue_manager                                            ue_mng;
  dummy_du_processor_cu_cp_notifier                     cu_cp_notifier;
  dummy_f1c_pdu_notifier                                f1c_pdu_notifier;
  dummy_f1c_du_management_notifier                      f1c_du_mgmt_notifier;
  dummy_du_processor_e1ap_control_notifier              e1ap_ctrl_notifier;
  dummy_rrc_ue_ngc_adapter                              rrc_ue_ngc_notifier;
  std::unique_ptr<dummy_du_processor_ue_task_scheduler> ue_task_sched;
  manual_task_worker                                    ctrl_worker{128};
  std::unique_ptr<du_processor_interface>               du_processor_obj;
};

} // namespace srs_cu_cp
} // namespace srsgnb
