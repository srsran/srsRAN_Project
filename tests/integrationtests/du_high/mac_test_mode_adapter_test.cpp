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

#include "lib/du_high/mac_test_mode_adapter.h"
#include "tests/unittests/mac/mac_test_helpers.h"
#include "srsran/ran/csi_report/csi_report_config_helpers.h"
#include "srsran/ran/csi_report/csi_report_on_pucch_helpers.h"
#include "srsran/support/async/async_test_utils.h"
#include "srsran/support/test_utils.h"
#include <gtest/gtest.h>

using namespace srsran;

struct mac_event_interceptor {
  optional<mac_ue_create_request> last_ue_created;

  optional<mac_uci_indication_message> last_uci;
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

  mac_dummy(mac_event_interceptor& interceptor_) : events(interceptor_) {}

  mac_cell_rach_handler&                get_rach_handler(du_cell_index_t cell_index) override { return *this; }
  mac_cell_control_information_handler& get_control_info_handler(du_cell_index_t cell_index) override { return *this; }
  mac_ue_control_information_handler&   get_ue_control_info_handler() override { return *this; }
  mac_cell_slot_handler&                get_slot_handler(du_cell_index_t cell_index) override { return *this; }
  mac_cell_manager&                     get_cell_manager() override { return *this; }
  mac_ue_configurator&                  get_ue_configurator() override { return *this; }
  mac_pdu_handler&                      get_pdu_handler() override { return *this; }
  mac_paging_information_handler&       get_cell_paging_info_handler() override { return *this; }

  void                 handle_rach_indication(const mac_rach_indication& rach_ind) override {}
  void                 handle_crc(const mac_crc_indication_message& msg) override {}
  void                 handle_uci(const mac_uci_indication_message& msg) override { events.last_uci = msg; }
  void                 handle_dl_buffer_state_update(const mac_dl_buffer_state_indication_message& dl_bs) override {}
  void                 handle_slot_indication(slot_point sl_tx) override {}
  void                 add_cell(const mac_cell_creation_request& cell_cfg) override {}
  void                 remove_cell(du_cell_index_t cell_index) override {}
  mac_cell_controller& get_cell_controller(du_cell_index_t cell_index) override { return *this; }
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
  mac_test_mode_adapter_test() : params(GetParam()), adapter{std::make_unique<mac_dummy>(rx_events), params.test_ue_cfg}
  {
    mac_ue_create_request      req = test_helpers::make_default_ue_creation_request();
    cell_config_builder_params builder{};
    builder.nof_dl_ports           = params.nof_ports;
    req.sched_cfg.cells.value()[0] = config_helpers::create_default_initial_ue_spcell_cell_config(builder);
    adapter.get_ue_configurator().handle_ue_create_request(req);
    srsran_assert(rx_events.last_ue_created.has_value(), "UE creation request was not forwarded to MAC");

    csi_cfg = create_csi_report_configuration(
        *rx_events.last_ue_created->sched_cfg.cells.value()[0].serv_cell_cfg.csi_meas_cfg);
  }

  test_params           params;
  mac_event_interceptor rx_events;
  mac_test_mode_adapter adapter;

  csi_report_configuration csi_cfg;
};

mac_uci_pdu make_random_uci_with_csi(rnti_t test_rnti = to_rnti(0x4601))
{
  mac_uci_pdu pdu;
  pdu.rnti = test_rnti;

  mac_uci_pdu::pucch_f2_or_f3_or_f4_type f2;
  f2.uci_part1_or_csi_part1_info.emplace();
  f2.uci_part1_or_csi_part1_info->status = uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure;
  f2.uci_part1_or_csi_part1_info->payload_type =
      mac_uci_pdu::pucch_f2_or_f3_or_f4_type::uci_payload_or_csi_information::payload_type_t::csi_part_payload;
  f2.uci_part1_or_csi_part1_info->payload.resize(test_rgen::uniform_int<unsigned>(0, 11));
  for (unsigned i = 0; i != f2.uci_part1_or_csi_part1_info->payload.size(); ++i) {
    f2.uci_part1_or_csi_part1_info->payload.set(i, test_rgen::uniform_int<unsigned>(0, 1));
  }

  pdu.pdu = f2;
  return pdu;
}

TEST_P(mac_test_mode_adapter_test, test_cfg_f2_csi_is_enforced_in_uci)
{
  mac_uci_indication_message uci_ind;
  uci_ind.sl_rx = {0, 0};
  uci_ind.ucis.push_back(make_random_uci_with_csi());
  adapter.get_control_info_handler(to_du_cell_index(0)).handle_uci(uci_ind);

  // Ensure the UCI reached the MAC with CRC success.
  ASSERT_TRUE(rx_events.last_uci.has_value());
  ASSERT_EQ(rx_events.last_uci->ucis.size(), 1);
  ASSERT_EQ(rx_events.last_uci->ucis[0].rnti, params.test_ue_cfg.rnti);
  ASSERT_TRUE(variant_holds_alternative<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(rx_events.last_uci->ucis[0].pdu));
  const mac_uci_pdu::pucch_f2_or_f3_or_f4_type& f2 =
      variant_get<mac_uci_pdu::pucch_f2_or_f3_or_f4_type>(rx_events.last_uci->ucis[0].pdu);
  ASSERT_TRUE(f2.uci_part1_or_csi_part1_info.has_value());
  ASSERT_EQ(f2.uci_part1_or_csi_part1_info->payload_type,
            mac_uci_pdu::pucch_f2_or_f3_or_f4_type::uci_payload_or_csi_information::payload_type_t::csi_part_payload);
  ASSERT_EQ(f2.uci_part1_or_csi_part1_info->status, uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass);

  // Check that the payload size is the same as the expected, given the UE config.
  units::bits expected_payload_size = get_csi_report_pucch_size(this->csi_cfg);
  ASSERT_EQ(f2.uci_part1_or_csi_part1_info->payload.size(), expected_payload_size.value());

  // Decode the CSI report and check that the CQI is the same as the one in the test config.
  csi_report_packed csi_bits(expected_payload_size.value());
  for (unsigned i = 0; i != csi_bits.size(); ++i) {
    csi_bits.set(i, f2.uci_part1_or_csi_part1_info->payload.test(i));
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
//           ports                             CQI RI PMI i1_1 i1_3  i2
  test_params{1, {to_rnti(0x4601), true, true, 12}},
  test_params{1, {to_rnti(0x4601), true, true, 5}},
  test_params{2, {to_rnti(0x4601), true, true, 12,  2,  1}},
  test_params{2, {to_rnti(0x4601), true, true, 3,   1,  3}},
  test_params{4, {to_rnti(0x4601), true, true, 12,  4,  0,   2,   0,  1}},
  test_params{4, {to_rnti(0x4601), true, true, 12,  1,  0,   1,   0,  3}},
  test_params{4, {to_rnti(0x4601), true, true, 12,  2,  0,   7,   1,  0}}
));
// clang-format on
