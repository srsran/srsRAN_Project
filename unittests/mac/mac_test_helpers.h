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

#include "lib/du_manager/converters/mac_cell_configuration_helpers.h"
#include "srsgnb/du/du_cell_config_helpers.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/scheduler/mac_scheduler.h"
#include "srsgnb/support/test_utils.h"

namespace srsgnb {

namespace test_helpers {

/// Generates default MAC Cell Configuration to be used in unit tests.
inline mac_cell_creation_request make_default_mac_cell_config()
{
  du_cell_config            default_du_cell_cfg = config_helpers::make_default_du_cell_config();
  mac_cell_creation_request req                 = make_mac_cell_config(to_du_cell_index(0), default_du_cell_cfg);
  return req;
}

class dummy_mac_scheduler : public mac_scheduler
{
public:
  sched_result next_sched_result;

  bool handle_cell_configuration_request(const sched_cell_configuration_request_message& msg) override { return true; }
  void handle_rach_indication(const rach_indication_message& msg) override {}
  void handle_ue_creation_request(const sched_ue_creation_request_message& ue_request) override {}
  void handle_ue_reconfiguration_request(const sched_ue_reconfiguration_message& ue_request) override {}
  void handle_ue_removal_request(du_ue_index_t ue_index) override {}
  void handle_ul_bsr_indication(const ul_bsr_indication_message& bsr) override {}
  void handle_crc_indication(const ul_crc_indication& crc) override {}
  void handle_uci_indication(const uci_indication& uci) override {}
  void handle_dl_mac_ce_indication(const dl_mac_ce_indication& mac_ce) override {}
  const sched_result* slot_indication(slot_point sl_tx, du_cell_index_t cell_index) override
  {
    return &next_sched_result;
  }
  void handle_dl_buffer_state_indication(const dl_buffer_state_indication_message& bs) override {}
};

class dummy_mac_cell_result_notifier : public mac_cell_result_notifier
{
public:
  optional<mac_dl_sched_result> last_sched_res;
  optional<mac_dl_data_result>  last_dl_data_res;
  optional<mac_ul_sched_result> last_ul_res;

  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override { last_sched_res = dl_res; }
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override { last_dl_data_res = dl_data; }
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override { last_ul_res = ul_res; }
};

class mac_sdu_rx_test_notifier : public mac_sdu_rx_notifier
{
public:
  byte_buffer_slice last_sdu;

  void on_new_sdu(byte_buffer_slice sdu) override { last_sdu = std::move(sdu); }
};

class mac_sdu_tx_builder_test_notifier : public mac_sdu_tx_builder
{
public:
  unsigned    next_bs = 0;
  byte_buffer previous_tx_sdu;

  byte_buffer_slice_chain on_new_tx_sdu(unsigned nof_bytes) override
  {
    previous_tx_sdu = test_rgen::random_vector<uint8_t>(nof_bytes);
    return byte_buffer_slice_chain{previous_tx_sdu.copy()};
  }
  unsigned on_buffer_state_update() override { return next_bs; }
};

struct mac_test_ue_bearer {
  mac_sdu_rx_test_notifier         ul_notifier;
  mac_sdu_tx_builder_test_notifier dl_notifier;
  mac_logical_channel              bearer;
};

struct mac_test_ue {
  du_ue_index_t                                       ue_index;
  rnti_t                                              rnti;
  slotted_array<mac_test_ue_bearer, MAX_NOF_RB_LCIDS> bearers;
  unique_timer                                        activity_timer;

  void                          add_bearer(lcid_t lcid);
  mac_ue_create_request_message make_ue_create_request();
};

} // namespace test_helpers

} // namespace srsgnb
