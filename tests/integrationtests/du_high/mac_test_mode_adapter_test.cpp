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

#include "lib/du_high/adapters/mac_test_mode_adapter.h"
#include "tests/unittests/mac/mac_test_helpers.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

struct mac_event_interceptor {
  optional<mac_ul_sched_result> next_ul_sched_res;

  optional<mac_ue_create_request> last_ue_created;

  optional<mac_uci_indication_message> last_uci;
  optional<mac_crc_indication_message> last_crc;
};

class phy_dummy : public mac_result_notifier, public mac_cell_result_notifier
{
public:
  mac_cell_result_notifier& get_cell(du_cell_index_t cell_index) override { return *this; }
  void                      on_new_downlink_scheduler_results(const mac_dl_sched_result& dl_res) override {}
  void                      on_new_downlink_data(const mac_dl_data_result& dl_data) override {}
  void                      on_new_uplink_scheduler_results(const mac_ul_sched_result& ul_res) override {}
  void                      on_cell_results_completion(slot_point slot) override {}
};

class mac_dummy : public mac_interface,
                  public mac_cell_rach_handler,
                  public mac_cell_control_information_handler,
                  public mac_ue_control_information_handler,
                  public mac_cell_slot_handler,
                  public mac_cell_manager,
                  public mac_ue_configurator,
                  public mac_pdu_handler,
                  public mac_paging_information_handler,
                  public mac_cell_controller
{
public:
  mac_event_interceptor& events;
  mac_result_notifier&   result_notifier;

  mac_dummy(mac_event_interceptor& interceptor_, mac_result_notifier& result_notifier_) :
    events(interceptor_), result_notifier(result_notifier_)
  {
  }

  mac_cell_rach_handler&                get_rach_handler(du_cell_index_t cell_index) override { return *this; }
  mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) override { return *this; }
  mac_ue_control_information_handler&   get_ue_control_info_handler() override { return *this; }
  mac_cell_slot_handler&                get_slot_handler(du_cell_index_t cell_index) override { return *this; }
  mac_cell_manager&                     get_cell_manager() override { return *this; }
  mac_ue_configurator&                  get_ue_configurator() override { return *this; }
  mac_pdu_handler&                      get_pdu_handler() override { return *this; }
  mac_paging_information_handler&       get_cell_paging_info_handler() override { return *this; }

  void handle_rach_indication(const mac_rach_indication& rach_ind) override {}
  void handle_crc(const mac_crc_indication_message& msg) override { events.last_crc = msg; }
  void handle_uci(const mac_uci_indication_message& msg) override { events.last_uci = msg; }
  void handle_dl_buffer_state_update(const mac_dl_buffer_state_indication_message& dl_bs) override {}
  void handle_slot_indication(slot_point sl_tx) override
  {
    if (events.next_ul_sched_res.has_value()) {
      result_notifier.get_cell(to_du_cell_index(0)).on_new_uplink_scheduler_results(events.next_ul_sched_res.value());
    }
    result_notifier.get_cell(to_du_cell_index(0)).on_cell_results_completion(sl_tx);
  }
  void                               handle_error_indication(slot_point sl_tx, error_event event) override {}
  void                               add_cell(const mac_cell_creation_request& cell_cfg) override {}
  void                               remove_cell(du_cell_index_t cell_index) override {}
  mac_cell_controller&               get_cell_controller(du_cell_index_t cell_index) override { return *this; }
  async_task<mac_ue_create_response> handle_ue_create_request(const mac_ue_create_request& cfg) override
  {
    events.last_ue_created = cfg;
    return launch_no_op_task(mac_ue_create_response{});
  }
  async_task<mac_ue_reconfiguration_response>
  handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& cfg) override
  {
    return launch_no_op_task(mac_ue_reconfiguration_response{});
  }
  async_task<mac_ue_delete_response> handle_ue_delete_request(const mac_ue_delete_request& cfg) override
  {
    return launch_no_op_task(mac_ue_delete_response{});
  }
  bool             handle_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu) override { return true; }
  void             handle_rx_data_indication(mac_rx_data_indication pdu) override {}
  void             handle_paging_information(const paging_information& msg) override {}
  async_task<void> start() override { return launch_no_op_task(); }
  async_task<void> stop() override { return launch_no_op_task(); }
};

struct test_params {
  unsigned                               nof_ports;
  srs_du::du_test_config::test_ue_config test_ue_cfg;
};

/// Formatter for test params.
void PrintTo(const test_params& value, ::std::ostream* os)
{
  *os << fmt::format("ports={} cqi={} ri={}", value.nof_ports, value.test_ue_cfg.cqi, value.test_ue_cfg.ri);
  if (value.nof_ports == 2) {
    *os << fmt::format(" pmi={}", value.test_ue_cfg.pmi);
  } else if (value.nof_ports == 4) {
    *os << fmt::format(
        " i1_1={} i1_3={} i2={}", value.test_ue_cfg.i_1_1, value.test_ue_cfg.i_1_3, value.test_ue_cfg.i_2);
  }
}

class mac_test_mode_adapter_test : public ::testing::TestWithParam<test_params>
{
protected:
  mac_test_mode_adapter_test() : params(GetParam()), adapter{params.test_ue_cfg, phy}
  {
    adapter.connect(std::make_unique<mac_dummy>(mac_events, adapter.get_phy_notifier()));

    // create cell
    cell_config_builder_params builder{};
    builder.nof_dl_ports = params.nof_ports;
    adapter.add_cell(test_helpers::make_default_mac_cell_config(builder));

    // create UE
    mac_ue_create_request req      = test_helpers::make_default_ue_creation_request(builder);
    req.sched_cfg.cells.value()[0] = config_helpers::create_default_initial_ue_spcell_cell_config(builder);
    adapter.get_ue_configurator().handle_ue_create_request(req);
    srsran_assert(mac_events.last_ue_created.has_value(), "UE creation request was not forwarded to MAC");

    csi_cfg = create_csi_report_configuration(
        *mac_events.last_ue_created->sched_cfg.cells.value()[0].serv_cell_cfg.csi_meas_cfg);
  }

  void run_slot()
  {
    adapter.get_slot_handler(to_du_cell_index(0)).handle_slot_indication(next_slot);

    next_slot++;

    mac_events.next_ul_sched_res.reset();
  }

  test_params           params;
  mac_event_interceptor mac_events;
  phy_dummy             phy;
  mac_test_mode_adapter adapter;

  csi_report_configuration csi_cfg;

  slot_point next_slot{0, 0};
};

mac_uci_pdu make_random_uci_with_csi(rnti_t test_rnti = to_rnti(0x4601))
{
  mac_uci_pdu pdu;
  pdu.rnti = test_rnti;

  mac_uci_pdu::pucch_f2_or_f3_or_f4_type f2;
  f2.csi_part1_info.emplace();
  f2.csi_part1_info->is_valid = false;
  f2.csi_part1_info->payload.resize(test_rgen::uniform_int<unsigned>(0, 11));
  for (unsigned i = 0; i != f2.csi_part1_info->payload.size(); ++i) {
    f2.csi_part1_info->payload.set(i, test_rgen::uniform_int<unsigned>(0, 1));
  }

  pdu.pdu = f2;
  return pdu;
}

TEST_P(mac_test_mode_adapter_test, when_uci_is_only_for_test_mode_ue_then_it_is_ignored)
{
  mac_uci_indication_message uci_ind;
  uci_ind.sl_rx = {0, 0};
  uci_ind.ucis.push_back(make_random_uci_with_csi());
  adapter.get_control_info_handler(to_du_cell_index(0)).handle_uci(uci_ind);

  ASSERT_FALSE(mac_events.last_uci.has_value());
}

TEST_P(mac_test_mode_adapter_test, when_uci_is_also_for_other_ues_then_test_mode_ue_is_cropped)
{
  mac_uci_indication_message uci_ind;
  uci_ind.sl_rx = {0, 0};
  uci_ind.ucis.push_back(make_random_uci_with_csi());
  uci_ind.ucis.push_back(make_random_uci_with_csi());
  uci_ind.ucis.back().rnti = to_rnti(0x4602);
  adapter.get_control_info_handler(to_du_cell_index(0)).handle_uci(uci_ind);

  ASSERT_TRUE(mac_events.last_uci.has_value());
  ASSERT_EQ(mac_events.last_uci->ucis.size(), 1);
  ASSERT_EQ(mac_events.last_uci->ucis[0].rnti, to_rnti(0x4602));
}

TEST_P(mac_test_mode_adapter_test, when_test_mode_ue_has_pucch_grants_then_uci_indications_are_auto_forwarded_to_mac)
{
  // PUCCH got scheduled for test mode UE.
  ul_sched_result ul_res{};
  pucch_info&     pucch            = ul_res.pucchs.emplace_back();
  pucch.crnti                      = this->params.test_ue_cfg.rnti;
  pucch.format                     = srsran::pucch_format::FORMAT_1;
  pucch.format_1.harq_ack_nof_bits = 1;
  pucch.format_1.sr_bits           = sr_nof_bits::no_sr;
  mac_events.next_ul_sched_res.emplace();
  mac_events.next_ul_sched_res->slot   = next_slot;
  mac_events.next_ul_sched_res->ul_res = &ul_res;

  // Run the slot with PUCCH scheduled.
  slot_point sl_rx = next_slot;
  this->run_slot();

  // After a few slots, the UCI indication is forwarded to the real MAC.
  const unsigned max_slots = 20;
  for (unsigned slot_count = 0; slot_count != max_slots; ++slot_count) {
    this->run_slot();
    if (mac_events.last_uci.has_value()) {
      break;
    }
  }
  ASSERT_TRUE(mac_events.last_uci.has_value());
  ASSERT_EQ(mac_events.last_uci->sl_rx, sl_rx);
  ASSERT_EQ(mac_events.last_uci->ucis.size(), 1);
  ASSERT_EQ(mac_events.last_uci->ucis[0].rnti, this->params.test_ue_cfg.rnti);
  ASSERT_TRUE(variant_holds_alternative<mac_uci_pdu::pucch_f0_or_f1_type>(mac_events.last_uci->ucis[0].pdu));
  const auto& f1 = variant_get<mac_uci_pdu::pucch_f0_or_f1_type>(mac_events.last_uci->ucis[0].pdu);
  ASSERT_FALSE(f1.sr_info.has_value());
  ASSERT_TRUE(f1.harq_info.has_value());
  ASSERT_EQ(f1.harq_info->harqs.size(), 1);
  ASSERT_EQ(f1.harq_info->harqs[0], uci_pucch_f0_or_f1_harq_values::ack);
  ASSERT_TRUE(f1.ul_sinr.value() > 0);
}

TEST_P(mac_test_mode_adapter_test, when_test_mode_ue_has_pusch_grants_then_crc_indications_are_auto_forwarded_to_mac)
{
  // PUCCH got scheduled for test mode UE.
  ul_sched_result ul_res{};
  ul_sched_info&  ulgrant   = ul_res.puschs.emplace_back();
  ulgrant.pusch_cfg.rnti    = this->params.test_ue_cfg.rnti;
  ulgrant.pusch_cfg.harq_id = to_harq_id(test_rgen::uniform_int(0, 15));
  mac_events.next_ul_sched_res.emplace();
  mac_events.next_ul_sched_res->slot   = next_slot;
  mac_events.next_ul_sched_res->ul_res = &ul_res;

  // Run the slot with PUSCH scheduled.
  slot_point sl_rx = next_slot;
  this->run_slot();

  // After a few slots, the CRC indication is forwarded to the real MAC.
  const unsigned max_slots = 20;
  for (unsigned slot_count = 0; slot_count != max_slots; ++slot_count) {
    this->run_slot();
    if (mac_events.last_crc.has_value()) {
      break;
    }
  }
  ASSERT_TRUE(mac_events.last_crc.has_value());
  ASSERT_EQ(mac_events.last_crc->sl_rx, sl_rx);
  ASSERT_EQ(mac_events.last_crc->crcs.size(), 1);
  ASSERT_EQ(mac_events.last_crc->crcs[0].rnti, this->params.test_ue_cfg.rnti);
  ASSERT_EQ(mac_events.last_crc->crcs[0].harq_id, ulgrant.pusch_cfg.harq_id);
  ASSERT_TRUE(mac_events.last_crc->crcs[0].tb_crc_success);
  ASSERT_TRUE(mac_events.last_crc->crcs[0].ul_sinr_metric.value() > 0);
}

TEST_P(mac_test_mode_adapter_test, when_uci_is_forwarded_to_mac_then_test_mode_csi_params_are_enforced)
{
  // PUCCH got scheduled for test mode UE.
  ul_sched_result ul_res{};
  pucch_info&     pucch            = ul_res.pucchs.emplace_back();
  pucch.crnti                      = this->params.test_ue_cfg.rnti;
  pucch.format                     = srsran::pucch_format::FORMAT_2;
  pucch.format_2.harq_ack_nof_bits = 1;
  pucch.format_2.sr_bits           = sr_nof_bits::one;
  pucch.format_2.csi_part1_bits    = 11;
  pucch.csi_rep_cfg                = this->csi_cfg;
  pucch.format_1.harq_ack_nof_bits = 1;
  pucch.format_1.sr_bits           = sr_nof_bits::no_sr;
  mac_events.next_ul_sched_res.emplace();
  mac_events.next_ul_sched_res->slot   = next_slot;
  mac_events.next_ul_sched_res->ul_res = &ul_res;

  // Run the slot with PUCCH scheduled.
  slot_point sl_rx = next_slot;
  this->run_slot();

  // After a few slots, the UCI indication is forwarded to the real MAC.
  const unsigned max_slots = 20;
  for (unsigned slot_count = 0; slot_count != max_slots; ++slot_count) {
    this->run_slot();
    if (mac_events.last_uci.has_value()) {
      break;
    }
  }
  ASSERT_TRUE(mac_events.last_uci.has_value());
  ASSERT_EQ(mac_events.last_uci->sl_rx, sl_rx);
  ASSERT_EQ(mac_events.last_uci->ucis.size(), 1);
  ASSERT_EQ(mac_events.last_uci->ucis[0].rnti, this->params.test_ue_cfg.rnti);
  ASSERT_TRUE(variant_holds_alternative<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(mac_events.last_uci->ucis[0].pdu));
  const auto& f2 = variant_get<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(mac_events.last_uci->ucis[0].pdu);
  // check SR info.
  ASSERT_TRUE(f2.sr_info.has_value());
  ASSERT_EQ(f2.sr_info->size(), 1);
  ASSERT_FALSE(f2.sr_info->test(0)); // SR not detected.
  ASSERT_TRUE(f2.ul_sinr.value() > 0);
  // check HARQ info.
  ASSERT_TRUE(f2.harq_info.has_value());
  ASSERT_TRUE(f2.harq_info->is_valid);
  ASSERT_EQ(f2.harq_info->payload.size(), 1);
  ASSERT_TRUE(f2.harq_info->payload.test(0));
  // check CSI info.
  ASSERT_TRUE(f2.csi_part1_info.has_value());
  ASSERT_TRUE(f2.csi_part1_info->is_valid);
  // Check that the payload size is the same as the expected, given the UE config.
  units::bits expected_payload_size = get_csi_report_pucch_size(this->csi_cfg);
  ASSERT_EQ(f2.csi_part1_info->payload.size(), expected_payload_size.value());
  // Decode the CSI report and check that the CQI is the same as the one in the test config.
  csi_report_packed csi_bits(expected_payload_size.value());
  for (unsigned i = 0; i != csi_bits.size(); ++i) {
    csi_bits.set(i, f2.csi_part1_info->payload.test(i));
  }
  csi_report_data report = csi_report_unpack_pucch(csi_bits, this->csi_cfg);
  ASSERT_EQ(*report.first_tb_wideband_cqi, params.test_ue_cfg.cqi);
  if (params.nof_ports == 1) {
    ASSERT_FALSE(report.pmi.has_value());
  } else {
    ASSERT_EQ(*report.ri, params.test_ue_cfg.ri);
    if (params.nof_ports == 2) {
      ASSERT_TRUE(variant_holds_alternative<csi_report_pmi::two_antenna_port>(report.pmi->type));
      ASSERT_EQ(variant_get<csi_report_pmi::two_antenna_port>(report.pmi->type).pmi, params.test_ue_cfg.pmi);
    } else {
      ASSERT_TRUE(variant_holds_alternative<csi_report_pmi::typeI_single_panel_4ports_mode1>(report.pmi->type));
      auto& t = variant_get<csi_report_pmi::typeI_single_panel_4ports_mode1>(report.pmi->type);
      ASSERT_EQ(t.i_1_1, params.test_ue_cfg.i_1_1);
      if (t.i_1_3.has_value()) {
        ASSERT_EQ(*t.i_1_3, params.test_ue_cfg.i_1_3);
      }
      ASSERT_EQ(t.i_2, params.test_ue_cfg.i_2);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(test_configs,
                         mac_test_mode_adapter_test,
                         // clang-format off
::testing::Values(
//           ports rnti           nof_ues            CQI RI PMI i1_1 i1_3  i2
  test_params{1, {to_rnti(0x4601), 1, 8, true, true, 12}},
  test_params{1, {to_rnti(0x4601), 1, 8, true, true, 5}},
  test_params{2, {to_rnti(0x4601), 1, 8, true, true, 12,  2,  1}},
  test_params{2, {to_rnti(0x4601), 1, 8, true, true, 3,   1,  3}},
  test_params{4, {to_rnti(0x4601), 1, 8, true, true, 12,  4,  0,   2,   0,  1}},
  test_params{4, {to_rnti(0x4601), 1, 8, true, true, 12,  1,  0,   1,   0,  3}},
  test_params{4, {to_rnti(0x4601), 1, 8, true, true, 12,  2,  0,   7,   1,  0}}
));
// clang-format on
