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

#include "du_processor_test_helpers.h"
#include "srsran/cu_cp/cell_meas_manager.h"
#include "srsran/support/async/async_test_utils.h"

using namespace srsran;
using namespace srs_cu_cp;

du_processor_test::du_processor_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  // create ue task scheduler
  ue_task_sched = std::make_unique<dummy_du_processor_ue_task_scheduler>(timers, ctrl_worker);

  // create and start DU processor
  du_processor_config_t du_cfg = {};
  du_cfg.du_index              = uint_to_du_index(0);

  du_processor_obj = create_du_processor(std::move(du_cfg),
                                         cu_cp_notifier,
                                         f1ap_du_mgmt_notifier,
                                         f1ap_pdu_notifier,
                                         e1ap_ctrl_notifier,
                                         ngap_ctrl_notifier,
                                         rrc_ue_ngap_notifier,
                                         rrc_ue_ngap_notifier,
                                         rrc_ue_cu_cp_notifier,
                                         *ue_task_sched,
                                         ue_mng,
                                         cell_meas_mng,
                                         ctrl_worker);
}

du_processor_test::~du_processor_test()
{
  // flush logger after each test
  srslog::flush();
}

void du_processor_test::attach_ue()
{
  // Generate valid F1SetupRequest
  f1ap_f1_setup_request f1_setup_request_msg;
  generate_valid_f1_setup_request(f1_setup_request_msg);
  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Generate ue_creation message
  ue_index_t                ue_index        = ue_index_t::min;
  cu_cp_ue_creation_message ue_creation_msg = generate_ue_creation_message(ue_index, MIN_CRNTI, 6576);
  // Pass message to DU processor
  du_processor_obj->handle_ue_creation_request(ue_creation_msg);
}

void du_processor_test::receive_rrc_reconfig_complete()
{
  // inject RRC Reconfiguration complete into UE object
}
