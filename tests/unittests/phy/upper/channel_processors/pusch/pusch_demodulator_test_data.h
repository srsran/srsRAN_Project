/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

// This file was generated using the following MATLAB class on 14-09-2023 (seed 0):
//   + "srsPUSCHDemodulatorUnittest.m"

#include "../../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_demodulator.h"
#include "srsran/support/file_tensor.h"

namespace srsran {

enum class ch_dims : unsigned { subcarrier = 0, symbol = 1, rx_port = 2, tx_layer = 3, nof_dims = 4 };

struct context_t {
  float                            noise_var;
  float                            sinr_dB;
  pusch_demodulator::configuration config;
};

struct test_case_t {
  context_t                                                            context;
  file_vector<resource_grid_reader_spy::expected_entry_t>              symbols;
  file_tensor<static_cast<unsigned>(ch_dims::nof_dims), cf_t, ch_dims> estimates;
  file_vector<uint8_t>                                                 scrambling_seq;
  file_vector<log_likelihood_ratio>                                    codeword;
};

static const std::vector<test_case_t> pusch_demodulator_test_data = {
    // clang-format off
  {{0.0016124, 30.3961, {41442, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, modulation_scheme::PI_2_BPSK, 2, 7, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 821, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols0.dat"}, {"test_data/pusch_demodulator_test_input_estimates0.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq0.dat"}, {"test_data/pusch_demodulator_test_output0.dat"}},
  {{0.0014186, 28.6765, {43699, {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::PI_2_BPSK, 1, 13, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 1, 995, 1, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols2.dat"}, {"test_data/pusch_demodulator_test_input_estimates2.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq2.dat"}, {"test_data/pusch_demodulator_test_output2.dat"}},
  {{0.00046207, 34.0166, {49651, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, modulation_scheme::PI_2_BPSK, 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 475, 1, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols4.dat"}, {"test_data/pusch_demodulator_test_input_estimates4.dat", {300, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq4.dat"}, {"test_data/pusch_demodulator_test_output4.dat"}},
  {{0.032172, 17.1321, {50669, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QPSK, 2, 10, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 206, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols6.dat"}, {"test_data/pusch_demodulator_test_input_estimates6.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq6.dat"}, {"test_data/pusch_demodulator_test_output6.dat"}},
  {{0.00056474, 32.9903, {14432, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, modulation_scheme::QPSK, 0, 9, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 484, 1, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols8.dat"}, {"test_data/pusch_demodulator_test_input_estimates8.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq8.dat"}, {"test_data/pusch_demodulator_test_output8.dat"}},
  {{0.0029155, 25.8468, {47998, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, modulation_scheme::QPSK, 1, 9, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 611, 1, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols10.dat"}, {"test_data/pusch_demodulator_test_input_estimates10.dat", {300, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq10.dat"}, {"test_data/pusch_demodulator_test_output10.dat"}},
  {{0.032172, 16.9952, {63078, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM16, 0, 9, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 184, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols12.dat"}, {"test_data/pusch_demodulator_test_input_estimates12.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq12.dat"}, {"test_data/pusch_demodulator_test_output12.dat"}},
  {{0.0056474, 23.3358, {18480, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM16, 2, 10, {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 335, 1, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols14.dat"}, {"test_data/pusch_demodulator_test_input_estimates14.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq14.dat"}, {"test_data/pusch_demodulator_test_output14.dat"}},
  {{0.0029155, 26.3812, {5596, {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM16, 2, 10, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 951, 1, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols16.dat"}, {"test_data/pusch_demodulator_test_input_estimates16.dat", {300, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq16.dat"}, {"test_data/pusch_demodulator_test_output16.dat"}},
  {{0.012808, 20.8851, {38266, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM64, 1, 7, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 304, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols18.dat"}, {"test_data/pusch_demodulator_test_input_estimates18.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq18.dat"}, {"test_data/pusch_demodulator_test_output18.dat"}},
  {{0.035633, 15.2493, {54651, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM64, 2, 9, {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 821, 1, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols20.dat"}, {"test_data/pusch_demodulator_test_input_estimates20.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq20.dat"}, {"test_data/pusch_demodulator_test_output20.dat"}},
  {{0.0011607, 30.3105, {58220, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM64, 0, 11, {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 469, 1, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols22.dat"}, {"test_data/pusch_demodulator_test_input_estimates22.dat", {300, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq22.dat"}, {"test_data/pusch_demodulator_test_output22.dat"}},
  {{0.05099, 15.433, {14184, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, modulation_scheme::QAM256, 2, 8, {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE1, 2, 166, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols24.dat"}, {"test_data/pusch_demodulator_test_input_estimates24.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq24.dat"}, {"test_data/pusch_demodulator_test_output24.dat"}},
  {{0.0035633, 25.3758, {23990, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM256, 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE1, 1, 589, 1, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols26.dat"}, {"test_data/pusch_demodulator_test_input_estimates26.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq26.dat"}, {"test_data/pusch_demodulator_test_output26.dat"}},
  {{0.0014612, 29.3542, {37893, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM256, 2, 9, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE1, 1, 224, 1, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols28.dat"}, {"test_data/pusch_demodulator_test_input_estimates28.dat", {300, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq28.dat"}, {"test_data/pusch_demodulator_test_output28.dat"}},
  {{0.0020299, 29.0053, {10406, {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::PI_2_BPSK, 2, 7, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 3, 437, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols30.dat"}, {"test_data/pusch_demodulator_test_input_estimates30.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq30.dat"}, {"test_data/pusch_demodulator_test_output30.dat"}},
  {{0.0071097, 22.1221, {27995, {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::PI_2_BPSK, 0, 13, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 2, 313, 1, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols32.dat"}, {"test_data/pusch_demodulator_test_input_estimates32.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq32.dat"}, {"test_data/pusch_demodulator_test_output32.dat"}},
  {{0.00036704, 35.4029, {44574, {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::PI_2_BPSK, 2, 11, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 1, 994, 1, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols34.dat"}, {"test_data/pusch_demodulator_test_input_estimates34.dat", {300, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq34.dat"}, {"test_data/pusch_demodulator_test_output34.dat"}},
  {{0.010174, 22.235, {63459, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0}, modulation_scheme::QPSK, 2, 11, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 3, 809, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols36.dat"}, {"test_data/pusch_demodulator_test_input_estimates36.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq36.dat"}, {"test_data/pusch_demodulator_test_output36.dat"}},
  {{0.00089506, 31.0998, {9334, {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QPSK, 0, 13, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 3, 264, 1, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols38.dat"}, {"test_data/pusch_demodulator_test_input_estimates38.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq38.dat"}, {"test_data/pusch_demodulator_test_output38.dat"}},
  {{0.0092196, 21.2482, {12298, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QPSK, 2, 7, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 1, 217, 1, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols40.dat"}, {"test_data/pusch_demodulator_test_input_estimates40.dat", {300, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq40.dat"}, {"test_data/pusch_demodulator_test_output40.dat"}},
  {{0.012808, 21.0703, {49380, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, modulation_scheme::QAM16, 0, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 1, 228, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols42.dat"}, {"test_data/pusch_demodulator_test_input_estimates42.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq42.dat"}, {"test_data/pusch_demodulator_test_output42.dat"}},
  {{0.0056474, 23.1284, {47900, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM16, 2, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 1, 488, 1, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols44.dat"}, {"test_data/pusch_demodulator_test_input_estimates44.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq44.dat"}, {"test_data/pusch_demodulator_test_output44.dat"}},
  {{0.018395, 18.2965, {847, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM16, 0, 12, {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE2, 3, 484, 1, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols46.dat"}, {"test_data/pusch_demodulator_test_input_estimates46.dat", {300, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq46.dat"}, {"test_data/pusch_demodulator_test_output46.dat"}},
  {{0.0020299, 29.3803, {10041, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM64, 2, 12, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 2, 342, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols48.dat"}, {"test_data/pusch_demodulator_test_input_estimates48.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq48.dat"}, {"test_data/pusch_demodulator_test_output48.dat"}},
  {{0.0089506, 21.446, {35207, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM64, 2, 11, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, dmrs_type::TYPE2, 3, 437, 1, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols50.dat"}, {"test_data/pusch_demodulator_test_input_estimates50.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq50.dat"}, {"test_data/pusch_demodulator_test_output50.dat"}},
  {{0.0014612, 29.1213, {43167, {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM64, 1, 7, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 1, 793, 1, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols52.dat"}, {"test_data/pusch_demodulator_test_input_estimates52.dat", {300, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq52.dat"}, {"test_data/pusch_demodulator_test_output52.dat"}},
  {{0.12808, 10.6052, {56908, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM256, 1, 7, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, dmrs_type::TYPE2, 3, 359, 1, {0}}}, {"test_data/pusch_demodulator_test_input_symbols54.dat"}, {"test_data/pusch_demodulator_test_input_estimates54.dat", {300, 14, 1, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq54.dat"}, {"test_data/pusch_demodulator_test_output54.dat"}},
  {{0.0056474, 22.6921, {60782, {1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, modulation_scheme::QAM256, 2, 10, {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE2, 1, 800, 1, {0, 1}}}, {"test_data/pusch_demodulator_test_input_symbols56.dat"}, {"test_data/pusch_demodulator_test_input_estimates56.dat", {300, 14, 2, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq56.dat"}, {"test_data/pusch_demodulator_test_output56.dat"}},
  {{0.0092196, 20.8536, {30368, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, modulation_scheme::QAM256, 2, 8, {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0}, dmrs_type::TYPE2, 1, 403, 1, {0, 1, 2, 3}}}, {"test_data/pusch_demodulator_test_input_symbols58.dat"}, {"test_data/pusch_demodulator_test_input_estimates58.dat", {300, 14, 4, 1}}, {"test_data/pusch_demodulator_test_output_scrambling_seq58.dat"}, {"test_data/pusch_demodulator_test_output58.dat"}},
    // clang-format on
};

} // namespace srsran
