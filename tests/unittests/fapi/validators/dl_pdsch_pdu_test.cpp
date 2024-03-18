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

#include "../../../lib/fapi/validators/dl_pdsch_pdu.h"
#include "../message_builder_helpers.h"
#include "helpers.h"
#include "srsran/fapi/message_validators.h"

using namespace srsran;
using namespace fapi;
using namespace unittest;

class validate_pdsch_pdu_field : public validate_fapi_pdu<dl_pdsch_pdu, dl_pdu_type>,
                                 public testing::TestWithParam<std::tuple<pdu_field_data<dl_pdsch_pdu>, test_case_data>>
{};

TEST_P(validate_pdsch_pdu_field, WithValue)
{
  auto params = GetParam();

  execute_test(std::get<0>(params),
               std::get<1>(params),
               build_valid_dl_pdsch_pdu,
               validate_dl_pdsch_pdu,
               srsran::fapi::message_type_id::dl_tti_request,
               dl_pdu_type::PDSCH);
};

INSTANTIATE_TEST_SUITE_P(rnti,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "RNTI",
                                              [](dl_pdsch_pdu& pdu, int value) { pdu.rnti = to_rnti(value); }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{32676, true},
                                                          test_case_data{65535, true})));

INSTANTIATE_TEST_SUITE_P(bwp_size,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "BWP Size",
                                              [](dl_pdsch_pdu& pdu, int value) { pdu.bwp_size = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{128, true},
                                                          test_case_data{275, true},
                                                          test_case_data{276, false})));

INSTANTIATE_TEST_SUITE_P(bwp_start,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "BWP Start",
                                              [](dl_pdsch_pdu& pdu, int value) { pdu.bwp_start = value; }}),
                                          testing::Values(test_case_data{1, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(scs,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Subcarrier spacing",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.scs = to_subcarrier_spacing(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false})));

INSTANTIATE_TEST_SUITE_P(
    Cyclic_prefix,
    validate_pdsch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                         "Cyclic prefix",
                         [](dl_pdsch_pdu& pdu, int value) { pdu.cp = static_cast<cyclic_prefix::options>(value); }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(mod_order,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "QAM modulation order",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.cws.front().qam_mod_order = value;
                                              }}),
                                          testing::Values(test_case_data{1, false},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false},
                                                          test_case_data{6, true},
                                                          test_case_data{7, false},
                                                          test_case_data{8, true},
                                                          test_case_data{9, false})));

INSTANTIATE_TEST_SUITE_P(mcs_index,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "MCS Index",
                                              [](dl_pdsch_pdu& pdu, int value) { pdu.cws.front().mcs_index = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{16, true},
                                                          test_case_data{31, true},
                                                          test_case_data{32, false})));

INSTANTIATE_TEST_SUITE_P(mcs_table,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "MCS Table",
                                              [](dl_pdsch_pdu& pdu, int value) { pdu.cws.front().mcs_table = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(rv_index,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "RV Index",
                                              [](dl_pdsch_pdu& pdu, int value) { pdu.cws.front().rv_index = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(nid_pdsch,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "nID PDSCH",
                                              [](dl_pdsch_pdu& pdu, int value) { pdu.nid_pdsch = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{512, true},
                                                          test_case_data{1023, true},
                                                          test_case_data{1024, false})));

INSTANTIATE_TEST_SUITE_P(nof_layers,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Number of layers",
                                              [](dl_pdsch_pdu& pdu, int value) { pdu.num_layers = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{4, true},
                                                          test_case_data{8, true},
                                                          test_case_data{9, false})));

INSTANTIATE_TEST_SUITE_P(tx_scheme,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Trasnmission scheme",
                                              [](dl_pdsch_pdu& pdu, int value) { pdu.transmission_scheme = value; }}),
                                          testing::Values(test_case_data{0, true}, test_case_data{1, false})));

INSTANTIATE_TEST_SUITE_P(
    ref_point,
    validate_pdsch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{"Reference point",
                                                                  [](dl_pdsch_pdu& pdu, int value) {
                                                                    pdu.ref_point =
                                                                        static_cast<pdsch_ref_point_type>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(
    dmrs_config_type,
    validate_pdsch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                         "DMRS config type",
                         [](dl_pdsch_pdu& pdu, int value) { pdu.dmrs_type = static_cast<dmrs_cfg_type>(value); }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(
    low_papr_dmrs,
    validate_pdsch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{"Low PAPR DMRS",
                                                                  [](dl_pdsch_pdu& pdu, int value) {
                                                                    pdu.low_papr_dmrs =
                                                                        static_cast<low_papr_dmrs_type>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(
    nscid,
    validate_pdsch_pdu_field,
    testing::Combine(
        testing::Values(pdu_field_data<dl_pdsch_pdu>{"NSCID", [](dl_pdsch_pdu& pdu, int value) { pdu.nscid = value; }}),
        testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(dmrs_grp_no_data,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Number of DMRS CDM groups without data",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.num_dmrs_cdm_grps_no_data = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false})));

INSTANTIATE_TEST_SUITE_P(
    resource_alloc,
    validate_pdsch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{"Resource allocation",
                                                                  [](dl_pdsch_pdu& pdu, int value) {
                                                                    pdu.resource_alloc =
                                                                        static_cast<resource_allocation_type>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(rb_size,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "RB Size",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.resource_alloc = srsran::fapi::resource_allocation_type::type_1;
                                                pdu.rb_size        = value;
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{128, true},
                                                          test_case_data{275, true},
                                                          test_case_data{276, false})));

INSTANTIATE_TEST_SUITE_P(rb_start,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "RB Start",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.resource_alloc = srsran::fapi::resource_allocation_type::type_1;
                                                pdu.rb_start       = value;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{128, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(vrb_prb_mapping,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "VRB-to-PRB-mapping",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.vrb_to_prb_mapping = static_cast<vrb_to_prb_mapping_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(start_symbol,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Start symbol index",
                                              [](dl_pdsch_pdu& pdu, int value) { pdu.start_symbol_index = value; }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{7, true},
                                                          test_case_data{13, true},
                                                          test_case_data{14, false})));

INSTANTIATE_TEST_SUITE_P(nof_symbols,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "PDSCH duration in symbols",
                                              [](dl_pdsch_pdu& pdu, int value) { pdu.nr_of_symbols = value; }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{7, true},
                                                          test_case_data{14, true},
                                                          test_case_data{15, false})));

INSTANTIATE_TEST_SUITE_P(profile_nr,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Ratio PDSCH EPRE to NZP CSI-RS EPRE",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.power_control_offset_profile_nr = value;
                                              }}),
                                          testing::Values(test_case_data{unsigned(-9), false},
                                                          test_case_data{unsigned(-8), true},
                                                          test_case_data{0, true},
                                                          test_case_data{15, true},
                                                          test_case_data{16, false})));

INSTANTIATE_TEST_SUITE_P(offset_ss_profile_nr,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.power_control_offset_ss_profile_nr =
                                                    static_cast<power_control_offset_ss>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, true},
                                                          test_case_data{4, false},
                                                          test_case_data{255, false})));

INSTANTIATE_TEST_SUITE_P(
    is_inline_tb_crc,
    validate_pdsch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{"Is inline TB CRC",
                                                                  [](dl_pdsch_pdu& pdu, int value) {
                                                                    pdu.is_inline_tb_crc =
                                                                        static_cast<inline_tb_crc_type>(value);
                                                                  }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

INSTANTIATE_TEST_SUITE_P(trans_type,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "PDSCH transmission type",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.pdsch_maintenance_v3.trans_type =
                                                    static_cast<pdsch_trans_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{2, true},
                                                          test_case_data{5, true},
                                                          test_case_data{6, false})));

INSTANTIATE_TEST_SUITE_P(coreset_start,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "CORESET start point",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.pdsch_maintenance_v3.coreset_start_point = value;
                                                pdu.pdsch_maintenance_v3.trans_type =
                                                    srsran::fapi::pdsch_trans_type::non_interleaved_common_ss;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{135, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(coreset_start_1,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "CORESET start point",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.pdsch_maintenance_v3.coreset_start_point = value;
                                                pdu.pdsch_maintenance_v3.trans_type =
                                                    srsran::fapi::pdsch_trans_type::non_interleaved_other;
                                              }}),
                                          testing::Values(test_case_data{0, true}, test_case_data{275, true})));

INSTANTIATE_TEST_SUITE_P(coreset_start_2,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "CORESET start point",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.pdsch_maintenance_v3.coreset_start_point = value;
                                                pdu.pdsch_maintenance_v3.trans_type =
                                                    srsran::fapi::pdsch_trans_type::interleaved_common_type0_coreset0;
                                              }}),
                                          testing::Values(test_case_data{0, true}, test_case_data{275, true})));

INSTANTIATE_TEST_SUITE_P(
    coreset_start_3,
    validate_pdsch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                         "CORESET start point",
                         [](dl_pdsch_pdu& pdu, int value) {
                           pdu.pdsch_maintenance_v3.coreset_start_point = value;
                           pdu.pdsch_maintenance_v3.trans_type =
                               srsran::fapi::pdsch_trans_type::interleaved_common_any_coreset0_present;
                         }}),
                     testing::Values(test_case_data{0, true},
                                     test_case_data{135, true},
                                     test_case_data{274, true},
                                     test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(
    coreset_start_4,
    validate_pdsch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                         "CORESET start point",
                         [](dl_pdsch_pdu& pdu, int value) {
                           pdu.pdsch_maintenance_v3.coreset_start_point = value;
                           pdu.pdsch_maintenance_v3.trans_type =
                               srsran::fapi::pdsch_trans_type::interleaved_common_any_coreset0_not_present;
                         }}),
                     testing::Values(test_case_data{0, true},
                                     test_case_data{135, true},
                                     test_case_data{274, true},
                                     test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(coreset_start_5,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "CORESET start point",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.pdsch_maintenance_v3.coreset_start_point = value;
                                                pdu.pdsch_maintenance_v3.trans_type =
                                                    srsran::fapi::pdsch_trans_type::interleaved_other;
                                              }}),
                                          testing::Values(test_case_data{0, true}, test_case_data{275, true})));

INSTANTIATE_TEST_SUITE_P(initial_dl_bwp_0,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Initial DL BWP size",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.pdsch_maintenance_v3.initial_dl_bwp_size = value;
                                                pdu.pdsch_maintenance_v3.trans_type =
                                                    srsran::fapi::pdsch_trans_type::non_interleaved_common_ss;
                                              }}),
                                          testing::Values(test_case_data{0, true},
                                                          test_case_data{135, true},
                                                          test_case_data{274, true},
                                                          test_case_data{275, true})));

INSTANTIATE_TEST_SUITE_P(initial_dl_bwp_1,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Initial DL BWP size",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.pdsch_maintenance_v3.initial_dl_bwp_size = value;
                                                pdu.pdsch_maintenance_v3.trans_type =
                                                    srsran::fapi::pdsch_trans_type::non_interleaved_other;
                                              }}),
                                          testing::Values(test_case_data{0, true}, test_case_data{275, true})));

INSTANTIATE_TEST_SUITE_P(initial_dl_bwp_2,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Initial DL BWP size",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.pdsch_maintenance_v3.initial_dl_bwp_size = value;
                                                pdu.pdsch_maintenance_v3.trans_type =
                                                    srsran::fapi::pdsch_trans_type::interleaved_common_type0_coreset0;
                                              }}),
                                          testing::Values(test_case_data{0, true}, test_case_data{275, true})));

INSTANTIATE_TEST_SUITE_P(
    initial_dl_bwp_3,
    validate_pdsch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                         "Initial DL BWP size",
                         [](dl_pdsch_pdu& pdu, int value) {
                           pdu.pdsch_maintenance_v3.initial_dl_bwp_size = value;
                           pdu.pdsch_maintenance_v3.trans_type =
                               srsran::fapi::pdsch_trans_type::interleaved_common_any_coreset0_present;
                         }}),
                     testing::Values(test_case_data{0, true},
                                     test_case_data{135, true},
                                     test_case_data{274, true},
                                     test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(
    initial_dl_bwp_4,
    validate_pdsch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                         "Initial DL BWP size",
                         [](dl_pdsch_pdu& pdu, int value) {
                           pdu.pdsch_maintenance_v3.initial_dl_bwp_size = value;
                           pdu.pdsch_maintenance_v3.trans_type =
                               srsran::fapi::pdsch_trans_type::interleaved_common_any_coreset0_not_present;
                         }}),
                     testing::Values(test_case_data{0, true},
                                     test_case_data{135, true},
                                     test_case_data{274, true},
                                     test_case_data{275, false})));

INSTANTIATE_TEST_SUITE_P(initial_dl_bwp_5,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Initial DL BWP size",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.pdsch_maintenance_v3.initial_dl_bwp_size = value;
                                                pdu.pdsch_maintenance_v3.trans_type =
                                                    srsran::fapi::pdsch_trans_type::interleaved_other;
                                              }}),
                                          testing::Values(test_case_data{0, true}, test_case_data{275, true})));

INSTANTIATE_TEST_SUITE_P(ldpc_base_graph,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "LDPC base graph",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.pdsch_maintenance_v3.ldpc_base_graph =
                                                    static_cast<ldpc_base_graph_type>(value);
                                              }}),
                                          testing::Values(test_case_data{0, false},
                                                          test_case_data{1, true},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false})));

INSTANTIATE_TEST_SUITE_P(max_cbg_per_tb,
                         validate_pdsch_pdu_field,
                         testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                                              "Maximum number CBG per TB",
                                              [](dl_pdsch_pdu& pdu, int value) {
                                                pdu.pdsch_maintenance_v3.max_num_cbg_per_tb = value;
                                              }}),
                                          testing::Values(test_case_data{1, false},
                                                          test_case_data{2, true},
                                                          test_case_data{3, false},
                                                          test_case_data{4, true},
                                                          test_case_data{5, false},
                                                          test_case_data{6, true},
                                                          test_case_data{7, false},
                                                          test_case_data{8, true},
                                                          test_case_data{9, false})));

INSTANTIATE_TEST_SUITE_P(
    lte_crs_mbsfn_derivation,
    validate_pdsch_pdu_field,
    testing::Combine(testing::Values(pdu_field_data<dl_pdsch_pdu>{
                         "LTE CRS MBSFN derivation method",
                         [](dl_pdsch_pdu& pdu, int value) {
                           pdu.pdsch_parameters_v4.lte_crs_mbsfn_derivation_method = value;
                         }}),
                     testing::Values(test_case_data{0, true}, test_case_data{1, true}, test_case_data{2, false})));

/// Valid PDU should pass.
TEST(validate_pdsch_pdu, valid_pdu_passes)
{
  dl_pdsch_pdu pdu = build_valid_dl_pdsch_pdu();

  validator_report report(0, 0);
  EXPECT_TRUE(validate_dl_pdsch_pdu(pdu, report));
  // Assert no reports were generated.
  EXPECT_TRUE(report.reports.empty());
}

/// Add 3 errors and check that validation fails with 3 errors.
TEST(validate_pdsch_pdu, invalid_pdu_fails)
{
  dl_pdsch_pdu pdu = build_valid_dl_pdsch_pdu();

  // Force 3 errors.
  pdu.bwp_size                        = 2690;
  pdu.power_control_offset_profile_nr = 255;
  pdu.cws[0].qam_mod_order            = 1;
  validator_report report(0, 0);
  EXPECT_FALSE(validate_dl_pdsch_pdu(pdu, report));
  // Assert 3 reports were generated.
  EXPECT_EQ(report.reports.size(), 3u);
}
