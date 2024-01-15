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

#include "../../../lib/fapi/pdu_validators/dl_ssb_pdu.h"
#include "helpers.h"
#include "srsran/fapi/message_validators.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_ssb_pdu_field : public validate_fapi_pdu<dl_ssb_pdu, dl_pdu_type>,
                               public testing::TestWithParam<std::tuple<pdu_field_data<dl_ssb_pdu>, test_case_data>>
{};

TEST_P(validate_ssb_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_dl_ssb_pdu,
               validate_dl_ssb_pdu,
               srsran::fapi::message_type_id::dl_tti_request,
               dl_pdu_type::SSB);
};

INSTANTIATE_TEST_SUITE_P(pci,
                         validate_ssb_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_ssb_pdu>{
                                              "Physical cell ID",
                                              [](dl_ssb_pdu& pdu, int value) { pdu.phys_cell_id = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{511, true},
                                                          test_case_data{1007, true},
                                                          test_case_data{1008, false})));

INSTANTIATE_TEST_SUITE_P(pss,
                         validate_ssb_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_ssb_pdu>{
                                              "Beta PSS profile NR",
                                              [](dl_ssb_pdu& pdu, int value) {
                                                pdu.beta_pss_profile_nr = static_cast<beta_pss_profile_type>(value);
                                                pdu.ssb_maintenance_v3.beta_pss_profile_sss =
                                                    (value == 255) ? 0 : std::numeric_limits<int16_t>::min();
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, false},
                                                          test_case_data{254, false},
                                                          test_case_data{255, true})));

INSTANTIATE_TEST_SUITE_P(pbch_block_index,
                         validate_ssb_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_ssb_pdu>{
                                              "SS/PBCH block index",
                                              [](dl_ssb_pdu& pdu, int value) { pdu.ssb_block_index = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{32, true},
                                                          test_case_data{63, true},
                                                          test_case_data{64, false})));

INSTANTIATE_TEST_SUITE_P(k_ssb,
                         validate_ssb_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_ssb_pdu>{
                                              "Subcarrier offset",
                                              [](dl_ssb_pdu& pdu, int value) { pdu.ssb_subcarrier_offset = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{16, true},
                                                          test_case_data{31, true},
                                                          test_case_data{32, false})));

INSTANTIATE_TEST_SUITE_P(
    offset_pointA,
    validate_ssb_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_ssb_pdu>{
                         "Offset point A",
                         [](dl_ssb_pdu& pdu, int value) { pdu.ssb_offset_pointA = value; }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1100, true}, test_case_data{2199, true})));

INSTANTIATE_TEST_SUITE_P(ssb_case,
                         validate_ssb_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_ssb_pdu>{
                                              "SSB Case type",
                                              [](dl_ssb_pdu& pdu, int value) {
                                                pdu.ssb_maintenance_v3.case_type = static_cast<ssb_pattern_case>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(scs,
                         validate_ssb_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_ssb_pdu>{
                                              "Subcarrier spacing",
                                              [](dl_ssb_pdu& pdu, int value) {
                                                pdu.ssb_maintenance_v3.scs = static_cast<subcarrier_spacing>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(L_max,
                         validate_ssb_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_ssb_pdu>{"L_max",
                                                                                     [](dl_ssb_pdu& pdu, int value) {
                                                                                       pdu.ssb_maintenance_v3.L_max =
                                                                                           value;
                                                                                     }}),
                                          testing::Values(test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false},
                                                          test_case_data{7, false},
                                                          test_case_data{8, true},
                                                          test_case_data{9, false},
                                                          test_case_data{63, false},
                                                          test_case_data{64, true},
                                                          test_case_data{65, false})));

INSTANTIATE_TEST_SUITE_P(scaling_power,
                         validate_ssb_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_ssb_pdu>{
                                              "Baseband power scaling applied to SS-PBCH",
                                              [](dl_ssb_pdu& pdu, int value) {
                                                pdu.ssb_maintenance_v3.ss_pbch_block_power_scaling = value;
                                              }}),
                                          testing::Values(test_case_data{static_cast<unsigned>(int16_t(-32768)), true},
                                                          test_case_data{static_cast<unsigned>(int16_t(-32767)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-11001)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-11000)), true},
                                                          test_case_data{0, true},
                                                          test_case_data{12000, true},
                                                          test_case_data{12001, false})));

/// Valid PDU should pass.
TEST(validate_ssb_pdu, valid_pdu_passes)
{
  dl_ssb_pdu pdu = build_valid_dl_ssb_pdu();

  validator_report report(0, 0);
  EXPECT_TRUE(validate_dl_ssb_pdu(pdu, report));
  // Assert no reports were generated.
  EXPECT_TRUE(report.reports.empty());
}

/// Add 3 errors and check that validation fails with 3 errors.
TEST(validate_ssb_pdu, invalid_pdu_fails)
{
  dl_ssb_pdu pdu = build_valid_dl_ssb_pdu();

  // Force 3 errors.
  pdu.phys_cell_id                                   = 2000;
  pdu.ssb_block_index                                = 100;
  pdu.ssb_maintenance_v3.ss_pbch_block_power_scaling = -12000;

  validator_report report(0, 0);
  EXPECT_FALSE(validate_dl_ssb_pdu(pdu, report));
  // Assert 3 reports were generated.
  EXPECT_EQ(report.reports.size(), 3u);
}

/// Only build and run this test when asserts are available, as the death is caused by an assert.
#ifdef ASSERTS_ENABLED
TEST(validate_ssb_pdu_death, invalid_point_A_death_test)
{
  EXPECT_DEATH(
      {
        dl_ssb_pdu pdu        = build_valid_dl_ssb_pdu();
        pdu.ssb_offset_pointA = 2200;
        validator_report report(0, 0);
        validate_dl_ssb_pdu(pdu, report);
      },
      ".*");
}
#endif
