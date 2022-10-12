/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi/validators/uci_pdus.h"
#include "helpers.h"
#include "srsgnb/fapi/message_validators.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;
using namespace fapi;
using namespace unittest;

class ValidateUCIPUSCHPDUField
  : public ValidateFAPIPDU<uci_pusch_pdu, uci_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<uci_pusch_pdu>, test_case_data>>
{};

TEST_P(ValidateUCIPUSCHPDUField, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_uci_pusch_pdu,
               validate_uci_pusch_pdu,
               srsgnb::fapi::message_type_id::uci_indication,
               uci_pdu_type::PUSCH);
}

INSTANTIATE_TEST_SUITE_P(RNTI,
                         ValidateUCIPUSCHPDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "RNTI",
                                              [](uci_pusch_pdu& pdu, int value) { pdu.rnti = to_rnti(value); }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32767, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(TA,
                         ValidateUCIPUSCHPDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "Timing advance offset",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.timing_advance_offset = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{31, true},
                                                          test_case_data{63, true},
                                                          test_case_data{64, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(TA_ns,
                         ValidateUCIPUSCHPDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "Timing advance offset in nanoseconds",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.timing_advance_offset_ns = value;
                                              }}),
                                          testing::Values(test_case_data{static_cast<unsigned>(int16_t(-32768)), true},
                                                          test_case_data{static_cast<unsigned>(int16_t(-32767)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-16801)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-16800)), true},
                                                          test_case_data{0, true},
                                                          test_case_data{16800, true},
                                                          test_case_data{16801, false})));

INSTANTIATE_TEST_SUITE_P(RSSI,
                         ValidateUCIPUSCHPDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "RSSI",
                                              [](uci_pusch_pdu& pdu, int value) { pdu.rssi = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{640, true},
                                                          test_case_data{1280, true},
                                                          test_case_data{1281, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(RSRP,
                         ValidateUCIPUSCHPDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "RSRP",
                                              [](uci_pusch_pdu& pdu, int value) { pdu.rsrp = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{640, true},
                                                          test_case_data{1280, true},
                                                          test_case_data{1281, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(HARQDetection,
                         ValidateUCIPUSCHPDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "HARQ detection status",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pusch_pdu::HARQ_BIT);
                                                pdu.harq.detection_status = static_cast<uci_detection_status>(value);
                                                pdu.harq.bit_length       = 1;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{5, true},
                                                          test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(HARQBitLength,
                         ValidateUCIPUSCHPDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "Expected HARQ bit length",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pusch_pdu::HARQ_BIT);
                                                pdu.harq.detection_status = uci_detection_status::crc_failure;
                                                pdu.harq.bit_length       = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(CSIPart1Detection,
                         ValidateUCIPUSCHPDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "CSI Part 1 detection status",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pusch_pdu::CSI_PART1_BIT);
                                                pdu.csi_part1.detection_status =
                                                    static_cast<uci_detection_status>(value);
                                                pdu.csi_part1.bit_length = 1;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{5, true},
                                                          test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(CSIPart1BitLength,
                         ValidateUCIPUSCHPDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "Expected CSI part1 bit length",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pusch_pdu::CSI_PART1_BIT);
                                                pdu.csi_part1.detection_status = uci_detection_status::crc_failure;
                                                pdu.csi_part1.bit_length       = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(CSIPart2Detection,
                         ValidateUCIPUSCHPDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "CSI Part 2 detection status",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pusch_pdu::CSI_PART2_BIT);
                                                pdu.csi_part2.detection_status =
                                                    static_cast<uci_detection_status>(value);
                                                pdu.csi_part2.bit_length = 1;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{5, true},
                                                          test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(CSIPart2BitLength,
                         ValidateUCIPUSCHPDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "Expected CSI part2 bit length",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pusch_pdu::CSI_PART2_BIT);
                                                pdu.csi_part2.detection_status = uci_detection_status::crc_failure;
                                                pdu.csi_part2.bit_length       = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

TEST(ValidateUCIPUSCHPDU, ValidPDUPasses)
{
  auto pdu = build_valid_uci_pusch_pdu();

  validator_report report(0, 0);
  ASSERT_TRUE(validate_uci_pusch_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_TRUE(report.reports.empty());
}

TEST(ValidateUCIPUSCHPDU, InvalidPDUFails)
{
  auto pdu = build_valid_uci_pusch_pdu();

  // Add 4 errors.
  pdu.timing_advance_offset_ns = std::numeric_limits<int16_t>::max();
  pdu.rssi                     = -16801;
  pdu.harq.detection_status    = static_cast<uci_detection_status>(6);
  pdu.csi_part2.bit_length     = 1707;

  validator_report report(0, 0);
  ASSERT_FALSE(validate_uci_pusch_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_EQ(report.reports.size(), 4u);
}

class ValidateUCIPUCCHFormat01PDUField
  : public ValidateFAPIPDU<uci_pucch_pdu_format_0_1, uci_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<uci_pucch_pdu_format_0_1>, test_case_data>>
{};

TEST_P(ValidateUCIPUCCHFormat01PDUField, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_uci_pucch_format01_pdu,
               validate_uci_pucch_format01_pdu,
               srsgnb::fapi::message_type_id::uci_indication,
               uci_pdu_type::PUCCH_format_0_1);
};

INSTANTIATE_TEST_SUITE_P(RNTI,
                         ValidateUCIPUCCHFormat01PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                                              "RNTI",
                                              [](uci_pucch_pdu_format_0_1& pdu, int value) {
                                                pdu.rnti = to_rnti(value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32767, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(
    Format,
    ValidateUCIPUCCHFormat01PDUField,
    testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                         "PUCCH format",
                         [](uci_pucch_pdu_format_0_1& pdu, int value) {
                           pdu.pucch_format = static_cast<uci_pucch_pdu_format_0_1::format_type>(value);
                         }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(TA,
                         ValidateUCIPUCCHFormat01PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                                              "Timing advance offset",
                                              [](uci_pucch_pdu_format_0_1& pdu, int value) {
                                                pdu.timing_advance_offset = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{31, true},
                                                          test_case_data{63, true},
                                                          test_case_data{64, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(TA_ns,
                         ValidateUCIPUCCHFormat01PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                                              "Timing advance offset in nanoseconds",
                                              [](uci_pucch_pdu_format_0_1& pdu, int value) {
                                                pdu.timing_advance_offset_ns = value;
                                              }}),
                                          testing::Values(test_case_data{static_cast<unsigned>(int16_t(-32768)), true},
                                                          test_case_data{static_cast<unsigned>(int16_t(-32767)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-16801)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-16800)), true},
                                                          test_case_data{0, true},
                                                          test_case_data{16800, true},
                                                          test_case_data{16801, false})));

INSTANTIATE_TEST_SUITE_P(RSSI,
                         ValidateUCIPUCCHFormat01PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                                              "RSSI",
                                              [](uci_pucch_pdu_format_0_1& pdu, int value) { pdu.rssi = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{640, true},
                                                          test_case_data{1280, true},
                                                          test_case_data{1281, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(RSRP,
                         ValidateUCIPUCCHFormat01PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                                              "RSRP",
                                              [](uci_pucch_pdu_format_0_1& pdu, int value) { pdu.rsrp = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{640, true},
                                                          test_case_data{1280, true},
                                                          test_case_data{1281, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(SRConfidenceLevel,
                         ValidateUCIPUCCHFormat01PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                                              "SR confidence level",
                                              [](uci_pucch_pdu_format_0_1& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_0_1::SR_BIT);
                                                pdu.sr.sr_confidence_level = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, false},
                                                          test_case_data{254, false},
                                                          test_case_data{255, true})));

INSTANTIATE_TEST_SUITE_P(HARQConfidence,
                         ValidateUCIPUCCHFormat01PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                                              "HARQ confidence level",
                                              [](uci_pucch_pdu_format_0_1& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_0_1::HARQ_BIT);
                                                pdu.harq.harq_values.resize(1, 0);
                                                pdu.harq.harq_confidence_level = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, false},
                                                          test_case_data{254, false},
                                                          test_case_data{255, true})));

INSTANTIATE_TEST_SUITE_P(
    HARQNumber,
    ValidateUCIPUCCHFormat01PDUField,
    testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                         "Number of HARQ bits",
                         [](uci_pucch_pdu_format_0_1& pdu, int value) {
                           pdu.pdu_bitmap.set(uci_pucch_pdu_format_0_1::HARQ_BIT);
                           pdu.harq.harq_values.resize(value, 0);
                           pdu.harq.harq_confidence_level = 0;
                         }}),
                     testing::Values(test_case_data{0, false}, test_case_data{1, true}, test_case_data{2, true})));

INSTANTIATE_TEST_SUITE_P(HARQValue,
                         ValidateUCIPUCCHFormat01PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                                              "HARQ value",
                                              [](uci_pucch_pdu_format_0_1& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_0_1::HARQ_BIT);
                                                pdu.harq.harq_confidence_level = 0;
                                                pdu.harq.harq_values.resize(1);
                                                pdu.harq.harq_values.front() = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

TEST(ValidateUCIPUCCHFormat01PDU, ValidPDUPasses)
{
  auto pdu = build_valid_uci_pucch_format01_pdu();

  validator_report report(0, 0);
  ASSERT_TRUE(validate_uci_pucch_format01_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_TRUE(report.reports.empty());
}

TEST(ValidateUCIPUCCHFormat01PDU, InvalidPDUFails)
{
  auto pdu = build_valid_uci_pucch_format01_pdu();

  // Add 3 errors.
  pdu.harq.harq_confidence_level = 2;
  pdu.harq.harq_values.back()    = 3;
  pdu.sr.sr_confidence_level     = 2;

  validator_report report(0, 0);
  ASSERT_FALSE(validate_uci_pucch_format01_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_EQ(report.reports.size(), 3u);
}

class ValidateUCIPUCCHFormat234PDUField
  : public ValidateFAPIPDU<uci_pucch_pdu_format_2_3_4, uci_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<uci_pucch_pdu_format_2_3_4>, test_case_data>>
{};

TEST_P(ValidateUCIPUCCHFormat234PDUField, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_uci_pucch_format234_pdu,
               validate_uci_pucch_format234_pdu,
               srsgnb::fapi::message_type_id::uci_indication,
               uci_pdu_type::PUCCH_format_2_3_4);
};

INSTANTIATE_TEST_SUITE_P(RNTI,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "RNTI",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.rnti = to_rnti(value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32767, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(Format,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "PUCCH format",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pucch_format =
                                                    static_cast<uci_pucch_pdu_format_2_3_4::format_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(TA,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "Timing advance offset",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.timing_advance_offset = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{31, true},
                                                          test_case_data{63, true},
                                                          test_case_data{64, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(TA_ns,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "Timing advance offset in nanoseconds",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.timing_advance_offset_ns = value;
                                              }}),
                                          testing::Values(test_case_data{static_cast<unsigned>(int16_t(-32768)), true},
                                                          test_case_data{static_cast<unsigned>(int16_t(-32767)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-16801)), false},
                                                          test_case_data{static_cast<unsigned>(int16_t(-16800)), true},
                                                          test_case_data{0, true},
                                                          test_case_data{16800, true},
                                                          test_case_data{16801, false})));

INSTANTIATE_TEST_SUITE_P(RSSI,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "RSSI",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) { pdu.rssi = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{640, true},
                                                          test_case_data{1280, true},
                                                          test_case_data{1281, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(RSRP,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "RSRP",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) { pdu.rsrp = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{640, true},
                                                          test_case_data{1280, true},
                                                          test_case_data{1281, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(SRBitlength,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "SR bit length",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::SR_BIT);
                                                pdu.sr.sr_bitlen = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(HARQDetection,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "HARQ detection status",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::HARQ_BIT);
                                                pdu.harq.detection_status = static_cast<uci_detection_status>(value);
                                                pdu.harq.bit_length       = 1;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{5, true},
                                                          test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(HARQBitLength,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "Expected HARQ bit length",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::HARQ_BIT);
                                                pdu.harq.detection_status = uci_detection_status::crc_failure;
                                                pdu.harq.bit_length       = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(CSIPart1Detection,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "CSI Part 1 detection status",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT);
                                                pdu.csi_part1.detection_status =
                                                    static_cast<uci_detection_status>(value);
                                                pdu.csi_part1.bit_length       = 1;
                                                pdu.uci_part1.detection_status = uci_detection_status::crc_failure;
                                                pdu.uci_part1.expected_uci_payload_size = 1;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{5, true},
                                                          test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(CSIPart1BitLength,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "Expected CSI part1 bit length",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT);
                                                pdu.csi_part1.detection_status = uci_detection_status::crc_failure;
                                                pdu.csi_part1.bit_length       = value;
                                                pdu.uci_part1.detection_status = uci_detection_status::crc_failure;
                                                pdu.uci_part1.expected_uci_payload_size = 1;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(CSIPart2Detection,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "CSI Part 2 detection status",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT);
                                                pdu.csi_part2.detection_status =
                                                    static_cast<uci_detection_status>(value);
                                                pdu.csi_part2.bit_length       = 1;
                                                pdu.uci_part2.detection_status = uci_detection_status::crc_failure;
                                                pdu.uci_part2.expected_uci_payload_size = 1;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{5, true},
                                                          test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(CSIPart2BitLength,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "Expected CSI part2 bit length",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT);
                                                pdu.csi_part2.detection_status = uci_detection_status::crc_failure;
                                                pdu.uci_part2.detection_status = uci_detection_status::crc_failure;
                                                pdu.uci_part2.expected_uci_payload_size = 1;
                                                pdu.csi_part2.bit_length                = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(UCIPart1Detection,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "UCI detection status",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT);
                                                pdu.csi_part1.detection_status = uci_detection_status::crc_failure;
                                                pdu.csi_part1.bit_length       = 1;
                                                pdu.uci_part1.detection_status =
                                                    static_cast<uci_detection_status>(value);
                                                pdu.uci_part1.expected_uci_payload_size = 1;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{5, true},
                                                          test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(UCIPart1BitLength,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "Expected UCI payload size",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT);
                                                pdu.csi_part1.detection_status = uci_detection_status::crc_failure;
                                                pdu.csi_part1.bit_length       = 1;
                                                pdu.uci_part1.detection_status = uci_detection_status::crc_failure;
                                                pdu.uci_part1.expected_uci_payload_size = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(UCIPart2Detection,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "UCI detection status",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT);
                                                pdu.csi_part2.detection_status = uci_detection_status::crc_failure;
                                                pdu.csi_part2.bit_length       = 1;
                                                pdu.uci_part2.detection_status =
                                                    static_cast<uci_detection_status>(value);
                                                pdu.uci_part2.expected_uci_payload_size = 1;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{5, true},
                                                          test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(UCIPart2BitLength,
                         ValidateUCIPUCCHFormat234PDUField,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "Expected UCI payload size",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT);
                                                pdu.csi_part2.detection_status = uci_detection_status::crc_failure;
                                                pdu.csi_part2.bit_length       = 1;
                                                pdu.uci_part2.detection_status = uci_detection_status::crc_failure;
                                                pdu.uci_part2.expected_uci_payload_size = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

TEST(ValidateUCIPUCCHFormat234PDU, ValidPDUPasses)
{
  auto pdu = build_valid_uci_pucch_format234_pdu();

  validator_report report(0, 0);
  ASSERT_TRUE(validate_uci_pucch_format234_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_TRUE(report.reports.empty());
}

TEST(ValidateUCIPUCCHFormat234PDU, InvalidPDUFails)
{
  auto pdu = build_valid_uci_pucch_format234_pdu();

  // Add 4 errors.
  pdu.timing_advance_offset      = 64;
  pdu.rsrp                       = 1281;
  pdu.csi_part1.detection_status = static_cast<uci_detection_status>(6);
  pdu.sr.sr_bitlen               = 1707;

  validator_report report(0, 0);
  ASSERT_FALSE(validate_uci_pucch_format234_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_EQ(report.reports.size(), 4u);
}

class ValidateUCIIndicationField
  : public ValidateFAPIMessage<uci_indication_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<uci_indication_message>, test_case_data>>
{};

TEST_P(ValidateUCIIndicationField, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_uci_indication,
               validate_uci_indication,
               srsgnb::fapi::message_type_id::uci_indication);
};

INSTANTIATE_TEST_SUITE_P(SFN,
                         ValidateUCIIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<uci_indication_message>{
                                              "sfn",
                                              [](uci_indication_message& msg, int value) { msg.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{522, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         ValidateUCIIndicationField,
                         testing::Combine(testing::Values(pdu_field_data<uci_indication_message>{
                                              "slot",
                                              [](uci_indication_message& msg, int value) { msg.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

TEST(ValidateUCIIndication, ValidIndicationPasses)
{
  auto msg = build_valid_uci_indication();

  const auto& result = validate_uci_indication(msg);
  ASSERT_TRUE(result);
}

TEST(ValidateUCIIndication, InvalidIndicationFails)
{
  auto msg = build_valid_uci_indication();

  // Add 3 errors.
  msg.sfn                                               = 1024;
  msg.pdus[0].pusch_pdu.rssi                            = 1281;
  msg.pdus[2].pucch_pdu_f234.csi_part1.detection_status = static_cast<uci_detection_status>(0);

  const auto& result = validate_uci_indication(msg);
  ASSERT_FALSE(result);
  // Assert no reports were generated.
  const auto& report = result.error();
  ASSERT_EQ(report.reports.size(), 3u);
}
