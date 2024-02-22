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

#include "../../../lib/fapi/validators/ul_pucch_pdu.h"
#include "../message_builder_helpers.h"
#include "helpers.h"
#include "srsran/fapi/message_validators.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_pucch_pdu_common_field
  : public validate_fapi_pdu<ul_pucch_pdu, ul_pdu_type>,
    public testing::TestWithParam<
        std::tuple<pdu_field_data<ul_pucch_pdu>, test_case_data, std::function<ul_pucch_pdu()>>>
{};

TEST_P(validate_pucch_pdu_common_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               std::get<2>(params),
               validate_ul_pucch_pdu,
               srsran::fapi::message_type_id::ul_tti_request,
               ul_pdu_type::PUCCH);
};

INSTANTIATE_TEST_SUITE_P(pci,
                         validate_pucch_pdu_common_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "RNTI",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.rnti = to_rnti(value); }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32768, true},
                                                          test_case_data{65535, true}),
                                          testing::Values(build_valid_ul_pucch_f0_pdu,
                                                          build_valid_ul_pucch_f1_pdu,
                                                          build_valid_ul_pucch_f2_pdu,
                                                          build_valid_ul_pucch_f3_pdu,
                                                          build_valid_ul_pucch_f4_pdu)));

INSTANTIATE_TEST_SUITE_P(bwp_size,
                         validate_pucch_pdu_common_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "BWP size",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.bwp_size = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{128, true},
                                                          test_case_data{275, true},
                                                          test_case_data{276, false}),
                                          testing::Values(build_valid_ul_pucch_f0_pdu,
                                                          build_valid_ul_pucch_f1_pdu,
                                                          build_valid_ul_pucch_f2_pdu,
                                                          build_valid_ul_pucch_f3_pdu,
                                                          build_valid_ul_pucch_f4_pdu)));

INSTANTIATE_TEST_SUITE_P(bwp_start,
                         validate_pucch_pdu_common_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "BWP start",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.bwp_start = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false}),
                                          testing::Values(build_valid_ul_pucch_f0_pdu,
                                                          build_valid_ul_pucch_f1_pdu,
                                                          build_valid_ul_pucch_f2_pdu,
                                                          build_valid_ul_pucch_f3_pdu,
                                                          build_valid_ul_pucch_f4_pdu)));

INSTANTIATE_TEST_SUITE_P(scs,
                         validate_pucch_pdu_common_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Subcarrier spacing",
                                              [](ul_pucch_pdu& pdu, int value) {
                                                pdu.scs = static_cast<subcarrier_spacing>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false}),
                                          testing::Values(build_valid_ul_pucch_f0_pdu,
                                                          build_valid_ul_pucch_f1_pdu,
                                                          build_valid_ul_pucch_f2_pdu,
                                                          build_valid_ul_pucch_f3_pdu,
                                                          build_valid_ul_pucch_f4_pdu)));

INSTANTIATE_TEST_SUITE_P(
    cp,
    validate_pucch_pdu_common_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                         "Cyclic prefix",
                         [](ul_pucch_pdu& pdu, int value) { pdu.cp = static_cast<cyclic_prefix::options>(value); }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false}),
                     testing::Values(build_valid_ul_pucch_f0_pdu,
                                     build_valid_ul_pucch_f1_pdu,
                                     build_valid_ul_pucch_f2_pdu,
                                     build_valid_ul_pucch_f3_pdu,
                                     build_valid_ul_pucch_f4_pdu)));

INSTANTIATE_TEST_SUITE_P(
    multi_slot_indicator,
    validate_pucch_pdu_common_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{"Multi slot tx indicator",
                                                                  [](ul_pucch_pdu& pdu, int value) {
                                                                    pdu.multi_slot_tx_indicator =
                                                                        static_cast<pucch_repetition_tx_slot>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{3, true}, test_case_data{4, false}),
                     testing::Values(build_valid_ul_pucch_f0_pdu,
                                     build_valid_ul_pucch_f1_pdu,
                                     build_valid_ul_pucch_f2_pdu,
                                     build_valid_ul_pucch_f3_pdu,
                                     build_valid_ul_pucch_f4_pdu)));

INSTANTIATE_TEST_SUITE_P(prb_start,
                         validate_pucch_pdu_common_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "PRB start",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.prb_start = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false}),
                                          testing::Values(build_valid_ul_pucch_f0_pdu,
                                                          build_valid_ul_pucch_f1_pdu,
                                                          build_valid_ul_pucch_f2_pdu,
                                                          build_valid_ul_pucch_f3_pdu,
                                                          build_valid_ul_pucch_f4_pdu)));

INSTANTIATE_TEST_SUITE_P(prb_size,
                         validate_pucch_pdu_common_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "PRB size",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.prb_size = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{8, true},
                                                          test_case_data{16, true},
                                                          test_case_data{17, false}),
                                          testing::Values(build_valid_ul_pucch_f0_pdu,
                                                          build_valid_ul_pucch_f1_pdu,
                                                          build_valid_ul_pucch_f2_pdu,
                                                          build_valid_ul_pucch_f3_pdu,
                                                          build_valid_ul_pucch_f4_pdu)));

INSTANTIATE_TEST_SUITE_P(start_symbol,
                         validate_pucch_pdu_common_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Start symbol index",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.start_symbol_index = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{6, true},
                                                          test_case_data{13, true},
                                                          test_case_data{14, false}),
                                          testing::Values(build_valid_ul_pucch_f0_pdu,
                                                          build_valid_ul_pucch_f1_pdu,
                                                          build_valid_ul_pucch_f2_pdu,
                                                          build_valid_ul_pucch_f3_pdu,
                                                          build_valid_ul_pucch_f4_pdu)));

INSTANTIATE_TEST_SUITE_P(second_hop_prb,
                         validate_pucch_pdu_common_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Second hop PRB",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.second_hop_prb = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false}),
                                          testing::Values(build_valid_ul_pucch_f0_pdu,
                                                          build_valid_ul_pucch_f1_pdu,
                                                          build_valid_ul_pucch_f2_pdu,
                                                          build_valid_ul_pucch_f3_pdu,
                                                          build_valid_ul_pucch_f4_pdu)));

INSTANTIATE_TEST_SUITE_P(
    part2_size_map_scope,
    validate_pucch_pdu_common_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                         "Part2 size map scope",
                         [](ul_pucch_pdu& pdu, int value) {
                           pdu.uci_correspondence.part2.back().part2_size_map_scope =
                               static_cast<uci_part1_to_part2_correspondence_v3::map_scope_type>(value);
                         }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false}),
                     testing::Values(build_valid_ul_pucch_f0_pdu,
                                     build_valid_ul_pucch_f1_pdu,
                                     build_valid_ul_pucch_f2_pdu,
                                     build_valid_ul_pucch_f3_pdu,
                                     build_valid_ul_pucch_f4_pdu)));

class validate_pucch_f0_pdu_field
  : public validate_fapi_pdu<ul_pucch_pdu, ul_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<ul_pucch_pdu>, test_case_data>>
{};

TEST_P(validate_pucch_f0_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_ul_pucch_f0_pdu,
               validate_ul_pucch_pdu,
               srsran::fapi::message_type_id::ul_tti_request,
               ul_pdu_type::PUCCH);
};

INSTANTIATE_TEST_SUITE_P(duration,
                         validate_pucch_f0_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Duration in symbols",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nr_of_symbols = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(
    group_hopping,
    validate_pucch_f0_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{"PUCCH group hopping",
                                                                  [](ul_pucch_pdu& pdu, int value) {
                                                                    pdu.pucch_grp_hopping =
                                                                        static_cast<pucch_group_hopping>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{2, true}, test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(nid_group_hopping,
                         validate_pucch_f0_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "NID PUCCH hopping",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_hopping = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(cyclic_shift,
                         validate_pucch_f0_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Initial cyclic shift",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.initial_cyclic_shift = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{6, true},
                                                          test_case_data{11, true},
                                                          test_case_data{12, false})));

INSTANTIATE_TEST_SUITE_P(
    sr_bit_length,
    validate_pucch_f0_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                         "SR bit len",
                         [](ul_pucch_pdu& pdu, int value) { pdu.sr_bit_len = value; }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(harq_bit_length,
                         validate_pucch_f0_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "HARQ bit len",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.bit_len_harq = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

class validate_pucch_f1_pdu_field
  : public validate_fapi_pdu<ul_pucch_pdu, ul_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<ul_pucch_pdu>, test_case_data>>
{};

TEST_P(validate_pucch_f1_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_ul_pucch_f1_pdu,
               validate_ul_pucch_pdu,
               srsran::fapi::message_type_id::ul_tti_request,
               ul_pdu_type::PUCCH);
};

INSTANTIATE_TEST_SUITE_P(duration,
                         validate_pucch_f1_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Duration in symbols",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nr_of_symbols = value; }}),
                                          testing::Values(test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{9, true},
                                                          test_case_data{14, true},
                                                          test_case_data{15, false})));

INSTANTIATE_TEST_SUITE_P(
    group_hopping,
    validate_pucch_f1_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{"PUCCH group hopping",
                                                                  [](ul_pucch_pdu& pdu, int value) {
                                                                    pdu.pucch_grp_hopping =
                                                                        static_cast<pucch_group_hopping>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{2, true}, test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(nid_group_hopping,
                         validate_pucch_f1_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "NID PUCCH hopping",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_hopping = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(cyclic_shift,
                         validate_pucch_f1_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Initial cyclic shift",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.initial_cyclic_shift = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{6, true},
                                                          test_case_data{11, true},
                                                          test_case_data{12, false})));

INSTANTIATE_TEST_SUITE_P(
    sr_bit_length,
    validate_pucch_f1_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                         "SR bit len",
                         [](ul_pucch_pdu& pdu, int value) { pdu.sr_bit_len = value; }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(harq_bit_length,
                         validate_pucch_f1_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "HARQ bit len",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.bit_len_harq = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(
    time_domain_occasions,
    validate_pucch_f1_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                         "Time domain occ index",
                         [](ul_pucch_pdu& pdu, int value) { pdu.time_domain_occ_index = value; }}),
                     testing::Values(test_case_data{0, true}, test_case_data{6, true}, test_case_data{7, false})));

class validate_pucch_f2_pdu_field
  : public validate_fapi_pdu<ul_pucch_pdu, ul_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<ul_pucch_pdu>, test_case_data>>
{};

TEST_P(validate_pucch_f2_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_ul_pucch_f2_pdu,
               validate_ul_pucch_pdu,
               srsran::fapi::message_type_id::ul_tti_request,
               ul_pdu_type::PUCCH);
};

INSTANTIATE_TEST_SUITE_P(duration,
                         validate_pucch_f2_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Duration in symbols",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nr_of_symbols = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(nid_scrambling,
                         validate_pucch_f2_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "NID PUCCH scrambling",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_scrambling = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(
    sr_bit_length,
    validate_pucch_f2_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                         "SR bit len",
                         [](ul_pucch_pdu& pdu, int value) { pdu.sr_bit_len = value; }}),
                     testing::Values(test_case_data{0, true}, test_case_data{4, true}, test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(harq_bit_length,
                         validate_pucch_f2_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "HARQ bit len",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.bit_len_harq = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(csi1_bit_length,
                         validate_pucch_f2_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "CSI part1 bit length",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.csi_part1_bit_length = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{800, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(max_code_rate,
                         validate_pucch_f2_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Max code rate",
                                              [](ul_pucch_pdu& pdu, int value) {
                                                pdu.pucch_maintenance_v3.max_code_rate = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{3, true},
                                                          test_case_data{7, true},
                                                          test_case_data{8, false})));

class validate_pucch_f3_pdu_field
  : public validate_fapi_pdu<ul_pucch_pdu, ul_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<ul_pucch_pdu>, test_case_data>>
{};

TEST_P(validate_pucch_f3_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_ul_pucch_f3_pdu,
               validate_ul_pucch_pdu,
               srsran::fapi::message_type_id::ul_tti_request,
               ul_pdu_type::PUCCH);
};

INSTANTIATE_TEST_SUITE_P(duration,
                         validate_pucch_f3_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Duration in symbols",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nr_of_symbols = value; }}),
                                          testing::Values(test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{9, true},
                                                          test_case_data{14, true},
                                                          test_case_data{15, false})));

INSTANTIATE_TEST_SUITE_P(nid_scrambling,
                         validate_pucch_f3_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "NID PUCCH scrambling",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_scrambling = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(
    group_hopping,
    validate_pucch_f3_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{"PUCCH group hopping",
                                                                  [](ul_pucch_pdu& pdu, int value) {
                                                                    pdu.pucch_grp_hopping =
                                                                        static_cast<pucch_group_hopping>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{2, true}, test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(nid_group_hopping,
                         validate_pucch_f3_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "NID PUCCH hopping",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_hopping = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(cyclic_shift,
                         validate_pucch_f3_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Initial cyclic shift",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.initial_cyclic_shift = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{6, true},
                                                          test_case_data{11, true},
                                                          test_case_data{12, false})));

INSTANTIATE_TEST_SUITE_P(m0_dmrs_cyclic_shift,
                         validate_pucch_f3_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "M0 PUCCH DMRS cyclic shift",
                                              [](ul_pucch_pdu& pdu, int value) {
                                                pdu.m0_pucch_dmrs_cyclic_shift = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{4, true},
                                                          test_case_data{9, true},
                                                          test_case_data{10, false})));

INSTANTIATE_TEST_SUITE_P(
    sr_bit_length,
    validate_pucch_f3_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                         "SR bit len",
                         [](ul_pucch_pdu& pdu, int value) { pdu.sr_bit_len = value; }}),
                     testing::Values(test_case_data{0, true}, test_case_data{4, true}, test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(harq_bit_length,
                         validate_pucch_f3_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "HARQ bit len",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.bit_len_harq = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(csi1_bit_length,
                         validate_pucch_f3_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "CSI part1 bit length",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.csi_part1_bit_length = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{800, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(max_code_rate,
                         validate_pucch_f3_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Max code rate",
                                              [](ul_pucch_pdu& pdu, int value) {
                                                pdu.pucch_maintenance_v3.max_code_rate = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{3, true},
                                                          test_case_data{7, true},
                                                          test_case_data{8, false})));

class validate_pucch_f4_pdu_field
  : public validate_fapi_pdu<ul_pucch_pdu, ul_pdu_type>,
    public testing::TestWithParam<std::tuple<pdu_field_data<ul_pucch_pdu>, test_case_data>>
{};

TEST_P(validate_pucch_f4_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_ul_pucch_f4_pdu,
               validate_ul_pucch_pdu,
               srsran::fapi::message_type_id::ul_tti_request,
               ul_pdu_type::PUCCH);
};

INSTANTIATE_TEST_SUITE_P(duration,
                         validate_pucch_f4_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Duration in symbols",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nr_of_symbols = value; }}),
                                          testing::Values(test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{9, true},
                                                          test_case_data{14, true},
                                                          test_case_data{15, false})));

INSTANTIATE_TEST_SUITE_P(nid_scrambling,
                         validate_pucch_f4_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "NID PUCCH scrambling",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_scrambling = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(
    group_hopping,
    validate_pucch_f4_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{"PUCCH group hopping",
                                                                  [](ul_pucch_pdu& pdu, int value) {
                                                                    pdu.pucch_grp_hopping =
                                                                        static_cast<pucch_group_hopping>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{2, true}, test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(nid_group_hopping,
                         validate_pucch_f4_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "NID PUCCH hopping",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.nid_pucch_hopping = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(cyclic_shift,
                         validate_pucch_f4_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Initial cyclic shift",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.initial_cyclic_shift = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{6, true},
                                                          test_case_data{11, true},
                                                          test_case_data{12, false})));

INSTANTIATE_TEST_SUITE_P(m0_dmrs_cyclic_shift,
                         validate_pucch_f4_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "M0 PUCCH DMRS cyclic shift",
                                              [](ul_pucch_pdu& pdu, int value) {
                                                pdu.m0_pucch_dmrs_cyclic_shift = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{4, true},
                                                          test_case_data{9, true},
                                                          test_case_data{10, false})));

INSTANTIATE_TEST_SUITE_P(
    sr_bit_length,
    validate_pucch_f4_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                         "SR bit len",
                         [](ul_pucch_pdu& pdu, int value) { pdu.sr_bit_len = value; }}),
                     testing::Values(test_case_data{0, true}, test_case_data{4, true}, test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(harq_bit_length,
                         validate_pucch_f4_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "HARQ bit len",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.bit_len_harq = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(csi1_bit_length,
                         validate_pucch_f4_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "CSI part1 bit length",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.csi_part1_bit_length = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{800, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(max_code_rate,
                         validate_pucch_f4_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Max code rate",
                                              [](ul_pucch_pdu& pdu, int value) {
                                                pdu.pucch_maintenance_v3.max_code_rate = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{3, true},
                                                          test_case_data{7, true},
                                                          test_case_data{8, false})));

INSTANTIATE_TEST_SUITE_P(pre_dft_occasion_index,
                         validate_pucch_f4_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Pre DFT occ index",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.pre_dft_occ_idx = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(pre_dft_occasion_length,
                         validate_pucch_f4_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pucch_pdu>{
                                              "Pre DFT occ len",
                                              [](ul_pucch_pdu& pdu, int value) { pdu.pre_dft_occ_len = value; }}),
                                          testing::Values(test_case_data{1, false},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

class validate_pucch_pdu : public testing::TestWithParam<std::tuple<std::function<ul_pucch_pdu()>>>
{};

TEST_P(validate_pucch_pdu, valid_pdu_passes)
{
  auto             params = GetParam();
  const auto&      pdu    = std::get<0>(params)();
  validator_report report(0, 0);
  ASSERT_TRUE(validate_ul_pucch_pdu(pdu, report));
  ASSERT_TRUE(report.reports.empty());
}

TEST_P(validate_pucch_pdu, invalid_pdu_fails)
{
  auto             params = GetParam();
  auto             pdu    = std::get<0>(params)();
  validator_report report(0, 0);
  // Add 4 errors.
  pdu.start_symbol_index = 20;
  pdu.prb_size           = 30;
  pdu.prb_start          = 300;
  pdu.rnti               = to_rnti(0);

  ASSERT_FALSE(validate_ul_pucch_pdu(pdu, report));
  ASSERT_EQ(4U, report.reports.size());
  // Check that the properties that caused the error are different.
  for (unsigned i = 0, e = report.reports.size(); i != e; ++i) {
    for (unsigned j = i + 1; j != e; ++j) {
      ASSERT_TRUE(std::strcmp(report.reports[i].property_name, report.reports[j].property_name) != 0);
    }
  }
}

INSTANTIATE_TEST_SUITE_P(builders,
                         validate_pucch_pdu,
                         testing::Combine(testing::Values(build_valid_ul_pucch_f0_pdu,
                                                          build_valid_ul_pucch_f1_pdu,
                                                          build_valid_ul_pucch_f2_pdu,
                                                          build_valid_ul_pucch_f3_pdu,
                                                          build_valid_ul_pucch_f4_pdu)));
