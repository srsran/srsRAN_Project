/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_processor_test_helpers.h"
#include "srsgnb/support/async/async_test_utils.h"

using namespace srsgnb;
using namespace srs_cu_cp;

du_processor_test::du_processor_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();

  ue_task_sched = std::make_unique<dummy_du_processor_to_cu_cp_task_scheduler>(timers);

  // create and start DU processor
  du_processor_config_t du_cfg = {};
  du_cfg.du_index              = uint_to_du_index(0);

  du_processor_obj = create_du_processor(std::move(du_cfg),
                                         cu_cp_notifier,
                                         f1c_du_mgmt_notifier,
                                         f1c_pdu_notifier,
                                         e1ap_ctrl_notifier,
                                         rrc_ue_ngc_notifier,
                                         rrc_ue_ngc_notifier,
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
  f1_setup_request_message f1_setup_request_msg = generate_valid_f1_setup_request();
  // Pass message to DU processor
  du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);

  // Generate ue_creation message
  ue_creation_message ue_creation_msg = generate_ue_creation_message(MIN_CRNTI, 12345678);
  // Pass message to DU processor
  du_processor_obj->handle_ue_creation_request(ue_creation_msg);
}

void du_processor_test::receive_rrc_reconfig_complete()
{
  // inject RRC Reconfiguration complete into UE object
}
