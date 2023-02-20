/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "lib/du_manager/converters/mac_config_helpers.h"
#include "srsran/du/du_cell_config_helpers.h"
#include "srsran/mac/config/mac_cell_group_config_factory.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/mac/mac_ue_configurator.h"
#include "srsran/pcap/pcap.h"
#include "srsran/scheduler/mac_scheduler.h"
#include "srsran/support/test_utils.h"

namespace srsran {

namespace test_helpers {

/// Generates default MAC Cell Configuration to be used in unit tests.
inline mac_cell_creation_request make_default_mac_cell_config(const cell_config_builder_params& params = {})
{
  mac_cell_creation_request req{};

  req.pci              = params.pci;
  req.cell_index       = to_du_cell_index(0);
  req.scs_common       = params.scs_common;
  req.dl_carrier       = config_helpers::make_default_carrier_configuration(params);
  req.ul_carrier       = config_helpers::make_default_carrier_configuration(params);
  req.ssb_cfg          = config_helpers::make_default_ssb_config(params);
  req.cell_barred      = false;
  req.intra_freq_resel = false;
  req.sched_req        = {}; // Note: MAC should not touch this field.

  byte_buffer dummy_sib1;
  for (unsigned i = 0; i != 100; ++i) {
    dummy_sib1.append(i);
  }
  req.bcch_dl_sch_payload = std::move(dummy_sib1);
  return req;
}

inline mac_ue_create_request_message make_default_ue_creation_request()
{
  mac_ue_create_request_message msg{};

  msg.ue_index   = to_du_ue_index(0);
  msg.crnti      = to_rnti(0x4601);
  msg.cell_index = to_du_cell_index(0);

  msg.mac_cell_group_cfg = config_helpers::make_initial_mac_cell_group_config();

  physical_cell_group_config& pcg_cfg = msg.phy_cell_group_cfg;
  pcg_cfg.p_nr_fr1                    = 10;
  pcg_cfg.pdsch_harq_codebook         = pdsch_harq_ack_codebook::dynamic;

  msg.sched_cfg.cells.push_back(config_helpers::create_default_initial_ue_spcell_cell_config());

  return msg;
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
  void handle_paging_indication(const paging_indication_message& pi) override {}
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
  bool                          is_complete = false;

  void on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override { last_sched_res = dl_res; }
  void on_new_downlink_data(const mac_dl_data_result& dl_data) override { last_dl_data_res = dl_data; }
  void on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override { last_ul_res = ul_res; }
  void on_cell_results_completion(slot_point slot) override { is_complete = true; }
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
  mac_logical_channel_to_setup     bearer;
};

struct mac_test_ue {
  du_ue_index_t                                       ue_index;
  rnti_t                                              rnti;
  slotted_array<mac_test_ue_bearer, MAX_NOF_RB_LCIDS> bearers;
  unique_timer                                        activity_timer;

  void                          add_bearer(lcid_t lcid);
  mac_ue_create_request_message make_ue_create_request();
};

class dummy_mac_pcap : public mac_pcap
{
public:
  ~dummy_mac_pcap() override = default;

  void open(const char* filename_) override {}
  void close() override {}
  bool is_write_enabled() override { return false; }
  void push_pdu(mac_nr_context_info context, const_span<uint8_t> pdu) override {}
  void push_pdu(mac_nr_context_info context, byte_buffer pdu) override {}
};

} // namespace test_helpers

} // namespace srsran
