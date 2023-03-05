/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_high_test_bench.h"
#include "srsran/du/du_cell_config_helpers.h"

using namespace srsran;
using namespace srs_du;

static f1ap_message create_f1_setup_response()
{
  f1ap_message f1ap_msg;
  f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  f1ap_msg.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list_present = true;
  f1ap_msg.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list.value.resize(1);
  return f1ap_msg;
}

static std::once_flag log_init_flag;

du_high_test_bench::du_high_test_bench() :
  pdu_handler(workers.ctrl_worker), du_obj([this]() {
    du_high_configuration cfg{};
    cfg.du_mng_executor = &workers.ctrl_worker;
    cfg.cell_executors  = &workers.cell_exec_mapper;
    cfg.ue_executors    = &workers.ue_exec_mapper;
    cfg.f1ap_notifier   = &cu_notifier;
    cfg.phy_adapter     = &phy;
    cfg.timers          = &timers;
    cfg.cells           = {config_helpers::make_default_du_cell_config()};
    cfg.sched_cfg       = config_helpers::make_default_scheduler_expert_config();
    cfg.pcap            = &pcap;
    return cfg;
  }())
{
  // Init relevant loggers.
  std::call_once(log_init_flag, []() {
    srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("DU-MNG").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("DU-F1").set_level(srslog::basic_levels::debug);
    srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
    srslog::init();
    return true;
  });

  // Start DU and try to connect to CU.
  du_obj.start();

  // Push F1AP setup response to DU, signaling that the CU accepted the F1 connection.
  du_obj.get_f1ap_message_handler().handle_message(create_f1_setup_response());
}
