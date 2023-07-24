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

#include "../du_processor/du_processor_test_helpers.h"
#include "../du_processor_test_messages.h"
#include "../test_helpers.h"
#include "lib/cu_cp/routine_managers/cu_cp_routine_manager.h"
#include "lib/cu_cp/ue_manager_impl.h"
#include "lib/f1ap/common/asn1_helpers.h"
#include "tests/unittests/f1ap/common/test_helpers.h"
#include "tests/unittests/f1ap/cu_cp/f1ap_cu_test_helpers.h"
#include "tests/unittests/rrc/test_helpers.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/cu_cp/du_processor.h"
#include "srsran/rrc/rrc.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

namespace srsran {
namespace srs_cu_cp {

struct du_wrapper {
  du_wrapper() : cu_cp_notifier(nullptr), f1ap_pdu_notifier(nullptr){};

  dummy_cell_meas_manager                               cell_meas_mng;
  dummy_du_processor_cu_cp_notifier                     cu_cp_notifier;
  dummy_f1ap_pdu_notifier                               f1ap_pdu_notifier;
  dummy_f1ap_du_management_notifier                     f1ap_du_mgmt_notifier;
  dummy_du_processor_e1ap_control_notifier              e1ap_ctrl_notifier;
  dummy_du_processor_ngap_control_notifier              ngap_ctrl_notifier;
  dummy_rrc_ue_ngap_adapter                             rrc_ue_ngap_notifier;
  dummy_rrc_ue_cu_cp_adapter                            rrc_ue_cu_cp_notifier;
  std::unique_ptr<dummy_du_processor_ue_task_scheduler> ue_task_sched;
  std::unique_ptr<du_processor_interface>               du_processor_obj;
};

/// Fixture class for CU-CP mobility tests
class mobility_test : public ::testing::Test
{
protected:
  mobility_test();
  ~mobility_test() override;

  du_wrapper*                  create_du(const du_processor_config_t& du_cfg);
  void                         attach_du_to_cu(du_wrapper& du, unsigned gnb_du_id, unsigned nrcell_id, pci_t pci);
  ue_creation_complete_message attach_ue(du_wrapper& du, unsigned nrcell_id);

private:
  srslog::basic_logger& test_logger  = srslog::fetch_basic_logger("TEST");
  srslog::basic_logger& cu_cp_logger = srslog::fetch_basic_logger("CU-CP");

  manual_task_worker ctrl_worker{128};
  timer_manager      timers;
  ue_configuration   ue_config;
  ue_manager         ue_mng{ue_config};

  std::unordered_map<du_index_t, du_wrapper> du_db;
};

} // namespace srs_cu_cp
} // namespace srsran
