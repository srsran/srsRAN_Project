/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi/validators/dl_csi_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

class ValidateCSIPDUField : public ValidateFAPIPDU<dl_csi_rs_pdu, dl_pdu_type>,
                            public testing::TestWithParam<std::tuple<pdu_field_data<dl_csi_rs_pdu>, test_case_data>>
{};

TEST_P(ValidateCSIPDUField, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_dl_csi_pdu,
               validate_dl_csi_pdu,
               srsgnb::fapi::message_type_id::dl_tti_request,
               dl_pdu_type::CSI_RS);
};

INSTANTIATE_TEST_SUITE_P(Subcarrier_spacing,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "Subcarrier spacing",
                                              [](dl_csi_rs_pdu& pdu, int value) {
                                                pdu.scs = to_subcarrier_spacing(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(
    Cyclic_prefix,
    ValidateCSIPDUField,
    testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                         "Cyclic prefix",
                         [](dl_csi_rs_pdu& pdu, int value) { pdu.cp = static_cast<cyclic_prefix::options>(value); }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(Start_RB,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "Start RB",
                                              [](dl_csi_rs_pdu& pdu, int value) { pdu.start_rb = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(Number_of_RBs,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "Number of RBs",
                                              [](dl_csi_rs_pdu& pdu, int value) { pdu.num_rbs = value; }}),
                                          testing::Values(test_case_data{23, false},
                                                          test_case_data{24, true},
                                                          test_case_data{150, true},
                                                          test_case_data{276, true},
                                                          test_case_data{277, false})));

INSTANTIATE_TEST_SUITE_P(CSI_type,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "CSI type",
                                              [](dl_csi_rs_pdu& pdu, int value) {
                                                pdu.type = static_cast<csi_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(Row,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "Row",
                                              [](dl_csi_rs_pdu& pdu, int value) { pdu.row = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{9, true},
                                                          test_case_data{18, true},
                                                          test_case_data{19, false})));

INSTANTIATE_TEST_SUITE_P(Symbol_L0,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "Symbol L0",
                                              [](dl_csi_rs_pdu& pdu, int value) { pdu.symb_L0 = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{6, true},
                                                          test_case_data{13, true},
                                                          test_case_data{14, false})));

INSTANTIATE_TEST_SUITE_P(Symbol_L1,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "Symbol L1",
                                              [](dl_csi_rs_pdu& pdu, int value) { pdu.symb_L1 = value; }}),
                                          testing::Values(test_case_data{1, false},
                                                          test_case_data{2, true},
                                                          test_case_data{5, true},
                                                          test_case_data{12, true},
                                                          test_case_data{13, false})));

INSTANTIATE_TEST_SUITE_P(CDM_type,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "CDM type",
                                              [](dl_csi_rs_pdu& pdu, int value) {
                                                pdu.cdm_type = static_cast<csi_cdm_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(Freq_density,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "Freq density",
                                              [](dl_csi_rs_pdu& pdu, int value) {
                                                pdu.freq_density = static_cast<csi_freq_density_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(Scrambling_ID,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "Scrambling ID",
                                              [](dl_csi_rs_pdu& pdu, int value) { pdu.scramb_id = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(Power_control_offset,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "Power control offset",
                                              [](dl_csi_rs_pdu& pdu, int value) {
                                                pdu.power_control_offset_profile_nr                       = value;
                                                pdu.csi_rs_maintenance_v3.csi_rs_power_offset_profile_sss = -32768;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{12, true},
                                                          test_case_data{23, true},
                                                          test_case_data{24, false})));

INSTANTIATE_TEST_SUITE_P(Power_control_offset_v3,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "Power control offset",
                                              [](dl_csi_rs_pdu& pdu, int value) {
                                                pdu.power_control_offset_profile_nr                       = 255;
                                                pdu.csi_rs_maintenance_v3.csi_rs_power_offset_profile_sss = value;
                                              }}),
                                          testing::Values(test_case_data{static_cast<unsigned>(int16_t(-32767)), true},
                                                          test_case_data{0, true},
                                                          test_case_data{32767, true},
                                                          test_case_data{static_cast<unsigned>(int16_t(-32768)),
                                                                         false})));

INSTANTIATE_TEST_SUITE_P(Power_control_offset_SS_profile_NR,
                         ValidateCSIPDUField,
                         testing::Combine(testing::Values(pdu_field_data<dl_csi_rs_pdu>{
                                              "Power control offset SS profile NR",
                                              [](dl_csi_rs_pdu& pdu, int value) {
                                                pdu.power_control_offset_ss_profile_nr =
                                                    static_cast<nzp_csi_rs_epre_to_ssb>(value);
                                                pdu.csi_rs_maintenance_v3.csi_rs_power_offset_profile_sss = -32768;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false},
                                                          test_case_data{254, false},
                                                          test_case_data{255, true})));

/// Valid PDU should pass.
TEST(ValidateCSIPDU, ValidPDUPasses)
{
  dl_csi_rs_pdu pdu = build_valid_dl_csi_pdu();

  validator_report report(0, 0);
  EXPECT_TRUE(validate_dl_csi_pdu(pdu, report));
  // Assert no reports were generated.
  EXPECT_TRUE(report.reports.empty());
}

/// Add 3 errors and check that validation fails with 3 errors.
TEST(ValidateCSIPDU, InvalidPDUFails)
{
  dl_csi_rs_pdu pdu = build_valid_dl_csi_pdu();

  // Force 3 errors.
  pdu.num_rbs = 0;
  pdu.symb_L1 = 100;
  pdu.row     = 128;

  validator_report report(0, 0);
  EXPECT_FALSE(validate_dl_csi_pdu(pdu, report));
  // Assert 3 reports were generated.
  EXPECT_EQ(report.reports.size(), 3u);
}
