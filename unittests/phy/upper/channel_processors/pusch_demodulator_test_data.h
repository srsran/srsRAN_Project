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

// This file was generated using the following MATLAB class on 23-Sep-2022:
//   + "srsPUSCHDemodulatorUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pusch_demodulator.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct context_t {
  float                            noise_var;
  pusch_demodulator::configuration config;
};

struct test_case_t {
  context_t                                               context;
  file_vector<resource_grid_reader_spy::expected_entry_t> symbols;
  file_vector<cf_t>                                       estimates;
  file_vector<log_likelihood_ratio>                       sch_data;
  file_vector<log_likelihood_ratio>                       harq_ack;
  file_vector<log_likelihood_ratio>                       csi_part1;
  file_vector<log_likelihood_ratio>                       csi_part2;
};

static const std::vector<test_case_t> pusch_demodulator_test_data = {
    // clang-format off
  {{0.36, {1, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, modulation_scheme::QPSK, 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 2, 0, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols0.dat"}, {"test_data/pusch_demodulator_test_input_estimates0.dat"}, {"test_data/pusch_demodulator_test_output_sch_soft_bits0.dat"}, {"test_data/pusch_demodulator_test_output_harq_ack_soft_bits0.dat"}, {"test_data/pusch_demodulator_test_output_csi_part1_soft_bits0.dat"}, {"test_data/pusch_demodulator_test_output_csi_part2_soft_bits0.dat"}},
  {{0.04, {1, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, modulation_scheme::QPSK, 1, 13, {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 2, 0, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols2.dat"}, {"test_data/pusch_demodulator_test_input_estimates2.dat"}, {"test_data/pusch_demodulator_test_output_sch_soft_bits2.dat"}, {"test_data/pusch_demodulator_test_output_harq_ack_soft_bits2.dat"}, {"test_data/pusch_demodulator_test_output_csi_part1_soft_bits2.dat"}, {"test_data/pusch_demodulator_test_output_csi_part2_soft_bits2.dat"}},
  {{0.16, {1, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, modulation_scheme::QPSK, 2, 10, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, dmrs_type::TYPE1, 2, 0, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols4.dat"}, {"test_data/pusch_demodulator_test_input_estimates4.dat"}, {"test_data/pusch_demodulator_test_output_sch_soft_bits4.dat"}, {"test_data/pusch_demodulator_test_output_harq_ack_soft_bits4.dat"}, {"test_data/pusch_demodulator_test_output_csi_part1_soft_bits4.dat"}, {"test_data/pusch_demodulator_test_output_csi_part2_soft_bits4.dat"}},
  {{0.64, {1, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, modulation_scheme::QAM16, 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 2, 0, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols6.dat"}, {"test_data/pusch_demodulator_test_input_estimates6.dat"}, {"test_data/pusch_demodulator_test_output_sch_soft_bits6.dat"}, {"test_data/pusch_demodulator_test_output_harq_ack_soft_bits6.dat"}, {"test_data/pusch_demodulator_test_output_csi_part1_soft_bits6.dat"}, {"test_data/pusch_demodulator_test_output_csi_part2_soft_bits6.dat"}},
  {{0.04, {1, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, modulation_scheme::QAM16, 1, 13, {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 2, 0, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols8.dat"}, {"test_data/pusch_demodulator_test_input_estimates8.dat"}, {"test_data/pusch_demodulator_test_output_sch_soft_bits8.dat"}, {"test_data/pusch_demodulator_test_output_harq_ack_soft_bits8.dat"}, {"test_data/pusch_demodulator_test_output_csi_part1_soft_bits8.dat"}, {"test_data/pusch_demodulator_test_output_csi_part2_soft_bits8.dat"}},
  {{0.49, {1, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, modulation_scheme::QAM16, 2, 10, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, dmrs_type::TYPE1, 2, 0, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols10.dat"}, {"test_data/pusch_demodulator_test_input_estimates10.dat"}, {"test_data/pusch_demodulator_test_output_sch_soft_bits10.dat"}, {"test_data/pusch_demodulator_test_output_harq_ack_soft_bits10.dat"}, {"test_data/pusch_demodulator_test_output_csi_part1_soft_bits10.dat"}, {"test_data/pusch_demodulator_test_output_csi_part2_soft_bits10.dat"}},
  {{0.09, {1, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, modulation_scheme::QAM64, 0, 14, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 2, 0, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols12.dat"}, {"test_data/pusch_demodulator_test_input_estimates12.dat"}, {"test_data/pusch_demodulator_test_output_sch_soft_bits12.dat"}, {"test_data/pusch_demodulator_test_output_harq_ack_soft_bits12.dat"}, {"test_data/pusch_demodulator_test_output_csi_part1_soft_bits12.dat"}, {"test_data/pusch_demodulator_test_output_csi_part2_soft_bits12.dat"}},
  {{0.81, {1, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, modulation_scheme::QAM64, 1, 13, {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 2, 0, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols14.dat"}, {"test_data/pusch_demodulator_test_input_estimates14.dat"}, {"test_data/pusch_demodulator_test_output_sch_soft_bits14.dat"}, {"test_data/pusch_demodulator_test_output_harq_ack_soft_bits14.dat"}, {"test_data/pusch_demodulator_test_output_csi_part1_soft_bits14.dat"}, {"test_data/pusch_demodulator_test_output_csi_part2_soft_bits14.dat"}},
  {{0.16, {1, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, modulation_scheme::QAM64, 2, 10, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0}, dmrs_type::TYPE1, 2, 0, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols16.dat"}, {"test_data/pusch_demodulator_test_input_estimates16.dat"}, {"test_data/pusch_demodulator_test_output_sch_soft_bits16.dat"}, {"test_data/pusch_demodulator_test_output_harq_ack_soft_bits16.dat"}, {"test_data/pusch_demodulator_test_output_csi_part1_soft_bits16.dat"}, {"test_data/pusch_demodulator_test_output_csi_part2_soft_bits16.dat"}},
    // clang-format on
};

} // namespace srsgnb
