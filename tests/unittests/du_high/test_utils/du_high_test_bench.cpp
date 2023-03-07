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
#include "tests/unittests/f1ap/du/f1ap_du_test_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/support/test_utils.h"

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

mac_rx_data_indication srsran::srs_du::create_ccch_message(slot_point sl_rx, rnti_t rnti)
{
  return mac_rx_data_indication{
      sl_rx,
      to_du_cell_index(0),
      {mac_rx_pdu{rnti, 0, 0, {0x34, 0x1e, 0x4f, 0xc0, 0x4f, 0xa6, 0x06, 0x3f, 0x00, 0x00, 0x00}}}};
}

phy_cell_test_dummy::phy_cell_test_dummy(task_executor& exec_) : test_exec(exec_) {}

void phy_cell_test_dummy::on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res)
{
  cached_dl_res = dl_res;
}

void phy_cell_test_dummy::on_new_downlink_data(const mac_dl_data_result& dl_data)
{
  cached_dl_data = dl_data;
}

void phy_cell_test_dummy::on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res)
{
  cached_ul_res = ul_res;
}

void phy_cell_test_dummy::on_cell_results_completion(slot_point slot)
{
  test_exec.execute([this,
                     dl_sched_res = cached_dl_res.has_value() ? *cached_dl_res->dl_res : dl_sched_result{},
                     ul_sched_res = cached_ul_res.has_value() ? *cached_ul_res->ul_res : ul_sched_result{},
                     dl_res_copy  = cached_dl_res,
                     dl_data_copy = cached_dl_data,
                     ul_res_copy  = cached_ul_res]() mutable {
    last_dl_res  = dl_res_copy;
    last_dl_data = dl_data_copy;
    last_ul_res  = ul_res_copy;
    if (last_dl_res.has_value()) {
      last_dl_sched_res   = dl_sched_res;
      last_dl_res->dl_res = &last_dl_sched_res;
    }
    if (last_ul_res.has_value()) {
      last_ul_sched_res   = ul_sched_res;
      last_ul_res->ul_res = &last_ul_sched_res;
    }
  });
  cached_dl_res  = {};
  cached_dl_data = {};
  cached_ul_res  = {};
}

dummy_f1ap_tx_pdu_notifier::dummy_f1ap_tx_pdu_notifier(task_executor& test_exec_) :
  test_exec(test_exec_), logger{srslog::fetch_basic_logger("TEST")}
{
}

void dummy_f1ap_tx_pdu_notifier::connect(f1ap_message_handler& f1ap_du_msg_handler)
{
  f1ap_du = &f1ap_du_msg_handler;
}

void dummy_f1ap_tx_pdu_notifier::on_new_message(const f1ap_message& msg)
{
  if (msg.pdu.type().value == asn1::f1ap::f1ap_pdu_c::types_opts::init_msg and
      msg.pdu.init_msg().proc_code == ASN1_F1AP_ID_F1_SETUP) {
    // Auto-schedule CU response.
    f1ap_du->handle_message(create_f1_setup_response());
  }

  // Dispatch storing of message to test main thread so it can be safely checked in the test function body.
  test_exec.execute([this, msg]() {
    logger.info("Received F1 UL message with {}", msg.pdu.type().to_string());
    last_f1ap_msg = msg;
  });
}

static void init_loggers()
{
  srslog::fetch_basic_logger("MAC", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("SCHED", true).set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("RLC").set_level(srslog::basic_levels::info);
  srslog::fetch_basic_logger("DU-MNG").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("DU-F1").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("ASN1").set_level(srslog::basic_levels::debug);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::debug);
  srslog::init();
}

du_high_test_bench::du_high_test_bench() :
  cu_notifier(workers.test_worker),
  phy(workers.test_worker),
  du_obj([this]() {
    init_loggers();

    du_high_configuration cfg{};
    cfg.du_mng_executor = &workers.ctrl_exec;
    cfg.cell_executors  = &workers.cell_exec_mapper;
    cfg.ue_executors    = &workers.ue_exec_mapper;
    cfg.f1ap_notifier   = &cu_notifier;
    cfg.phy_adapter     = &phy;
    cfg.timers          = &timers;
    cfg.cells           = {config_helpers::make_default_du_cell_config()};
    cfg.sched_cfg       = config_helpers::make_default_scheduler_expert_config();
    cfg.pcap            = &pcap;

    return cfg;
  }()),
  next_slot(0, test_rgen::uniform_int<unsigned>(0, 10239))
{
  // Short-circuit the CU notifier to the F1AP-DU for automatic CU-CP responses.
  cu_notifier.connect(du_obj.get_f1ap_message_handler());

  // Start DU and try to connect to CU.
  du_obj.start();

  // Ensure the result is saved in the notifier.
  run_until(
      [this]() { return cu_notifier.last_f1ap_msg.pdu.type().value == asn1::f1ap::f1ap_pdu_c::types_opts::init_msg; });
}

du_high_test_bench::~du_high_test_bench()
{
  // Stop workers before starting to take down other components.
  workers.stop();
}

bool du_high_test_bench::run_until(unique_function<bool()> condition)
{
  const unsigned MAX_COUNT = 1000;
  for (unsigned count = 0; count != MAX_COUNT; ++count) {
    if (condition()) {
      return true;
    }
    run_slot();
  }
  return false;
}

bool du_high_test_bench::add_ue(rnti_t rnti)
{
  cu_notifier.last_f1ap_msg.pdu = {};

  // Send UL-CCCH message.
  du_obj.get_pdu_handler(to_du_cell_index(0)).handle_rx_data_indication(create_ccch_message(next_slot, rnti));

  // Wait for Init UL RRC Message to come out of the F1AP.
  run_until([this]() { return cu_notifier.last_f1ap_msg.pdu.type() == asn1::f1ap::f1ap_pdu_c::types_opts::init_msg; });
  const asn1::f1ap::init_ul_rrc_msg_transfer_s& msg =
      cu_notifier.last_f1ap_msg.pdu.init_msg().value.init_ul_rrc_msg_transfer();
  return msg->c_rnti->value == rnti;
}

void du_high_test_bench::run_slot()
{
  du_obj.get_slot_handler(to_du_cell_index(0)).handle_slot_indication(next_slot);

  // Need to yield control of main thread.
  std::this_thread::sleep_for(std::chrono::milliseconds{1});

  workers.test_worker.run_pending_tasks();

  ++next_slot;
}
