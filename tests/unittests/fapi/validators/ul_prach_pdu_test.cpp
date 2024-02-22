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

#include "../../../lib/fapi/validators/ul_prach_pdu.h"
#include "../message_builder_helpers.h"
#include "helpers.h"
#include "srsran/fapi/message_validators.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_prach_pdu_field : public validate_fapi_pdu<ul_prach_pdu, ul_pdu_type>,
                                 public testing::TestWithParam<std::tuple<pdu_field_data<ul_prach_pdu>, test_case_data>>
{};

TEST_P(validate_prach_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_ul_prach_pdu,
               validate_ul_prach_pdu,
               srsran::fapi::message_type_id::ul_tti_request,
               ul_pdu_type::PRACH);
};

INSTANTIATE_TEST_SUITE_P(pci,
                         validate_prach_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_prach_pdu>{
                                              "Physical cell ID",
                                              [](ul_prach_pdu& pdu, int value) { pdu.phys_cell_id = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1007, true},
                                                          test_case_data{100, true},
                                                          test_case_data{1008, false})));

INSTANTIATE_TEST_SUITE_P(prach_occasions,
                         validate_prach_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_prach_pdu>{
                                              "Number of PRACH occasions",
                                              [](ul_prach_pdu& pdu, int value) { pdu.num_prach_ocas = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{7, true},
                                                          test_case_data{8, false})));

INSTANTIATE_TEST_SUITE_P(prach_format,
                         validate_prach_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_prach_pdu>{
                                              "PRACH format",
                                              [](ul_prach_pdu& pdu, int value) {
                                                pdu.prach_format = static_cast<prach_format_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{7, true},
                                                          test_case_data{13, true},
                                                          test_case_data{14, false})));

INSTANTIATE_TEST_SUITE_P(fd_ra_idx,
                         validate_prach_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_prach_pdu>{
                                              "Frequency domain occasion index",
                                              [](ul_prach_pdu& pdu, int value) { pdu.index_fd_ra = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{3, true},
                                                          test_case_data{7, true},
                                                          test_case_data{8, false})));

INSTANTIATE_TEST_SUITE_P(prach_start_symb,
                         validate_prach_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_prach_pdu>{
                                              "PRACH start symbol",
                                              [](ul_prach_pdu& pdu, int value) { pdu.prach_start_symbol = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{7, true},
                                                          test_case_data{13, true},
                                                          test_case_data{14, false})));

INSTANTIATE_TEST_SUITE_P(prach_zero_correlation_zone,
                         validate_prach_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_prach_pdu>{
                                              "Zero-correlation zone configuration number",
                                              [](ul_prach_pdu& pdu, int value) { pdu.num_cs = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{200, true},
                                                          test_case_data{419, true},
                                                          test_case_data{420, false})));

INSTANTIATE_TEST_SUITE_P(is_msg_a_prach,
                         validate_prach_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_prach_pdu>{
                                              "Is msgA-PRACH",
                                              [](ul_prach_pdu& pdu, int value) { pdu.is_msg_a_prach = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(
    prach_config_scope,
    validate_prach_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_prach_pdu>{"PRACH config scope",
                                                                  [](ul_prach_pdu& pdu, int value) {
                                                                    pdu.maintenance_v3.prach_config_scope =
                                                                        static_cast<prach_config_scope_type>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(num_fd_ra,
                         validate_prach_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_prach_pdu>{
                                              "Number of frequency domain occasions",
                                              [](ul_prach_pdu& pdu, int value) {
                                                pdu.maintenance_v3.num_fd_ra = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{4, true},
                                                          test_case_data{8, true},
                                                          test_case_data{9, false})));

INSTANTIATE_TEST_SUITE_P(preamble_index,
                         validate_prach_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_prach_pdu>{
                                              "Start preamble index",
                                              [](ul_prach_pdu& pdu, int value) {
                                                pdu.maintenance_v3.start_preamble_index = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{32, true},
                                                          test_case_data{63, true},
                                                          test_case_data{64, false},
                                                          test_case_data{128, false},
                                                          test_case_data{254, false},
                                                          test_case_data{255, true})));

INSTANTIATE_TEST_SUITE_P(num_preamble_indices,
                         validate_prach_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_prach_pdu>{
                                              "Number of preambles logical indices",
                                              [](ul_prach_pdu& pdu, int value) {
                                                pdu.maintenance_v3.num_preamble_indices = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{4, true},
                                                          test_case_data{32, true},
                                                          test_case_data{64, true},
                                                          test_case_data{65, false})));

TEST(validate_ul_prach_pdu, valid_pdu_passes)
{
  validator_report report(0, 0);
  const auto&      pdu = build_valid_ul_prach_pdu();
  ASSERT_TRUE(validate_ul_prach_pdu(pdu, report));
  ASSERT_TRUE(report.reports.empty());
}

TEST(validate_ul_prach_pdu, invalid_pdu_fails)
{
  validator_report report(0, 0);
  auto             pdu = build_valid_ul_prach_pdu();

  // Add 6 errors.
  pdu.maintenance_v3.num_preamble_indices = 65;
  pdu.maintenance_v3.start_preamble_index = 254;
  pdu.maintenance_v3.num_fd_ra            = 9;
  pdu.num_cs                              = 500;
  pdu.index_fd_ra                         = 18;
  pdu.num_prach_ocas                      = 10;

  ASSERT_FALSE(validate_ul_prach_pdu(pdu, report));
  ASSERT_EQ(4U, report.reports.size());
  ASSERT_EQ(6U, report.nof_errors);
  // Check that the properties that caused the error are different.
  for (unsigned i = 0, e = report.reports.size(); i != e; ++i) {
    for (unsigned j = i + 1; j != e; ++j) {
      ASSERT_TRUE(std::strcmp(report.reports[i].property_name, report.reports[j].property_name) != 0);
    }
  }
}
