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

#include "../../../lib/fapi/validators/tx_precoding_and_beamforming_pdu.h"
#include "../message_builder_helpers.h"
#include "helpers.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_tx_precoding_and_beamforming_pdu_field
  : public validate_fapi_sub_pdu<tx_precoding_and_beamforming_pdu, dl_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<tx_precoding_and_beamforming_pdu>, test_case_data>>
{};

TEST_P(validate_tx_precoding_and_beamforming_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_tx_precoding_and_beamforming_pdu,
               validate_tx_precoding_and_beamforming_pdu,
               srsran::fapi::message_type_id::dl_tti_request,
               dl_pdu_type::SSB);
};

INSTANTIATE_TEST_SUITE_P(nof_prgs,
                         validate_tx_precoding_and_beamforming_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<tx_precoding_and_beamforming_pdu>{
                                              "Number of PRGs",
                                              [](tx_precoding_and_beamforming_pdu& pdu, int value) {
                                                if (value == 0) {
                                                  pdu.prgs.clear();
                                                }
                                              }}),
                                          testing::Values(test_case_data{0, false}, test_case_data{1, true})));

INSTANTIATE_TEST_SUITE_P(prb_size,
                         validate_tx_precoding_and_beamforming_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<tx_precoding_and_beamforming_pdu>{
                                              "PRG size",
                                              [](tx_precoding_and_beamforming_pdu& pdu, int value) {
                                                pdu.prg_size = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{140, true},
                                                          test_case_data{275, true},
                                                          test_case_data{276, false})));

INSTANTIATE_TEST_SUITE_P(
    nof_beam_indexes,
    validate_tx_precoding_and_beamforming_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<tx_precoding_and_beamforming_pdu>{
                         "Number of beam indexes",
                         [](tx_precoding_and_beamforming_pdu& pdu, int value) {
                           // NOTE: This test uses the default build of a transmission precoding and beamforming PDU. In
                           // the default, the number of digital beamborming interfaces is configured to 2, son changing
                           // this number to a different one should result in failing the validation.
                           pdu.dig_bf_interfaces = value;
                         }}),
                     testing::Values(test_case_data{0, false}, test_case_data{2, true}, test_case_data{3, false})));

/// Valid PDU should pass.
TEST(validate_tx_precoding_and_beamforming_pdu, valid_pdu_passes)
{
  tx_precoding_and_beamforming_pdu pdu = build_valid_tx_precoding_and_beamforming_pdu();

  validator_report report(0, 0);
  EXPECT_TRUE(validate_tx_precoding_and_beamforming_pdu(pdu, report, srsran::fapi::dl_pdu_type::CSI_RS));
  // Assert no reports were generated.
  EXPECT_TRUE(report.reports.empty());
}

TEST(validate_tx_precoding_and_beamforming_pdu, invalid_pdu_should_fails)
{
  tx_precoding_and_beamforming_pdu pdu = build_valid_tx_precoding_and_beamforming_pdu();

  // Add 2 errors.
  pdu.prgs.clear();
  pdu.prg_size = 0;

  validator_report report(0, 0);
  EXPECT_FALSE(validate_tx_precoding_and_beamforming_pdu(pdu, report, srsran::fapi::dl_pdu_type::CSI_RS));
  // Assert 3 reports were generated.
  EXPECT_EQ(report.reports.size(), 2u);
}
