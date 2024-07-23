/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "../test_helpers.h"
#include "du_processor_test_helpers.h"
#include "lib/cu_cp/cu_cp_controller/cu_cp_ue_admission_controller.h"
#include "lib/cu_cp/du_processor/du_configuration_manager.h"
#include "lib/cu_cp/du_processor/du_processor.h"
#include "lib/cu_cp/du_processor/du_processor_factory.h"
#include "lib/cu_cp/ue_manager/ue_manager_impl.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "tests/unittests/rrc/test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/executors/manual_task_worker.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

/// Fixture class for DU processor creation
class du_processor_test : public ::testing::Test
{
protected:
  du_processor_test();
  ~du_processor_test() override;

  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  timer_manager       timers;
  manual_task_worker  ctrl_worker{128};
  cu_cp_configuration cu_cp_cfg;

  ue_manager                              ue_mng{cu_cp_cfg};
  dummy_ngap_ue_context_removal_handler   ngap_ue_removal_handler;
  dummy_du_processor_cu_cp_notifier       cu_cp_notifier{&ue_mng};
  dummy_du_connection_notifier            du_conn_notifier;
  dummy_f1ap_pdu_notifier                 f1ap_pdu_notifier;
  dummy_rrc_ue_ngap_adapter               rrc_ue_ngap_notifier;
  dummy_rrc_ue_cu_cp_adapter              rrc_ue_cu_cp_notifier;
  dummy_rrc_du_cu_cp_adapter              rrc_du_cu_cp_notifier;
  std::unique_ptr<cu_cp_du_event_handler> cu_cp_event_handler;
  std::unique_ptr<common_task_scheduler>  common_task_sched;
  du_configuration_manager                du_cfg_mgr;
  std::unique_ptr<du_processor>           du_processor_obj;

  async_task<ue_index_t>                        t;
  std::optional<lazy_task_launcher<ue_index_t>> t_launcher;
};

} // namespace srs_cu_cp
} // namespace srsran
