/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

// This file was generated using the following MATLAB class on 09-Sep-2022:
//   + "srsPUSCHProcessorUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pusch_processor.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

using rg_entry = resource_grid_reader_spy::expected_entry_t;

struct fix_reference_channel_description {
  std::string fix_reference_channel;
  unsigned    channel_bandwidth_MHz;
};

struct fix_reference_channel_slot {
  pusch_processor::pdu_t config;
  file_vector<rg_entry>  data;
  file_vector<rg_entry>  dmrs;
  file_vector<uint8_t>   sch_data;
};

struct test_case_t {
  fix_reference_channel_description       description;
  std::vector<fix_reference_channel_slot> slots;
};

static const std::vector<test_case_t> pusch_processor_test_data = {
    // clang-format off
{{"G-FR1-A3-8", 5}, {  {{{0, 0}, 1, 25, 0, cyclic_prefix::NORMAL, modulation_scheme::QPSK, {{0, ldpc_base_graph_type::BG2, true}}, {}, 0, 1, {0}, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 0, 0, 2, rb_allocation::make_type1(0, 25), 0, 14, ldpc::MAX_CODEBLOCK_SIZE / 8}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_data_symbols0.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_dmrs_symbols0.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_transport_block0.dat"}},   {{{0, 1}, 1, 25, 0, cyclic_prefix::NORMAL, modulation_scheme::QPSK, {{0, ldpc_base_graph_type::BG2, true}}, {}, 0, 1, {0}, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 0, 0, 2, rb_allocation::make_type1(0, 25), 0, 14, ldpc::MAX_CODEBLOCK_SIZE / 8}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_data_symbols1.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_dmrs_symbols1.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_transport_block1.dat"}},   {{{0, 2}, 1, 25, 0, cyclic_prefix::NORMAL, modulation_scheme::QPSK, {{0, ldpc_base_graph_type::BG2, true}}, {}, 0, 1, {0}, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 0, 0, 2, rb_allocation::make_type1(0, 25), 0, 14, ldpc::MAX_CODEBLOCK_SIZE / 8}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_data_symbols2.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_dmrs_symbols2.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_transport_block2.dat"}},   {{{0, 3}, 1, 25, 0, cyclic_prefix::NORMAL, modulation_scheme::QPSK, {{0, ldpc_base_graph_type::BG2, true}}, {}, 0, 1, {0}, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 0, 0, 2, rb_allocation::make_type1(0, 25), 0, 14, ldpc::MAX_CODEBLOCK_SIZE / 8}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_data_symbols3.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_dmrs_symbols3.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_transport_block3.dat"}},   {{{0, 4}, 1, 25, 0, cyclic_prefix::NORMAL, modulation_scheme::QPSK, {{0, ldpc_base_graph_type::BG2, true}}, {}, 0, 1, {0}, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 0, 0, 2, rb_allocation::make_type1(0, 25), 0, 14, ldpc::MAX_CODEBLOCK_SIZE / 8}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_data_symbols4.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_dmrs_symbols4.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_transport_block4.dat"}},   {{{0, 5}, 1, 25, 0, cyclic_prefix::NORMAL, modulation_scheme::QPSK, {{0, ldpc_base_graph_type::BG2, true}}, {}, 0, 1, {0}, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 0, 0, 2, rb_allocation::make_type1(0, 25), 0, 14, ldpc::MAX_CODEBLOCK_SIZE / 8}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_data_symbols5.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_dmrs_symbols5.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_transport_block5.dat"}},   {{{0, 6}, 1, 25, 0, cyclic_prefix::NORMAL, modulation_scheme::QPSK, {{0, ldpc_base_graph_type::BG2, true}}, {}, 0, 1, {0}, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 0, 0, 2, rb_allocation::make_type1(0, 25), 0, 14, ldpc::MAX_CODEBLOCK_SIZE / 8}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_data_symbols6.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_dmrs_symbols6.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_transport_block6.dat"}},   {{{0, 7}, 1, 25, 0, cyclic_prefix::NORMAL, modulation_scheme::QPSK, {{0, ldpc_base_graph_type::BG2, true}}, {}, 0, 1, {0}, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 0, 0, 2, rb_allocation::make_type1(0, 25), 0, 14, ldpc::MAX_CODEBLOCK_SIZE / 8}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_data_symbols7.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_dmrs_symbols7.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_transport_block7.dat"}},   {{{0, 8}, 1, 25, 0, cyclic_prefix::NORMAL, modulation_scheme::QPSK, {{0, ldpc_base_graph_type::BG2, true}}, {}, 0, 1, {0}, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 0, 0, 2, rb_allocation::make_type1(0, 25), 0, 14, ldpc::MAX_CODEBLOCK_SIZE / 8}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_data_symbols8.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_dmrs_symbols8.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_transport_block8.dat"}},   {{{0, 9}, 1, 25, 0, cyclic_prefix::NORMAL, modulation_scheme::QPSK, {{0, ldpc_base_graph_type::BG2, true}}, {}, 0, 1, {0}, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 0, 0, 2, rb_allocation::make_type1(0, 25), 0, 14, ldpc::MAX_CODEBLOCK_SIZE / 8}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_data_symbols9.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_pdsch_dmrs_symbols9.dat"}, {"test_data/pusch_processor_G-FR1-A3-8_5_transport_block9.dat"}}}},
    // clang-format on
};

} // namespace srsgnb
