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

#include "../../../lib/fapi/validators/ul_pusch_pdu.h"
#include "../message_builder_helpers.h"
#include "helpers.h"
#include "srsran/fapi/message_validators.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_pusch_pdu_field : public validate_fapi_pdu<ul_pusch_pdu, ul_pdu_type>,
                                 public testing::TestWithParam<std::tuple<pdu_field_data<ul_pusch_pdu>, test_case_data>>
{};

TEST_P(validate_pusch_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_ul_pusch_pdu,
               validate_ul_pusch_pdu,
               srsran::fapi::message_type_id::ul_tti_request,
               ul_pdu_type::PUSCH);
};

INSTANTIATE_TEST_SUITE_P(RNTI,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "RNTI",
                                              [](ul_pusch_pdu& pdu, int value) { pdu.rnti = to_rnti(value); }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32768, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(BWP_Size,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "BWP size",
                                              [](ul_pusch_pdu& pdu, int value) { pdu.bwp_size = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{128, true},
                                                          test_case_data{275, true},
                                                          test_case_data{276, false})));

INSTANTIATE_TEST_SUITE_P(BWP_Start,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "BWP start",
                                              [](ul_pusch_pdu& pdu, int value) { pdu.bwp_start = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(Subcarrier_spacing,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Subcarrier spacing",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.scs = to_subcarrier_spacing(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(
    Cyclic_prefix,
    validate_pusch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                         "Cyclic prefix",
                         [](ul_pusch_pdu& pdu, int value) { pdu.cp = static_cast<cyclic_prefix::options>(value); }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(Modulation_order_tp_disabled,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "QAM modulation order",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.transform_precoding = false;
                                                pdu.qam_mod_order       = static_cast<modulation_scheme>(value);
                                              }}),
                                          testing::Values(test_case_data{1, false},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false},
                                                          test_case_data{6, true},
                                                          test_case_data{7, false},
                                                          test_case_data{8, true})));

INSTANTIATE_TEST_SUITE_P(Modulation_order_tp_enabled,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "QAM modulation order",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.transform_precoding = true;
                                                pdu.qam_mod_order       = static_cast<modulation_scheme>(value);
                                              }}),
                                          testing::Values(test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false},
                                                          test_case_data{6, true},
                                                          test_case_data{7, false},
                                                          test_case_data{8, true})));

INSTANTIATE_TEST_SUITE_P(MCS_index,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "MCS index",
                                              [](ul_pusch_pdu& pdu, int value) { pdu.mcs_index = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{16, true},
                                                          test_case_data{31, true},
                                                          test_case_data{32, false})));

INSTANTIATE_TEST_SUITE_P(MCS_table,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "MCS table",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.mcs_table = static_cast<pusch_mcs_table>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(NID_PUSCH,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "NID PUSCH",
                                              [](ul_pusch_pdu& pdu, int value) { pdu.nid_pusch = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(Layers,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Number of layers",
                                              [](ul_pusch_pdu& pdu, int value) { pdu.num_layers = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(
    DMRS_type,
    validate_pusch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                         "DMRS config type",
                         [](ul_pusch_pdu& pdu, int value) { pdu.dmrs_type = static_cast<dmrs_cfg_type>(value); }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(
    Low_PAPR_DMRS,
    validate_pusch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{"Low PAPR DMRS",
                                                                  [](ul_pusch_pdu& pdu, int value) {
                                                                    pdu.low_papr_dmrs =
                                                                        static_cast<low_papr_dmrs_type>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(DMRS_identity,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "PUSCH DMRS identity",
                                              [](ul_pusch_pdu& pdu, int value) { pdu.pusch_dmrs_identity = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{503, true},
                                                          test_case_data{1007, true},
                                                          test_case_data{1008, false})));

INSTANTIATE_TEST_SUITE_P(
    NSCID,
    validate_pusch_pdu_field,
    testing::Combine(
        testing::Values(pdu_field_data<ul_pusch_pdu>{"NSCID", [](ul_pusch_pdu& pdu, int value) { pdu.nscid = value; }}),
        testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(DMRS_groups_no_data,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Number of DMRS CDM groups without data",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.num_dmrs_cdm_grps_no_data = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(
    Resource_allocation_type,
    validate_pusch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{"Resource allocation type",
                                                                  [](ul_pusch_pdu& pdu, int value) {
                                                                    pdu.resource_alloc =
                                                                        static_cast<resource_allocation_type>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(RB_start,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "RB start",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.resource_alloc = resource_allocation_type::type_1;
                                                pdu.rb_size        = 1;
                                                pdu.rb_start       = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(RB_size,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "RB size",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.resource_alloc = resource_allocation_type::type_1;
                                                pdu.rb_start       = 1;
                                                pdu.rb_size        = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{128, true},
                                                          test_case_data{275, true},
                                                          test_case_data{276, false})));

INSTANTIATE_TEST_SUITE_P(
    VRB_mapping,
    validate_pusch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{"VRB-to-PRB mapping",
                                                                  [](ul_pusch_pdu& pdu, int value) {
                                                                    pdu.vrb_to_prb_mapping =
                                                                        static_cast<vrb_to_prb_mapping_type>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, false}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(Tx_direct_current_location,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Tx direct current location",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.tx_direct_current_location = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2048, true},
                                                          test_case_data{4095, true},
                                                          test_case_data{4096, false})));

INSTANTIATE_TEST_SUITE_P(Symbol_start,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Start symbol index",
                                              [](ul_pusch_pdu& pdu, int value) { pdu.start_symbol_index = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{6, true},
                                                          test_case_data{13, true},
                                                          test_case_data{14, false})));

INSTANTIATE_TEST_SUITE_P(Num_symbols,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Number of symbols",
                                              [](ul_pusch_pdu& pdu, int value) { pdu.nr_of_symbols = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{7, true},
                                                          test_case_data{14, true},
                                                          test_case_data{15, false})));

INSTANTIATE_TEST_SUITE_P(RV_index,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "RV index",
                                              [](ul_pusch_pdu& pdu, int value) { pdu.pusch_data.rv_index = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(HARQ_index,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "HARQ process id",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_data.harq_process_id = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{7, true},
                                                          test_case_data{15, true},
                                                          test_case_data{16, false})));

INSTANTIATE_TEST_SUITE_P(HARQ_ack_bitlen,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "HARQ ACK bit length",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_uci.harq_ack_bit_length = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{200, true},
                                                          test_case_data{800, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(CSI_part1_bitlen,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "CSI part1 bit length",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_uci.csi_part1_bit_length = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{200, true},
                                                          test_case_data{800, true},
                                                          test_case_data{1706, true},
                                                          test_case_data{1707, false})));

INSTANTIATE_TEST_SUITE_P(CSI_part2_bitlen,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Flag CSI part2",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_uci.flags_csi_part2 = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, false},
                                                          test_case_data{65534, false},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(alpha_scaling,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Alpha scaling",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_uci.alpha_scaling = static_cast<alpha_scaling_opt>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(beta_offset_HARQ,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Beta offset HARQ ACK",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_uci.beta_offset_harq_ack = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{7, true},
                                                          test_case_data{15, true},
                                                          test_case_data{16, false})));

INSTANTIATE_TEST_SUITE_P(beta_offset_CSI1,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Beta offset CSI Part1",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_uci.beta_offset_csi1 = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{9, true},
                                                          test_case_data{18, true},
                                                          test_case_data{19, false})));

INSTANTIATE_TEST_SUITE_P(beta_offset_CSI2,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Beta offset CSI Part2",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_uci.beta_offset_csi2 = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{9, true},
                                                          test_case_data{18, true},
                                                          test_case_data{19, false})));

INSTANTIATE_TEST_SUITE_P(PTRS_port,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "PTRS DMRS port",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_ptrs.port_info.back().ptrs_dmrs_port = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{5, true},
                                                          test_case_data{11, true},
                                                          test_case_data{12, false})));

INSTANTIATE_TEST_SUITE_P(PTRS_RE_offset,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "PTRS RE offset",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_ptrs.port_info.back().ptrs_re_offset = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{5, true},
                                                          test_case_data{11, true},
                                                          test_case_data{12, false})));

INSTANTIATE_TEST_SUITE_P(PTRS_time_density,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "PTRS time density",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_ptrs.ptrs_time_density = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(
    PTRS_freq_density,
    validate_pusch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                         "PTRS frequency density",
                         [](ul_pusch_pdu& pdu, int value) { pdu.pusch_ptrs.ptrs_freq_density = value; }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(UL_PTRS_power,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "UL PTRS power",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_ptrs.ul_ptrs_power = static_cast<ul_ptrs_power_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(low_PAPR_group,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Low PAPR group number",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_ofdm.low_papr_group_number = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{15, true},
                                                          test_case_data{29, true},
                                                          test_case_data{30, false})));

INSTANTIATE_TEST_SUITE_P(UL_PTRS_sample_density,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "UL PTRS sample density",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_ofdm.ul_ptrs_sample_density = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{4, true},
                                                          test_case_data{8, true},
                                                          test_case_data{9, false})));

INSTANTIATE_TEST_SUITE_P(UL_PTRS_time_density,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "UL PTRS time density transform precoding",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_ofdm.ul_ptrs_time_density_transform_precoding = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(trans_type,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "PUSCH trans type",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_maintenance_v3.pusch_trans_type = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(delta_BWP0_to_active_BWP,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Delta BWP0 start from active BWP",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_maintenance_v3.delta_bwp0_start_from_active_bwp = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(initial_BWP_size,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Initial UL BWP size",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_maintenance_v3.initial_ul_bwp_size = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(group_seq_hopping,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "Group or sequence hopping",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_maintenance_v3.group_or_sequence_hopping = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(second_hop_PRB,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "PUSCH second hop PRB",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_maintenance_v3.pusch_second_hop_prb = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(LDPC_graph,
                         validate_pusch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                                              "LDPC base graph",
                                              [](ul_pusch_pdu& pdu, int value) {
                                                pdu.pusch_maintenance_v3.ldpc_base_graph =
                                                    static_cast<ldpc_base_graph_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(
    part2_size_map,
    validate_pusch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<ul_pusch_pdu>{
                         "Part2 size map scope",
                         [](ul_pusch_pdu& pdu, int value) {
                           pdu.uci_correspondence.part2.back().part2_size_map_scope =
                               static_cast<uci_part1_to_part2_correspondence_v3::map_scope_type>(value);
                         }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

/// Valid PDU should pass.
TEST(validate_pusch_pdu, valid_pdu_passes)
{
  validator_report report(0, 0);
  const auto&      pdu = build_valid_ul_pusch_pdu();
  EXPECT_TRUE(validate_ul_pusch_pdu(pdu, report));
  // Assert no reports were generated.
  EXPECT_TRUE(report.reports.empty());
}

/// Add 3 errors and check that validation fails with 3 errors.
TEST(validate_pusch_pdu, invalid_pdu_fails)
{
  validator_report report(0, 0);
  auto             pdu = build_valid_ul_pusch_pdu();

  // Add 4 errors.
  pdu.bwp_start      = 6500;
  pdu.bwp_size       = 2133;
  pdu.nr_of_symbols  = 32;
  pdu.resource_alloc = static_cast<resource_allocation_type>(54);

  EXPECT_FALSE(validate_ul_pusch_pdu(pdu, report));
  ASSERT_EQ(4U, report.reports.size());
  // Check that the properties that caused the error are different.
  for (unsigned i = 0, e = report.reports.size(); i != e; ++i) {
    for (unsigned j = i + 1; j != e; ++j) {
      EXPECT_TRUE(std::strcmp(report.reports[i].property_name, report.reports[j].property_name) != 0);
    }
  }
}
