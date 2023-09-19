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

#include "du_high_test_bench.h"
#include "tests/test_doubles/f1ap/f1ap_test_message_validators.h"
#include "tests/test_doubles/mac/mac_test_messages.h"
#include "tests/unittests/f1ap/du/f1ap_du_test_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/du_high/du_high_factory.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace srs_du;

static f1ap_message create_f1_setup_response()
{
  f1ap_message f1ap_msg;
  f1ap_msg.pdu.set_successful_outcome().load_info_obj(ASN1_F1AP_ID_F1_SETUP);
  f1ap_msg.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list_present = true;
  f1ap_msg.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list.resize(1);
  auto& cell = f1ap_msg.pdu.successful_outcome().value.f1_setup_resp()->cells_to_be_activ_list[0];
  cell.load_info_obj(ASN1_F1AP_ID_CELLS_TO_BE_ACTIV_LIST_ITEM);
  cell->cells_to_be_activ_list_item().nr_cgi.plmn_id.from_string("00f101");
  cell->cells_to_be_activ_list_item().nr_cgi.nr_cell_id.from_string("000000000000101111000110000101001110");
  return f1ap_msg;
}

namespace {

/// Dummy F1AP Tx PDU notifier that emulates the CU-CP side.
class dummy_du_f1ap_tx_pdu_notifier : public f1ap_message_notifier
{
public:
  dummy_du_f1ap_tx_pdu_notifier(task_executor&                         test_exec_,
                                std::vector<f1ap_message>&             last_f1ap_msgs_,
                                std::unique_ptr<f1ap_message_notifier> du_rx_notifier_) :
    test_exec(test_exec_), last_f1ap_msgs(last_f1ap_msgs_), du_rx_notifier(std::move(du_rx_notifier_))
  {
  }

  void on_new_message(const f1ap_message& msg) override
  {
    if (msg.pdu.type().value == asn1::f1ap::f1ap_pdu_c::types_opts::init_msg and
        msg.pdu.init_msg().proc_code == ASN1_F1AP_ID_F1_SETUP) {
      // Auto-schedule CU response.
      du_rx_notifier->on_new_message(create_f1_setup_response());
    }

    // Dispatch storing of message to test main thread so it can be safely checked in the test function body.
    bool result = test_exec.execute([this, msg]() {
      logger.info("Received F1 UL message with {}", msg.pdu.type().to_string());
      last_f1ap_msgs.push_back(msg);
    });
    EXPECT_TRUE(result);
  }

  srslog::basic_logger&                  logger = srslog::fetch_basic_logger("TEST");
  task_executor&                         test_exec;
  std::vector<f1ap_message>&             last_f1ap_msgs;
  std::unique_ptr<f1ap_message_notifier> du_rx_notifier;
};

} // namespace

bool srsran::srs_du::is_ue_context_release_complete_valid(const f1ap_message& msg,
                                                          gnb_du_ue_f1ap_id_t du_ue_id,
                                                          gnb_cu_ue_f1ap_id_t cu_ue_id)
{
  if (not(msg.pdu.type() == asn1::f1ap::f1ap_pdu_c::types_opts::successful_outcome and
          msg.pdu.successful_outcome().proc_code == ASN1_F1AP_ID_UE_CONTEXT_RELEASE)) {
    return false;
  }
  const asn1::f1ap::ue_context_release_complete_s& resp =
      msg.pdu.successful_outcome().value.ue_context_release_complete();
  return (gnb_cu_ue_f1ap_id_t)resp->gnb_cu_ue_f1ap_id == cu_ue_id and (gnb_du_ue_f1ap_id_t)
                                                                              resp->gnb_du_ue_f1ap_id == du_ue_id;
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
  bool result =
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
  EXPECT_TRUE(result);
  cached_dl_res  = {};
  cached_dl_data = {};
  cached_ul_res  = {};
}

dummy_f1c_test_client::dummy_f1c_test_client(task_executor& test_exec_) : test_exec(test_exec_) {}

std::unique_ptr<f1ap_message_notifier>
dummy_f1c_test_client::handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier)
{
  return std::make_unique<dummy_du_f1ap_tx_pdu_notifier>(test_exec, last_f1ap_msgs, std::move(du_rx_pdu_notifier));
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
  du_high_cfg([this]() {
    init_loggers();

    du_high_configuration cfg{};
    cfg.exec_mapper  = &workers.exec_mapper;
    cfg.f1c_client   = &cu_notifier;
    cfg.f1u_gw       = &cu_up_sim;
    cfg.phy_adapter  = &phy;
    cfg.timers       = &timers;
    cfg.gnb_du_id    = 0;
    cfg.gnb_du_name  = "srsdu";
    cfg.du_bind_addr = {"127.0.0.1"};
    cfg.cells        = {config_helpers::make_default_du_cell_config()};
    cfg.qos          = config_helpers::make_default_du_qos_config_list(1000);
    cfg.sched_cfg    = config_helpers::make_default_scheduler_expert_config();
    cfg.pcap         = &pcap;

    return cfg;
  }()),
  du_hi(make_du_high(du_high_cfg)),
  next_slot(0, test_rgen::uniform_int<unsigned>(0, 10239))
{
  // Start DU and try to connect to CU.
  du_hi->start();

  // Ensure the result is saved in the notifier.
  run_until([this]() { return not cu_notifier.last_f1ap_msgs.empty(); });
}

du_high_test_bench::~du_high_test_bench()
{
  du_hi->stop();

  // Stop workers before starting to take down other components.
  workers.stop();
}

bool du_high_test_bench::run_until(unique_function<bool()> condition, unsigned max_slot_count)
{
  for (unsigned count = 0; count != max_slot_count; ++count) {
    if (condition()) {
      return true;
    }
    run_slot();
  }
  return false;
}

bool du_high_test_bench::add_ue(rnti_t rnti)
{
  cu_notifier.last_f1ap_msgs.clear();

  // Send UL-CCCH message.
  du_hi->get_pdu_handler().handle_rx_data_indication(test_helpers::create_ccch_message(next_slot, rnti));

  // Wait for Init UL RRC Message to come out of the F1AP.
  bool ret = run_until([this]() { return not cu_notifier.last_f1ap_msgs.empty(); }, 100);
  return ret and test_helpers::is_init_ul_rrc_msg_transfer_valid(cu_notifier.last_f1ap_msgs.back(), rnti);
}

void du_high_test_bench::run_slot()
{
  // Signal slot indication to l2.
  du_hi->get_slot_handler(to_du_cell_index(0)).handle_slot_indication(next_slot);

  // Wait for slot indication to be processed and the l2 results to be sent back to the l1 (in this case, the test main
  // thread).
  const unsigned                       MAX_COUNT = 1000;
  const optional<mac_dl_sched_result>& dl_result = phy.cell.last_dl_res;
  for (unsigned count = 0; count < MAX_COUNT and (not dl_result.has_value() or dl_result->slot != next_slot); ++count) {
    // Process tasks dispatched to the test main thread (e.g. L2 slot result)
    workers.test_worker.run_pending_tasks();

    // Wait for tasks to arrive to test thread.
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
  EXPECT_TRUE(dl_result.has_value() and dl_result->slot == next_slot);

  ++next_slot;
}
