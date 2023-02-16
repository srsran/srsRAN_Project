/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/pcap/pcap.h"

namespace srsgnb {

struct phy_cell_test_dummy : public mac_cell_result_notifier {
  optional<mac_dl_sched_result> last_dl_res;
  optional<mac_dl_data_result>  last_dl_data;
  optional<mac_ul_sched_result> last_ul_res;

  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override { last_dl_res = dl_res; }
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override { last_dl_data = dl_data; }
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override { last_ul_res = ul_res; }
  void on_cell_results_completion(slot_point slot) override {}
};

struct phy_test_dummy : public mac_result_notifier {
public:
  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return cell; }
  phy_cell_test_dummy       cell;
};

struct mac_pcap_dummy : public mac_pcap {
public:
  void open(const char* filename_) override {}
  void close() override {}
  bool is_write_enabled() override { return false; }
  void push_pdu(mac_nr_context_info context, const_span<uint8_t> pdu) override {}
  void push_pdu(mac_nr_context_info context, byte_buffer pdu) override {}
};

} // namespace srsgnb
