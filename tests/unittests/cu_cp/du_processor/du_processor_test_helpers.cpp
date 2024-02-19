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

#include "du_processor_test_helpers.h"
#include "tests/unittests/cu_cp/test_helpers.h"
#include "tests/unittests/f1ap/common/f1ap_cu_test_messages.h"
#include "srsran/support/async/async_test_utils.h"
#include <memory>

using namespace srsran;
using namespace srs_cu_cp;

du_processor_test::du_processor_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  cu_cp_notifier = std::make_unique<dummy_du_processor_cu_cp_notifier>(ngap_ue_removal_handler, &ue_mng);

  // create ue task scheduler
  ue_task_sched = std::make_unique<dummy_du_processor_ue_task_scheduler>(timers, ctrl_worker);

  // create and start DU processor
  du_processor_config_t du_cfg = {};
  du_cfg.du_index              = uint_to_du_index(0);
  du_cfg.du_setup_notif        = &du_conn_notifier;

  du_processor_obj = create_du_processor(std::move(du_cfg),
                                         *cu_cp_notifier,
                                         f1ap_du_mgmt_notifier,
                                         f1ap_pdu_notifier,
                                         e1ap_ctrl_notifier,
                                         ngap_ctrl_notifier,
                                         f1ap_cu_cp_notifier,
                                         rrc_ue_ngap_notifier,
                                         rrc_ue_ngap_notifier,
                                         rrc_du_cu_cp_notifier,
                                         *ue_task_sched,
                                         ue_mng,
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
  f1ap_message f1_setup_req = generate_f1_setup_request();
  // Pass message to DU processor
  du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler().handle_message(f1_setup_req);

  // Generate ue_creation message
  ue_index_t                      ue_index = ue_index_t::min;
  ue_rrc_context_creation_request req = generate_ue_rrc_context_creation_request(ue_index, rnti_t::MIN_CRNTI, 6576);
  // Pass message to DU processor
  du_processor_obj->handle_ue_rrc_context_creation_request(req);
}

void du_processor_test::receive_rrc_reconfig_complete()
{
  // inject RRC Reconfiguration complete into UE object
}
