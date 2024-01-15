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

#include "../../../lib/fapi/pdu_validators/uci_pdus.h"
#include "helpers.h"
#include "srsran/fapi/message_validators.h"
#include "srsran/support/test_utils.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_uci_pusch_pdu_field
  : public validate_fapi_pdu<uci_pusch_pdu, uci_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<uci_pusch_pdu>, test_case_data>>
{};

TEST_P(validate_uci_pusch_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_uci_pusch_pdu,
               validate_uci_pusch_pdu,
               srsran::fapi::message_type_id::uci_indication,
               uci_pdu_type::PUSCH);
}

INSTANTIATE_TEST_SUITE_P(RNTI,
                         validate_uci_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "RNTI",
                                              [](uci_pusch_pdu& pdu, int value) { pdu.rnti = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32767, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(TA,
                         validate_uci_pusch_pdu_field,
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
                         validate_uci_pusch_pdu_field,
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
                         validate_uci_pusch_pdu_field,
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
                         validate_uci_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "RSRP",
                                              [](uci_pusch_pdu& pdu, int value) { pdu.rsrp = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{640, true},
                                                          test_case_data{1280, true},
                                                          test_case_data{1281, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(
    HARQDetection,
    validate_uci_pusch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                         "HARQ detection status",
                         [](uci_pusch_pdu& pdu, int value) {
                           pdu.pdu_bitmap.set(uci_pusch_pdu::HARQ_BIT);
                           pdu.harq.detection_status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(value);
                           if (pdu.harq.detection_status ==
                                   srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure ||
                               pdu.harq.detection_status == srsran::uci_pusch_or_pucch_f2_3_4_detection_status::dtx) {
                             pdu.harq.payload = {};
                           }
                         }}),
                     testing::Values(test_case_data{0, false},
                                     test_case_data{1, true},
                                     test_case_data{3, true},
                                     test_case_data{5, true},
                                     test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(HARQ_bit_length,
                         validate_uci_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "Expected HARQ bit length",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.harq.expected_bit_length = value;
                                                pdu.harq.payload =
                                                    bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>(value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true})));

INSTANTIATE_TEST_SUITE_P(
    HARQ_payload_bit_length_valid,
    validate_uci_pusch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                         "Expected HARQ payload",
                         [](uci_pusch_pdu& pdu, int value) {
                           if (value != 2) {
                             pdu.harq.expected_bit_length = value;
                           }
                           pdu.harq.payload = bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>(value);
                         }}),
                     testing::Values(test_case_data{1, true}, test_case_data{2, false}, test_case_data{3, true})));

INSTANTIATE_TEST_SUITE_P(HARQ_payload_bit_length_invalid,
                         validate_uci_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "Expected HARQ payload",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.harq.detection_status =
                                                    srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure;
                                                pdu.harq.payload =
                                                    bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true}, test_case_data{1, false})));

INSTANTIATE_TEST_SUITE_P(
    CSIPart1Detection,
    validate_uci_pusch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                         "CSI-Part1 detection status",
                         [](uci_pusch_pdu& pdu, int value) {
                           pdu.pdu_bitmap.set(uci_pusch_pdu::CSI_PART1_BIT);
                           pdu.csi_part1.detection_status =
                               static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(value);
                           if (pdu.csi_part1.detection_status ==
                                   srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure ||
                               pdu.csi_part1.detection_status ==
                                   srsran::uci_pusch_or_pucch_f2_3_4_detection_status::dtx) {
                             pdu.csi_part1.payload = {};
                           }
                         }}),
                     testing::Values(test_case_data{0, false},
                                     test_case_data{1, true},
                                     test_case_data{3, true},
                                     test_case_data{5, true},
                                     test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(CSI1_bit_length,
                         validate_uci_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "Expected CSI-Part1 bit length",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.csi_part1.expected_bit_length = value;
                                                pdu.csi_part1.payload =
                                                    bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>(value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true})));

INSTANTIATE_TEST_SUITE_P(
    CSIPart2Detection,
    validate_uci_pusch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                         "CSI-Part2 detection status",
                         [](uci_pusch_pdu& pdu, int value) {
                           pdu.pdu_bitmap.set(uci_pusch_pdu::CSI_PART2_BIT);
                           pdu.csi_part2.detection_status =
                               static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(value);
                           if (pdu.csi_part2.detection_status ==
                                   srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure ||
                               pdu.csi_part2.detection_status ==
                                   srsran::uci_pusch_or_pucch_f2_3_4_detection_status::dtx) {
                             pdu.csi_part2.payload = {};
                           }
                         }}),
                     testing::Values(test_case_data{0, false},
                                     test_case_data{1, true},
                                     test_case_data{3, true},
                                     test_case_data{5, true},
                                     test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(CSI2_bit_length,
                         validate_uci_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_pusch_pdu>{
                                              "Expected CSI-Part2 bit length",
                                              [](uci_pusch_pdu& pdu, int value) {
                                                pdu.csi_part2.expected_bit_length = value;
                                                pdu.csi_part2.payload =
                                                    bounded_bitset<uci_constants::MAX_NOF_HARQ_BITS>(value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true})));

TEST(validate_uci_pusch_pdu_field, valid_pdu_passes)
{
  auto pdu = build_valid_uci_pusch_pdu();

  validator_report report(0, 0);
  ASSERT_TRUE(validate_uci_pusch_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_TRUE(report.reports.empty());
}

TEST(validate_uci_pusch_pdu_field, invalid_pdu_fails)
{
  auto pdu = build_valid_uci_pusch_pdu();

  // Add 4 errors.
  pdu.timing_advance_offset_ns = std::numeric_limits<int16_t>::max();
  pdu.rssi                     = -16801;
  pdu.harq.detection_status    = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(6);

  validator_report report(0, 0);
  ASSERT_FALSE(validate_uci_pusch_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_EQ(report.reports.size(), 3u);
}

class validate_uci_pucch_format01_pdu_field
  : public validate_fapi_pdu<uci_pucch_pdu_format_0_1, uci_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<uci_pucch_pdu_format_0_1>, test_case_data>>
{};

TEST_P(validate_uci_pucch_format01_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_uci_pucch_format01_pdu,
               validate_uci_pucch_format01_pdu,
               srsran::fapi::message_type_id::uci_indication,
               uci_pdu_type::PUCCH_format_0_1);
};

INSTANTIATE_TEST_SUITE_P(RNTI,
                         validate_uci_pucch_format01_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                                              "RNTI",
                                              [](uci_pucch_pdu_format_0_1& pdu, int value) { pdu.rnti = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32767, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(
    Format,
    validate_uci_pucch_format01_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                         "PUCCH format",
                         [](uci_pucch_pdu_format_0_1& pdu, int value) {
                           pdu.pucch_format = static_cast<uci_pucch_pdu_format_0_1::format_type>(value);
                         }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(TA,
                         validate_uci_pucch_format01_pdu_field,
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
                         validate_uci_pucch_format01_pdu_field,
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
                         validate_uci_pucch_format01_pdu_field,
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
                         validate_uci_pucch_format01_pdu_field,
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
                         validate_uci_pucch_format01_pdu_field,
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
                         validate_uci_pucch_format01_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                                              "HARQ confidence level",
                                              [](uci_pucch_pdu_format_0_1& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_0_1::HARQ_BIT);
                                                pdu.harq.harq_values.resize(1, uci_pucch_f0_or_f1_harq_values::nack);
                                                pdu.harq.harq_confidence_level = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, false},
                                                          test_case_data{254, false},
                                                          test_case_data{255, true})));

INSTANTIATE_TEST_SUITE_P(
    HARQNumber,
    validate_uci_pucch_format01_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                         "Number of HARQ bits",
                         [](uci_pucch_pdu_format_0_1& pdu, int value) {
                           pdu.pdu_bitmap.set(uci_pucch_pdu_format_0_1::HARQ_BIT);
                           pdu.harq.harq_values.resize(value, uci_pucch_f0_or_f1_harq_values::nack);
                           pdu.harq.harq_confidence_level = 0;
                         }}),
                     testing::Values(test_case_data{0, false}, test_case_data{1, true}, test_case_data{2, true})));

INSTANTIATE_TEST_SUITE_P(HARQValue,
                         validate_uci_pucch_format01_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_0_1>{
                                              "HARQ value",
                                              [](uci_pucch_pdu_format_0_1& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_0_1::HARQ_BIT);
                                                pdu.harq.harq_confidence_level = 0;
                                                pdu.harq.harq_values.resize(1);
                                                pdu.harq.harq_values.front() =
                                                    static_cast<uci_pucch_f0_or_f1_harq_values>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

TEST(validate_uci_pucch_format01_pdu, valid_pdu_passes)
{
  auto pdu = build_valid_uci_pucch_format01_pdu();

  validator_report report(0, 0);
  ASSERT_TRUE(validate_uci_pucch_format01_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_TRUE(report.reports.empty());
}

TEST(validate_uci_pucch_format01_pdu, invalid_pdu_fails)
{
  auto pdu = build_valid_uci_pucch_format01_pdu();

  // Add 3 errors.
  pdu.harq.harq_confidence_level = 2;
  pdu.harq.harq_values.back()    = static_cast<uci_pucch_f0_or_f1_harq_values>(3);
  pdu.sr.sr_confidence_level     = 2;

  validator_report report(0, 0);
  ASSERT_FALSE(validate_uci_pucch_format01_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_EQ(report.reports.size(), 3u);
}

class validate_uci_pucch_format234_pdu_field
  : public validate_fapi_pdu<uci_pucch_pdu_format_2_3_4, uci_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<uci_pucch_pdu_format_2_3_4>, test_case_data>>
{};

TEST_P(validate_uci_pucch_format234_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_uci_pucch_format234_pdu,
               validate_uci_pucch_format234_pdu,
               srsran::fapi::message_type_id::uci_indication,
               uci_pdu_type::PUCCH_format_2_3_4);
};

INSTANTIATE_TEST_SUITE_P(RNTI,
                         validate_uci_pucch_format234_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "RNTI",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) { pdu.rnti = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32767, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(Format,
                         validate_uci_pucch_format234_pdu_field,
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
                         validate_uci_pucch_format234_pdu_field,
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
                         validate_uci_pucch_format234_pdu_field,
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
                         validate_uci_pucch_format234_pdu_field,
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
                         validate_uci_pucch_format234_pdu_field,
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
                         validate_uci_pucch_format234_pdu_field,
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

INSTANTIATE_TEST_SUITE_P(
    HARQDetection,
    validate_uci_pucch_format234_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                         "HARQ detection status",
                         [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                           pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::HARQ_BIT);
                           pdu.harq.detection_status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(value);
                           if (pdu.harq.detection_status == srsran::uci_pusch_or_pucch_f2_3_4_detection_status::dtx ||
                               pdu.harq.detection_status ==
                                   srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure) {
                             pdu.harq.payload = {};
                           }
                         }}),
                     testing::Values(test_case_data{0, false},
                                     test_case_data{1, true},
                                     test_case_data{3, true},
                                     test_case_data{5, true},
                                     test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(
    CSIPart1Detection,
    validate_uci_pucch_format234_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                         "CSI-Part1 detection status",
                         [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                           pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT);
                           pdu.csi_part1.detection_status =
                               static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(value);
                           if (pdu.csi_part1.detection_status ==
                                   srsran::uci_pusch_or_pucch_f2_3_4_detection_status::dtx ||
                               pdu.csi_part1.detection_status ==
                                   srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure) {
                             pdu.csi_part1.payload = {};
                           }
                         }}),
                     testing::Values(test_case_data{0, false},
                                     test_case_data{1, true},
                                     test_case_data{3, true},
                                     test_case_data{5, true},
                                     test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(CSIPart1BitLength,
                         validate_uci_pucch_format234_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                                              "Expected CSI-Part1 bit length",
                                              [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                                                pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::CSI_PART1_BIT);
                                                pdu.csi_part1.detection_status =
                                                    uci_pusch_or_pucch_f2_3_4_detection_status::crc_pass;
                                                pdu.csi_part1.expected_bit_length = value;
                                                pdu.csi_part1.payload =
                                                    bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_OR_PART2_BITS>(
                                                        value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{850, true},
                                                          test_case_data{1706, true})));

INSTANTIATE_TEST_SUITE_P(
    CSIPart2Detection,
    validate_uci_pucch_format234_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<uci_pucch_pdu_format_2_3_4>{
                         "CSI-Part2 detection status",
                         [](uci_pucch_pdu_format_2_3_4& pdu, int value) {
                           pdu.pdu_bitmap.set(uci_pucch_pdu_format_2_3_4::CSI_PART2_BIT);
                           pdu.csi_part2.detection_status =
                               static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(value);
                           if (pdu.csi_part2.detection_status ==
                                   srsran::uci_pusch_or_pucch_f2_3_4_detection_status::dtx ||
                               pdu.csi_part2.detection_status ==
                                   srsran::uci_pusch_or_pucch_f2_3_4_detection_status::crc_failure) {
                             pdu.csi_part2.payload = {};
                           }
                         }}),
                     testing::Values(test_case_data{0, false},
                                     test_case_data{1, true},
                                     test_case_data{3, true},
                                     test_case_data{5, true},
                                     test_case_data{6, false})));

TEST(validate_uci_pucch_format234_pdu, valid_pdu_passes)
{
  auto pdu = build_valid_uci_pucch_format234_pdu();

  validator_report report(0, 0);
  ASSERT_TRUE(validate_uci_pucch_format234_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_TRUE(report.reports.empty());
}

TEST(validate_uci_pucch_format234_pdu, invalid_pdu_fails)
{
  auto pdu = build_valid_uci_pucch_format234_pdu();

  // Add 4 errors.
  pdu.timing_advance_offset      = 64;
  pdu.rsrp                       = 1281;
  pdu.csi_part1.detection_status = static_cast<uci_pusch_or_pucch_f2_3_4_detection_status>(6);
  pdu.sr.sr_bitlen               = 1707;

  validator_report report(0, 0);
  ASSERT_FALSE(validate_uci_pucch_format234_pdu(pdu, report));
  // Assert no reports were generated.
  ASSERT_EQ(report.reports.size(), 4u);
}

class validate_uci_indication_field
  : public validate_fapi_message<uci_indication_message>,
    public testing::TestWithParam<std::tuple<pdu_field_data<uci_indication_message>, test_case_data>>
{};

TEST_P(validate_uci_indication_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_uci_indication,
               validate_uci_indication,
               srsran::fapi::message_type_id::uci_indication);
};

INSTANTIATE_TEST_SUITE_P(SFN,
                         validate_uci_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_indication_message>{
                                              "sfn",
                                              [](uci_indication_message& msg, int value) { msg.sfn = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{522, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(slot,
                         validate_uci_indication_field,
                         testing::Combine(testing::Values(pdu_field_data<uci_indication_message>{
                                              "slot",
                                              [](uci_indication_message& msg, int value) { msg.slot = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{80, true},
                                                          test_case_data{159, true},
                                                          test_case_data{160, false})));

TEST(validate_uci_indication, valid_indication_passes)
{
  auto msg = build_valid_uci_indication();

  const auto& result = validate_uci_indication(msg);
  ASSERT_TRUE(result);
}

TEST(validate_uci_indication, invalid_indication_fails)
{
  auto msg = build_valid_uci_indication();

  // Add 3 errors.
  msg.sfn                    = 1024;
  msg.pdus[0].pusch_pdu.rssi = 1281;

  const auto& result = validate_uci_indication(msg);
  ASSERT_FALSE(result);
  // Assert no reports were generated.
  const auto& report = result.error();
  ASSERT_EQ(report.reports.size(), 2u);
}
