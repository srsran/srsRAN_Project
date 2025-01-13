/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../../../lib/fapi/validators/ul_srs_pdu.h"
#include "../message_builder_helpers.h"
#include "helpers.h"
#include "srsran/fapi/message_validators.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_srs_pdu_field : public validate_fapi_pdu<ul_srs_pdu, ul_pdu_type>,
                               public testing::TestWithParam<std::tuple<pdu_field_data<ul_srs_pdu>, test_case_data>>
{};

TEST_P(validate_srs_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_ul_srs_pdu,
               validate_ul_srs_pdu,
               srsran::fapi::message_type_id::ul_tti_request,
               ul_pdu_type::SRS);
}

INSTANTIATE_TEST_SUITE_P(RNTI,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "RNTI",
                                              [](ul_srs_pdu& pdu, int value) { pdu.rnti = to_rnti(value); }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32768, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(BWP_Size,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "BWP size",
                                              [](ul_srs_pdu& pdu, int value) { pdu.bwp_size = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{128, true},
                                                          test_case_data{275, true},
                                                          test_case_data{276, false})));

INSTANTIATE_TEST_SUITE_P(BWP_Start,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "BWP start",
                                              [](ul_srs_pdu& pdu, int value) { pdu.bwp_start = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(Subcarrier_spacing,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{"Subcarrier spacing",
                                                                                     [](ul_srs_pdu& pdu, int value) {
                                                                                       pdu.scs =
                                                                                           to_subcarrier_spacing(value);
                                                                                     }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(
    Cyclic_prefix,
    validate_srs_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                         "Cyclic prefix",
                         [](ul_srs_pdu& pdu, int value) { pdu.cp = static_cast<cyclic_prefix::options>(value); }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(nof_antenna_port,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Number of antenna ports",
                                              [](ul_srs_pdu& pdu, int value) { pdu.num_ant_ports = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(nof_symbols,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Number of symbols",
                                              [](ul_srs_pdu& pdu, int value) { pdu.num_symbols = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(nof_repetitions,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{"Number of repetitions",
                                                                                     [](ul_srs_pdu& pdu, int value) {
                                                                                       pdu.num_repetitions =
                                                                                           static_cast<srs_nof_symbols>(
                                                                                               value);
                                                                                     }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(time_start_position,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Time start position",
                                              [](ul_srs_pdu& pdu, int value) { pdu.time_start_position = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{6, true},
                                                          test_case_data{13, true},
                                                          test_case_data{14, false},
                                                          test_case_data{15, false})));

INSTANTIATE_TEST_SUITE_P(config_index,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Config index",
                                              [](ul_srs_pdu& pdu, int value) { pdu.config_index = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{32, true},
                                                          test_case_data{63, true},
                                                          test_case_data{64, false},
                                                          test_case_data{65, false})));

INSTANTIATE_TEST_SUITE_P(sequence_id,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Sequence identifier",
                                              [](ul_srs_pdu& pdu, int value) { pdu.sequence_id = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false},
                                                          test_case_data{1025, false})));

INSTANTIATE_TEST_SUITE_P(bandwidth_index,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Bandwidth index",
                                              [](ul_srs_pdu& pdu, int value) { pdu.bandwidth_index = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(comb_size,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{"Comb size",
                                                                                     [](ul_srs_pdu& pdu, int value) {
                                                                                       pdu.comb_size =
                                                                                           static_cast<tx_comb_size>(
                                                                                               value);
                                                                                     }}),
                                          testing::Values(test_case_data{2, true}, test_case_data{4, true})));

INSTANTIATE_TEST_SUITE_P(comb_offset_n2,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{"Comb offset",
                                                                                     [](ul_srs_pdu& pdu, int value) {
                                                                                       pdu.comb_size = tx_comb_size::n2;
                                                                                       pdu.comb_offset = value;
                                                                                     }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, false},
                                                          test_case_data{3, false},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(comb_offset_n4,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{"Comb offset",
                                                                                     [](ul_srs_pdu& pdu, int value) {
                                                                                       pdu.comb_size = tx_comb_size::n4;
                                                                                       pdu.comb_offset = value;
                                                                                     }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(cyclic_shift_n2,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{"Cyclic shift",
                                                                                     [](ul_srs_pdu& pdu, int value) {
                                                                                       pdu.comb_size = tx_comb_size::n2;
                                                                                       pdu.cyclic_shift = value;
                                                                                     }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{3, true},
                                                          test_case_data{7, true},
                                                          test_case_data{8, false},
                                                          test_case_data{9, false})));

INSTANTIATE_TEST_SUITE_P(cyclic_shift_n4,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{"Cyclic shift",
                                                                                     [](ul_srs_pdu& pdu, int value) {
                                                                                       pdu.comb_size = tx_comb_size::n4;
                                                                                       pdu.cyclic_shift = value;
                                                                                     }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{5, true},
                                                          test_case_data{11, true},
                                                          test_case_data{12, false},
                                                          test_case_data{15, false})));

INSTANTIATE_TEST_SUITE_P(frequency_position,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Frequency position",
                                              [](ul_srs_pdu& pdu, int value) { pdu.frequency_position = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{32, true},
                                                          test_case_data{67, true},
                                                          test_case_data{68, false},
                                                          test_case_data{105, false})));

INSTANTIATE_TEST_SUITE_P(frequency_shift,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Frequency shift",
                                              [](ul_srs_pdu& pdu, int value) { pdu.frequency_shift = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{268, true},
                                                          test_case_data{269, false},
                                                          test_case_data{1005, false})));

INSTANTIATE_TEST_SUITE_P(frequency_hopping,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Frequency hopping",
                                              [](ul_srs_pdu& pdu, int value) { pdu.frequency_hopping = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(group_or_sequence_hopping,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Group or sequence hopping",
                                              [](ul_srs_pdu& pdu, int value) {
                                                pdu.group_or_sequence_hopping =
                                                    static_cast<srs_group_or_sequence_hopping>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(resource_type,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Resource type",
                                              [](ul_srs_pdu& pdu, int value) {
                                                pdu.resource_type = static_cast<srs_resource_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(t_srs,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{"T-SRS",
                                                                                     [](ul_srs_pdu& pdu, int value) {
                                                                                       pdu.t_srs =
                                                                                           static_cast<srs_periodicity>(
                                                                                               value);
                                                                                     }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, true},
                                                          test_case_data{6, false},
                                                          test_case_data{7, false},
                                                          test_case_data{8, true},
                                                          test_case_data{9, false},
                                                          test_case_data{10, true},
                                                          test_case_data{11, false},
                                                          test_case_data{15, false},
                                                          test_case_data{16, true},
                                                          test_case_data{17, false},
                                                          test_case_data{19, false},
                                                          test_case_data{20, true},
                                                          test_case_data{21, false},
                                                          test_case_data{31, false},
                                                          test_case_data{32, true},
                                                          test_case_data{33, false},
                                                          test_case_data{39, false},
                                                          test_case_data{40, true},
                                                          test_case_data{41, false},
                                                          test_case_data{63, false},
                                                          test_case_data{64, true},
                                                          test_case_data{65, false},
                                                          test_case_data{79, false},
                                                          test_case_data{80, true},
                                                          test_case_data{81, false},
                                                          test_case_data{159, false},
                                                          test_case_data{160, true},
                                                          test_case_data{161, false},
                                                          test_case_data{319, false},
                                                          test_case_data{320, true},
                                                          test_case_data{321, false},
                                                          test_case_data{639, false},
                                                          test_case_data{640, true},
                                                          test_case_data{641, false},
                                                          test_case_data{1279, false},
                                                          test_case_data{1280, true},
                                                          test_case_data{1281, false},
                                                          test_case_data{2559, false},
                                                          test_case_data{2560, true},
                                                          test_case_data{2561, false})));

INSTANTIATE_TEST_SUITE_P(t_offset,
                         validate_srs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_srs_pdu>{
                                              "Slot offset value",
                                              [](ul_srs_pdu& pdu, int value) { pdu.t_offset = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1200, true},
                                                          test_case_data{2559, true},
                                                          test_case_data{2560, false},
                                                          test_case_data{3000, false})));

/// Valid PDU should pass.
TEST(validate_srs_pdu, valid_pdu_passes)
{
  validator_report report(0, 0);
  const auto&      pdu = build_valid_ul_srs_pdu();
  EXPECT_TRUE(validate_ul_srs_pdu(pdu, report));
  // Assert no reports were generated.
  EXPECT_TRUE(report.reports.empty());
}

/// Add 4 errors and check that validation fails with 3 errors.
TEST(validate_srs_pdu, invalid_pdu_fails)
{
  validator_report report(0, 0);
  auto             pdu = build_valid_ul_srs_pdu();

  // Add 4 errors.
  pdu.bwp_start       = 6500;
  pdu.bwp_size        = 2133;
  pdu.config_index    = 132;
  pdu.bandwidth_index = 8;

  EXPECT_FALSE(validate_ul_srs_pdu(pdu, report));
  ASSERT_EQ(4U, report.reports.size());
  // Check that the properties that caused the error are different.
  for (unsigned i = 0, e = report.reports.size(); i != e; ++i) {
    for (unsigned j = i + 1; j != e; ++j) {
      EXPECT_TRUE(std::strcmp(report.reports[i].property_name, report.reports[j].property_name) != 0);
    }
  }
}
