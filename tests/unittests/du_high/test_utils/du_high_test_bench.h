/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_high_worker_manager.h"
#include "dummy_test_components.h"
#include "lib/du_high/du_high.h"

namespace srsran {
namespace srs_du {

struct phy_cell_test_dummy : public mac_cell_result_notifier {
public:
  explicit phy_cell_test_dummy(task_executor& exec_);
  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override;
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override;
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override;
  void on_cell_results_completion(slot_point slot) override;

  optional<mac_dl_sched_result> last_dl_res;
  optional<mac_dl_data_result>  last_dl_data;
  optional<mac_ul_sched_result> last_ul_res;

private:
  task_executor&  test_exec;
  dl_sched_result last_dl_sched_res;
  ul_sched_result last_ul_sched_res;

  optional<mac_dl_sched_result> cached_dl_res;
  optional<mac_dl_data_result>  cached_dl_data;
  optional<mac_ul_sched_result> cached_ul_res;
};

struct phy_test_dummy : public mac_result_notifier {
public:
  phy_test_dummy(task_executor& exec_) : cell(exec_) {}

  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
  phy_cell_test_dummy       cell;
};

class dummy_f1ap_tx_pdu_notifier : public f1ap_message_notifier
{
public:
  dummy_f1ap_tx_pdu_notifier(task_executor& test_exec_);

  void connect(f1ap_message_handler& f1ap_du_msg_handler);

  void on_new_message(const f1ap_message& msg) override;

  f1ap_message          last_f1ap_msg;
  task_executor&        test_exec;
  f1ap_message_handler* f1ap_du = nullptr;
  srslog::basic_logger& logger  = srslog::fetch_basic_logger("TEST");
};

mac_rx_data_indication create_ccch_message(slot_point sl_rx, rnti_t rnti);

class du_high_test_bench
{
public:
  du_high_test_bench();
  ~du_high_test_bench();

  bool add_ue(rnti_t rnti);

  void run_slot();

  bool run_until(unique_function<bool()> condition);

  du_high_worker_manager     workers;
  dummy_f1ap_tx_pdu_notifier cu_notifier;
  phy_test_dummy             phy;
  mac_pcap_dummy             pcap;
  timer_manager2             timers;

  du_high du_obj;

  slot_point next_slot;
};

} // namespace srs_du
} // namespace srsran
