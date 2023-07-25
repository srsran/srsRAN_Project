/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mobility_test_helpers.h"
#include "lib/cu_cp/du_processor/du_processor_factory.h"
#include "srsran/cu_cp/cell_meas_manager.h"
#include "srsran/support/async/async_test_utils.h"

using namespace srsran;
using namespace srs_cu_cp;

mobility_test::mobility_test()
{
  test_logger.set_level(srslog::basic_levels::debug);
  cu_cp_logger.set_level(srslog::basic_levels::debug);
  srslog::init();
}

mobility_test::~mobility_test()
{
  // flush logger after each test
  srslog::flush();
}

du_wrapper* mobility_test::create_du(const du_processor_config_t& du_cfg)
{
  auto        it     = du_db.emplace(du_cfg.du_index, du_wrapper{});
  du_wrapper& new_du = it.first->second;

  // create ue task scheduler
  new_du.ue_task_sched = std::make_unique<dummy_du_processor_ue_task_scheduler>(timers, ctrl_worker);

  // create DU processor
  new_du.du_processor_obj = create_du_processor(std::move(du_cfg),
                                                new_du.cu_cp_notifier,
                                                new_du.f1ap_du_mgmt_notifier,
                                                new_du.f1ap_pdu_notifier,
                                                new_du.e1ap_ctrl_notifier,
                                                new_du.ngap_ctrl_notifier,
                                                new_du.rrc_ue_ngap_notifier,
                                                new_du.rrc_ue_ngap_notifier,
                                                new_du.rrc_ue_cu_cp_notifier,
                                                *new_du.ue_task_sched,
                                                ue_mng,
                                                new_du.cell_meas_mng,
                                                ctrl_worker);

  // Connect test adapter.
  new_du.f1ap_pdu_notifier.attach_handler(&new_du.test_adapter);
  new_du.test_adapter.set_handler(
      &new_du.du_processor_obj->get_du_processor_f1ap_interface().get_f1ap_message_handler());

  return &new_du;
}

void mobility_test::attach_du_to_cu(du_wrapper& du, unsigned gnb_du_id, unsigned nrcell_id, pci_t pci)
{
  // Generate valid F1SetupRequest
  f1ap_f1_setup_request f1_setup_request_msg;
  generate_valid_f1_setup_request(f1_setup_request_msg, gnb_du_id, nrcell_id, pci);

  // Pass message to DU processor
  du.du_processor_obj->handle_f1_setup_request(f1_setup_request_msg);
}

ue_creation_complete_message mobility_test::attach_ue(du_wrapper& du, unsigned nrcell_id)
{
  // Generate ue_creation message
  ue_creation_message ue_creation_msg = generate_ue_creation_message(MIN_CRNTI, nrcell_id);
  // Pass message to DU processor
  return du.du_processor_obj->handle_ue_creation_request(ue_creation_msg);
}