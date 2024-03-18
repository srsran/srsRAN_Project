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

#pragma once

// This file was generated using the following MATLAB class on 05-03-2024 (seed 0):
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
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.0142, 2.0226, 20, 20.3665, 0.0092779, 0, {"test_data/port_channel_estimator_test_input_rg0.dat"}, {"test_data/port_channel_estimator_test_pilots0.dat"}, {"test_data/port_channel_estimator_test_output_ch_est0.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 1.974, 1.98, 20, 20.4101, 0.009002, 0.12207, {"test_data/port_channel_estimator_test_input_rg1.dat"}, {"test_data/port_channel_estimator_test_pilots1.dat"}, {"test_data/port_channel_estimator_test_output_ch_est1.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.024, 1.9989, 20, 20.5577, 0.0089217, 0.7487, {"test_data/port_channel_estimator_test_input_rg2.dat"}, {"test_data/port_channel_estimator_test_pilots2.dat"}, {"test_data/port_channel_estimator_test_output_ch_est2.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.0196, 2.0086, 20, 20.2465, 0.0095636, 0.53711, {"test_data/port_channel_estimator_test_input_rg3.dat"}, {"test_data/port_channel_estimator_test_pilots3.dat"}, {"test_data/port_channel_estimator_test_output_ch_est3.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.0127, 2.0016, 20, 20.2558, 0.0095102, 0.53711, {"test_data/port_channel_estimator_test_input_rg4.dat"}, {"test_data/port_channel_estimator_test_pilots4.dat"}, {"test_data/port_channel_estimator_test_output_ch_est4.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 8, 4, {{{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 0.97, 1.0151, 20, 20.655, 0.008342, 0.66732, {"test_data/port_channel_estimator_test_input_rg5.dat"}, {"test_data/port_channel_estimator_test_pilots5.dat"}, {"test_data/port_channel_estimator_test_output_ch_est5.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 8, 4, {{{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 10, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 0.87208, 0.96164, 20, 18.8145, 0.011458, 1.0498, {"test_data/port_channel_estimator_test_input_rg6.dat"}, {"test_data/port_channel_estimator_test_pilots6.dat"}, {"test_data/port_channel_estimator_test_output_ch_est6.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 0.91172, 0.99641, 20, 19.4351, 0.010384, 1.0254, {"test_data/port_channel_estimator_test_input_rg7.dat"}, {"test_data/port_channel_estimator_test_pilots7.dat"}, {"test_data/port_channel_estimator_test_output_ch_est7.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 0.9395, 1.01, 20, 18.6912, 0.012699, 0.86263, {"test_data/port_channel_estimator_test_input_rg8.dat"}, {"test_data/port_channel_estimator_test_pilots8.dat"}, {"test_data/port_channel_estimator_test_output_ch_est8.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 2, {{{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.98011, 1.0529, 20, 18.8057, 0.012903, 1.0417, {"test_data/port_channel_estimator_test_input_rg9.dat"}, {"test_data/port_channel_estimator_test_pilots9.dat"}, {"test_data/port_channel_estimator_test_output_ch_est9.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 2, {{{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 1, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0321, 1.0681, 20, 22.8276, 0.0053822, 0.52083, {"test_data/port_channel_estimator_test_input_rg10.dat"}, {"test_data/port_channel_estimator_test_pilots10.dat"}, {"test_data/port_channel_estimator_test_output_ch_est10.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 1, {{{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0258, 0.98303, 20, 23.3508, 0.0047421, 0.53711, {"test_data/port_channel_estimator_test_input_rg11.dat"}, {"test_data/port_channel_estimator_test_pilots11.dat"}, {"test_data/port_channel_estimator_test_output_ch_est11.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 2, {{{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0101, 1.01, 20, 20.8373, 0.0083301, 0.21159, {"test_data/port_channel_estimator_test_input_rg12.dat"}, {"test_data/port_channel_estimator_test_pilots12.dat"}, {"test_data/port_channel_estimator_test_output_ch_est12.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 2, {{{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 6, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.1507, 1.0308, 20, 20.6124, 0.0099939, 0.96029, {"test_data/port_channel_estimator_test_input_rg13.dat"}, {"test_data/port_channel_estimator_test_pilots13.dat"}, {"test_data/port_channel_estimator_test_output_ch_est13.dat"}},
    // clang-format on
};

} // namespace srsran
