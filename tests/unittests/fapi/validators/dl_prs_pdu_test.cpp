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

#include "dl_prs_pdu.h"
#include "helpers.h"
#include "message_builder_helpers.h"
#include "srsran/fapi/message_validators.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_prs_pdu_field : public validate_fapi_pdu<dl_prs_pdu, dl_pdu_type>,
                               public testing::TestWithParam<std::tuple<pdu_field_data<dl_prs_pdu>, test_case_data>>
{};

TEST_P(validate_prs_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_dl_prs_pdu,
               validate_dl_prs_pdu,
               srsran::fapi::message_type_id::dl_tti_request,
               dl_pdu_type::PRS);
}

INSTANTIATE_TEST_SUITE_P(Subcarrier_spacing,
                         validate_prs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{"Subcarrier spacing",
                                                                                     [](dl_prs_pdu& pdu, int value) {
                                                                                       pdu.scs =
                                                                                           to_subcarrier_spacing(value);
                                                                                     }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(
    Cyclic_prefix,
    validate_prs_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{
                         "Cyclic prefix",
                         [](dl_prs_pdu& pdu, int value) { pdu.cp = static_cast<cyclic_prefix::options>(value); }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(n_id,
                         validate_prs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{
                                              "n ID PRS",
                                              [](dl_prs_pdu& pdu, int value) { pdu.nid_prs = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2048, true},
                                                          test_case_data{4095, true},
                                                          test_case_data{4096, false})));

INSTANTIATE_TEST_SUITE_P(comb_size,
                         validate_prs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{"Comb size",
                                                                                     [](dl_prs_pdu& pdu, int value) {
                                                                                       pdu.comb_size =
                                                                                           static_cast<prs_comb_size>(
                                                                                               value);
                                                                                     }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, false},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false},
                                                          test_case_data{7, false},
                                                          test_case_data{6, true},
                                                          test_case_data{9, false},
                                                          test_case_data{11, false},
                                                          test_case_data{12, true},
                                                          test_case_data{13, false})));

INSTANTIATE_TEST_SUITE_P(
    comb_offset_size_2,
    validate_prs_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{"Comb offset",
                                                                [](dl_prs_pdu& pdu, int value) {
                                                                  pdu.comb_size   = prs_comb_size::two;
                                                                  pdu.comb_offset = value;
                                                                }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(comb_offset_size_4,
                         validate_prs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{"Comb offset",
                                                                                     [](dl_prs_pdu& pdu, int value) {
                                                                                       pdu.comb_size =
                                                                                           prs_comb_size::four;
                                                                                       pdu.comb_offset = value;
                                                                                     }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(comb_offset_size_6,
                         validate_prs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{"Comb offset",
                                                                                     [](dl_prs_pdu& pdu, int value) {
                                                                                       pdu.comb_size =
                                                                                           prs_comb_size::six;
                                                                                       pdu.comb_offset = value;
                                                                                     }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{3, true},
                                                          test_case_data{5, true},
                                                          test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(comb_offset_size_12,
                         validate_prs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{"Comb offset",
                                                                                     [](dl_prs_pdu& pdu, int value) {
                                                                                       pdu.comb_size =
                                                                                           prs_comb_size::twelve;
                                                                                       pdu.comb_offset = value;
                                                                                     }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{6, true},
                                                          test_case_data{11, true},
                                                          test_case_data{12, false})));

INSTANTIATE_TEST_SUITE_P(num_symbols,
                         validate_prs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{"Number of symbols",
                                                                                     [](dl_prs_pdu& pdu, int value) {
                                                                                       pdu.num_symbols =
                                                                                           static_cast<prs_num_symbols>(
                                                                                               value);
                                                                                     }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, false},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false},
                                                          test_case_data{7, false},
                                                          test_case_data{6, true},
                                                          test_case_data{9, false},
                                                          test_case_data{11, false},
                                                          test_case_data{12, true},
                                                          test_case_data{13, false})));

INSTANTIATE_TEST_SUITE_P(symbol_offset,
                         validate_prs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{
                                              "Symbol offset",
                                              [](dl_prs_pdu& pdu, int value) { pdu.first_symbol = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{6, true},
                                                          test_case_data{12, true},
                                                          test_case_data{13, false})));

INSTANTIATE_TEST_SUITE_P(num_rbs,
                         validate_prs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{
                                              "Number of RBs",
                                              [](dl_prs_pdu& pdu, int value) { pdu.num_rbs = value; }}),
                                          testing::Values(test_case_data{23, false},
                                                          test_case_data{24, true},
                                                          test_case_data{25, false},
                                                          test_case_data{26, false},
                                                          test_case_data{27, false},
                                                          test_case_data{28, true},
                                                          test_case_data{272, true},
                                                          test_case_data{274, false})));

INSTANTIATE_TEST_SUITE_P(start_rb,
                         validate_prs_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_prs_pdu>{
                                              "PRB offset",
                                              [](dl_prs_pdu& pdu, int value) { pdu.start_rb = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1100, true},
                                                          test_case_data{2176, true},
                                                          test_case_data{2177, false})));

/// Valid PDU should pass.
TEST(validate_prs_pdu, valid_pdu_passes)
{
  dl_prs_pdu pdu = build_valid_dl_prs_pdu();

  validator_report report(0, 0);
  EXPECT_TRUE(validate_dl_prs_pdu(pdu, report));
  // Assert no reports were generated.
  EXPECT_TRUE(report.reports.empty());
}

/// Add 3 errors and check that validation fails with 3 errors.
TEST(validate_prs_pdu, invalid_pdu_fails)
{
  dl_prs_pdu pdu = build_valid_dl_prs_pdu();

  // Force 3 errors.
  pdu.comb_size   = static_cast<prs_comb_size>(3);
  pdu.num_symbols = static_cast<prs_num_symbols>(7);
  pdu.num_rbs     = 5;

  validator_report report(0, 0);
  EXPECT_FALSE(validate_dl_prs_pdu(pdu, report));
  // Assert 3 reports were generated.
  EXPECT_EQ(report.reports.size(), 3u);
}
