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

// This file was generated using the following MATLAB class on 07-07-2023:
//   + "srsChEstimatorUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/signal_processors/port_channel_estimator.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  port_channel_estimator::configuration                   cfg;
  unsigned                                                grid_size_prbs = 0;
  float                                                   rsrp           = 0;
  float                                                   epre           = 0;
  float                                                   snr_true       = 0;
  float                                                   snr_est        = 0;
  float                                                   noise_var_est  = 0;
  float                                                   ta_us          = 0;
  file_vector<resource_grid_reader_spy::expected_entry_t> grid;
  file_vector<cf_t>                                       pilots;
  file_vector<resource_grid_reader_spy::expected_entry_t> estimates;
};

static const std::vector<test_case_t> port_channel_estimator_test_data = {
    // clang-format off
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.0064, 2.0276, 20, 17.5726, 0.017585, 0.8138, {"test_data/port_channel_estimator_test_input_rg0.dat"}, {"test_data/port_channel_estimator_test_pilots0.dat"}, {"test_data/port_channel_estimator_test_output_ch_est0.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 1.9833, 1.994, 20, 20.532, 0.0087938, 0.11393, {"test_data/port_channel_estimator_test_input_rg1.dat"}, {"test_data/port_channel_estimator_test_pilots1.dat"}, {"test_data/port_channel_estimator_test_output_ch_est1.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 1.9981, 2.0041, 20, 20.0625, 0.0098711, 0.21159, {"test_data/port_channel_estimator_test_input_rg2.dat"}, {"test_data/port_channel_estimator_test_pilots2.dat"}, {"test_data/port_channel_estimator_test_output_ch_est2.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.0265, 2.0049, 20, 19.7153, 0.010845, 0.86263, {"test_data/port_channel_estimator_test_input_rg3.dat"}, {"test_data/port_channel_estimator_test_pilots3.dat"}, {"test_data/port_channel_estimator_test_output_ch_est3.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.0143, 2.0047, 20, 20.0703, 0.0099333, 0.53711, {"test_data/port_channel_estimator_test_input_rg4.dat"}, {"test_data/port_channel_estimator_test_pilots4.dat"}, {"test_data/port_channel_estimator_test_output_ch_est4.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 8, 4, {{{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 1.05, 1.0903, 20, 17.8753, 0.017125, 0.39062, {"test_data/port_channel_estimator_test_input_rg5.dat"}, {"test_data/port_channel_estimator_test_pilots5.dat"}, {"test_data/port_channel_estimator_test_output_ch_est5.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 8, 4, {{{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 10, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 0.99021, 1.0626, 20, 15.0438, 0.030999, 0.66732, {"test_data/port_channel_estimator_test_input_rg6.dat"}, {"test_data/port_channel_estimator_test_pilots6.dat"}, {"test_data/port_channel_estimator_test_output_ch_est6.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 0.95628, 1.0211, 20, 15.2485, 0.028559, 0.66732, {"test_data/port_channel_estimator_test_input_rg7.dat"}, {"test_data/port_channel_estimator_test_pilots7.dat"}, {"test_data/port_channel_estimator_test_output_ch_est7.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 0.97925, 1.0234, 20, 16.3413, 0.022739, 0.52083, {"test_data/port_channel_estimator_test_input_rg8.dat"}, {"test_data/port_channel_estimator_test_pilots8.dat"}, {"test_data/port_channel_estimator_test_output_ch_est8.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 2, {{{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.90784, 0.91875, 20, 19.1682, 0.010995, 0.35807, {"test_data/port_channel_estimator_test_input_rg9.dat"}, {"test_data/port_channel_estimator_test_pilots9.dat"}, {"test_data/port_channel_estimator_test_output_ch_est9.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 2, {{{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 1, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.98147, 0.98732, 20, 23.8609, 0.0040345, 0.13835, {"test_data/port_channel_estimator_test_input_rg10.dat"}, {"test_data/port_channel_estimator_test_pilots10.dat"}, {"test_data/port_channel_estimator_test_output_ch_est10.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 1, {{{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.9545, 0.95118, 20, 20.5315, 0.0084455, 0.32552, {"test_data/port_channel_estimator_test_input_rg11.dat"}, {"test_data/port_channel_estimator_test_pilots11.dat"}, {"test_data/port_channel_estimator_test_output_ch_est11.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 2, {{{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0373, 1.0046, 20, 20.514, 0.0092152, 0.53711, {"test_data/port_channel_estimator_test_input_rg12.dat"}, {"test_data/port_channel_estimator_test_pilots12.dat"}, {"test_data/port_channel_estimator_test_output_ch_est12.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 2, {{{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 6, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0078, 1.008, 20, 21.9349, 0.0064548, 0.21159, {"test_data/port_channel_estimator_test_input_rg13.dat"}, {"test_data/port_channel_estimator_test_pilots13.dat"}, {"test_data/port_channel_estimator_test_output_ch_est13.dat"}},
    // clang-format on
};

} // namespace srsran
