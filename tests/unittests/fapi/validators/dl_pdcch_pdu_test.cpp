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

#include "../../../lib/fapi/pdu_validators/dl_pdcch_pdu.h"
#include "helpers.h"
#include "srsran/fapi/message_validators.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_pdcch_pdu_field : public validate_fapi_pdu<dl_pdcch_pdu, dl_pdu_type>,
                                 public testing::TestWithParam<std::tuple<pdu_field_data<dl_pdcch_pdu>, test_case_data>>
{};

TEST_P(validate_pdcch_pdu_field, WithValue)
{
  auto params = GetParam();

  std::function<bool(const dl_pdcch_pdu&, validator_report&)> validator = [](const dl_pdcch_pdu& pdu,
                                                                             validator_report&   report_) {
    return validate_dl_pdcch_pdu(message_type_id::dl_tti_request, pdu, report_);
  };

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_dl_pdcch_pdu,
               validator,
               srsran::fapi::message_type_id::dl_tti_request,
               dl_pdu_type::PDCCH);
};

INSTANTIATE_TEST_SUITE_P(coreset_bwp_size,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "CORESET BWP size",
                                              [](dl_pdcch_pdu& pdu, int value) { pdu.coreset_bwp_size = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{140, true},
                                                          test_case_data{275, true},
                                                          test_case_data{276, false})));

INSTANTIATE_TEST_SUITE_P(coreset_bwp_start,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "CORESET BWP start",
                                              [](dl_pdcch_pdu& pdu, int value) { pdu.coreset_bwp_start = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{140, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(Subcarrier_spacing,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "Subcarrier spacing",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.scs = to_subcarrier_spacing(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(
    Cyclic_prefix,
    validate_pdcch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                         "Cyclic prefix",
                         [](dl_pdcch_pdu& pdu, int value) { pdu.cp = static_cast<cyclic_prefix::options>(value); }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(start_symbol,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "Start symbol index",
                                              [](dl_pdcch_pdu& pdu, int value) { pdu.start_symbol_index = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{7, true},
                                                          test_case_data{13, true},
                                                          test_case_data{14, false})));

INSTANTIATE_TEST_SUITE_P(duration_symbols,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "Duration symbols",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.duration_symbols     = value;
                                                pdu.cce_reg_mapping_type = cce_to_reg_mapping_type::non_interleaved;
                                                pdu.reg_bundle_size      = 6;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(
    cce_reg_mapping,
    validate_pdcch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{"CCE to REG mapping type",
                                                                  [](dl_pdcch_pdu& pdu, int value) {
                                                                    pdu.cce_reg_mapping_type =
                                                                        static_cast<cce_to_reg_mapping_type>(value);
                                                                    if (pdu.cce_reg_mapping_type ==
                                                                        cce_to_reg_mapping_type::interleaved) {
                                                                      pdu.duration_symbols = 2;
                                                                      pdu.reg_bundle_size  = 2;
                                                                      pdu.shift_index      = 1;
                                                                      pdu.interleaver_size = 2;
                                                                    } else {
                                                                      pdu.reg_bundle_size = 6;
                                                                    }
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(reg_bundle_size,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "REG bundle size",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.cce_reg_mapping_type = cce_to_reg_mapping_type::non_interleaved;
                                                pdu.reg_bundle_size      = value;
                                              }}),
                                          testing::Values(test_case_data{1, false},
                                                          test_case_data{2, false},
                                                          test_case_data{3, false},
                                                          test_case_data{4, false},
                                                          test_case_data{5, false},
                                                          test_case_data{6, true},
                                                          test_case_data{7, false})));

INSTANTIATE_TEST_SUITE_P(reg_bundle_size_2,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "REG bundle size",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.cce_reg_mapping_type = cce_to_reg_mapping_type::interleaved;
                                                pdu.duration_symbols     = 2;
                                                pdu.reg_bundle_size      = value;
                                              }}),
                                          testing::Values(test_case_data{1, false},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, false},
                                                          test_case_data{5, false},
                                                          test_case_data{6, true},
                                                          test_case_data{7, false})));

INSTANTIATE_TEST_SUITE_P(reg_bundle_size_3,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "REG bundle size",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.cce_reg_mapping_type = cce_to_reg_mapping_type::interleaved;
                                                pdu.duration_symbols     = 1;
                                                pdu.reg_bundle_size      = value;
                                              }}),
                                          testing::Values(test_case_data{1, false},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, false},
                                                          test_case_data{5, false},
                                                          test_case_data{6, true},
                                                          test_case_data{7, false})));

INSTANTIATE_TEST_SUITE_P(reg_bundle_size_4,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "REG bundle size",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.cce_reg_mapping_type = cce_to_reg_mapping_type::interleaved;
                                                pdu.duration_symbols     = 3;
                                                pdu.reg_bundle_size      = value;
                                              }}),
                                          testing::Values(test_case_data{1, false},
                                                          test_case_data{2, false},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false},
                                                          test_case_data{5, false},
                                                          test_case_data{6, true},
                                                          test_case_data{7, false})));

INSTANTIATE_TEST_SUITE_P(interleaver_size,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "Interleaver size",
                                              [](dl_pdcch_pdu& pdu, int value) { pdu.interleaver_size = value; }}),
                                          testing::Values(test_case_data{1, false},
                                                          test_case_data{2, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false},
                                                          test_case_data{5, false},
                                                          test_case_data{6, true},
                                                          test_case_data{7, false})));

INSTANTIATE_TEST_SUITE_P(
    coreset_type,
    validate_pdcch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{"CORESET type",
                                                                  [](dl_pdcch_pdu& pdu, int value) {
                                                                    pdu.coreset_type =
                                                                        static_cast<pdcch_coreset_type>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(shift_index,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "Shift index - PBCH or SIB1",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.coreset_type = pdcch_coreset_type::pbch_or_coreset0;
                                                pdu.shift_index  = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(shift_index_oher,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "Shift index - Other",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.coreset_type = pdcch_coreset_type::other;
                                                pdu.shift_index  = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{140, true},
                                                          test_case_data{275, true},
                                                          test_case_data{276, false})));

INSTANTIATE_TEST_SUITE_P(
    precoder_granularity,
    validate_pdcch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                         "Precoder granularity",
                         [](dl_pdcch_pdu& pdu, int value) {
                           pdu.precoder_granularity =
                               static_cast<coreset_configuration::precoder_granularity_type>(value);
                         }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(rnti,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "RNTI",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.dl_dci.back().rnti = to_rnti(value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32768, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(cce_index,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "CCE index",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.dl_dci.back().cce_index = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{63, true},
                                                          test_case_data{135, true},
                                                          test_case_data{136, false})));

INSTANTIATE_TEST_SUITE_P(aggregation_level,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "Aggregation level",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.dl_dci.back().aggregation_level = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false},
                                                          test_case_data{7, false},
                                                          test_case_data{8, true},
                                                          test_case_data{9, false},
                                                          test_case_data{12, false},
                                                          test_case_data{15, false},
                                                          test_case_data{16, true},
                                                          test_case_data{17, false})));

INSTANTIATE_TEST_SUITE_P(dmrs_power,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "DMRS Power offset",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.dl_dci.back().power_control_offset_ss_profile_nr = value;
                                                pdu.maintenance_v3.info.back().pdcch_dmrs_power_offset_profile_sss =
                                                    -32768;
                                              }}),
                                          testing::Values(test_case_data{static_cast<unsigned>(int16_t(-126)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-9)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-8)), true},
                                                          test_case_data{0, true},
                                                          test_case_data{8, true},
                                                          test_case_data{9, false})));

INSTANTIATE_TEST_SUITE_P(dmrs_power_1,
                         validate_pdcch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                                              "DMRS Power offset",
                                              [](dl_pdcch_pdu& pdu, int value) {
                                                pdu.dl_dci.back().power_control_offset_ss_profile_nr = -127;
                                                pdu.maintenance_v3.info.back().pdcch_dmrs_power_offset_profile_sss =
                                                    value;
                                              }}),
                                          testing::Values(test_case_data{static_cast<unsigned>(int16_t(-32768)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-32767)), true},
                                                          test_case_data{0, true},
                                                          test_case_data{32767, true})));

INSTANTIATE_TEST_SUITE_P(
    collocated_al16_candidate,
    validate_pdcch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdcch_pdu>{
                         "Collocated AL16 candidate",
                         [](dl_pdcch_pdu& pdu, int value) {
                           pdu.maintenance_v3.info.back().collocated_AL16_candidate = value;
                         }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

/// Valid PDU should pass.
TEST(validate_pdcch_pdu, valid_pdu_passes)
{
  dl_pdcch_pdu pdu = build_valid_dl_pdcch_pdu();

  validator_report report(0, 0);
  EXPECT_TRUE(validate_dl_pdcch_pdu(message_type_id::dl_tti_request, pdu, report));
  // Assert no reports were generated.
  EXPECT_TRUE(report.reports.empty());
}

/// Add 3 errors and check that validation fails with 3 errors.
TEST(validate_pdcch_pdu, invalid_pdu_fails)
{
  dl_pdcch_pdu pdu = build_valid_dl_pdcch_pdu();

  // Force 3 errors.
  pdu.coreset_bwp_size = 2690;
  pdu.reg_bundle_size  = 8;
  pdu.dl_dci[0].rnti   = to_rnti(0);

  validator_report report(0, 0);
  EXPECT_FALSE(validate_dl_pdcch_pdu(message_type_id::dl_tti_request, pdu, report));
  // Assert 3 reports were generated.
  EXPECT_EQ(report.reports.size(), 3u);
}
