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

// This file was generated using the following MATLAB class on 31-01-2023:
//   + "srsChEstimatorUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/signal_processors/port_channel_estimator.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  port_channel_estimator::configuration                   cfg;
  unsigned                                                grid_size_prbs = 0;
  float                                                   rsrp           = 0;
  float                                                   epre           = 0;
  float                                                   snr_true       = 0;
  float                                                   snr_est        = 0;
  float                                                   noise_var_est  = 0;
  file_vector<resource_grid_reader_spy::expected_entry_t> grid;
  file_vector<cf_t>                                       pilots;
  file_vector<resource_grid_reader_spy::expected_entry_t> estimates;
};

static const std::vector<test_case_t> port_channel_estimator_test_data = {
    // clang-format off
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 1.9676, 0.98613, 20, 6.7998, 0.20604, {"test_data/port_channel_estimator_test_input_rg0.dat"}, {"test_data/port_channel_estimator_test_pilots0.dat"}, {"test_data/port_channel_estimator_test_output_ch_est0.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.0361, 1.0205, 20, 30, 0.0010205, {"test_data/port_channel_estimator_test_input_rg1.dat"}, {"test_data/port_channel_estimator_test_pilots1.dat"}, {"test_data/port_channel_estimator_test_output_ch_est1.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.019, 1.0119, 20, -0.38588, 1.1059, {"test_data/port_channel_estimator_test_input_rg2.dat"}, {"test_data/port_channel_estimator_test_pilots2.dat"}, {"test_data/port_channel_estimator_test_output_ch_est2.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 1.9863, 0.99549, 20, 30, 0.00099549, {"test_data/port_channel_estimator_test_input_rg3.dat"}, {"test_data/port_channel_estimator_test_pilots3.dat"}, {"test_data/port_channel_estimator_test_output_ch_est3.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 1.9848, 0.99474, 20, 10.1017, 0.097172, {"test_data/port_channel_estimator_test_input_rg4.dat"}, {"test_data/port_channel_estimator_test_pilots4.dat"}, {"test_data/port_channel_estimator_test_output_ch_est4.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 8, 4, {{{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 1.0364, 1.0364, 20, 30, 0.0010364, {"test_data/port_channel_estimator_test_input_rg5.dat"}, {"test_data/port_channel_estimator_test_pilots5.dat"}, {"test_data/port_channel_estimator_test_output_ch_est5.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 8, 4, {{{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 10, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 0.92835, 0.92835, 20, 30, 0.00092835, {"test_data/port_channel_estimator_test_input_rg6.dat"}, {"test_data/port_channel_estimator_test_pilots6.dat"}, {"test_data/port_channel_estimator_test_output_ch_est6.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 0.98371, 0.98371, 20, 0.26066, 0.9264, {"test_data/port_channel_estimator_test_input_rg7.dat"}, {"test_data/port_channel_estimator_test_pilots7.dat"}, {"test_data/port_channel_estimator_test_output_ch_est7.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 1.0289, 1.0289, 20, 30, 0.0010289, {"test_data/port_channel_estimator_test_input_rg8.dat"}, {"test_data/port_channel_estimator_test_pilots8.dat"}, {"test_data/port_channel_estimator_test_output_ch_est8.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 2, {{{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.93801, 0.93801, 20, 30, 0.00093801, {"test_data/port_channel_estimator_test_input_rg9.dat"}, {"test_data/port_channel_estimator_test_pilots9.dat"}, {"test_data/port_channel_estimator_test_output_ch_est9.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 2, {{{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 1, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.95059, 0.95059, 20, 30, 0.00095059, {"test_data/port_channel_estimator_test_input_rg10.dat"}, {"test_data/port_channel_estimator_test_pilots10.dat"}, {"test_data/port_channel_estimator_test_output_ch_est10.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 1, {{{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0282, 1.0282, 20, 30, 0.0010282, {"test_data/port_channel_estimator_test_input_rg11.dat"}, {"test_data/port_channel_estimator_test_pilots11.dat"}, {"test_data/port_channel_estimator_test_output_ch_est11.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 2, {{{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.99519, 0.99519, 20, 30, 0.00099519, {"test_data/port_channel_estimator_test_input_rg12.dat"}, {"test_data/port_channel_estimator_test_pilots12.dat"}, {"test_data/port_channel_estimator_test_output_ch_est12.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 2, {{{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 6, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0079, 1.0079, 20, 30, 0.0010079, {"test_data/port_channel_estimator_test_input_rg13.dat"}, {"test_data/port_channel_estimator_test_pilots13.dat"}, {"test_data/port_channel_estimator_test_output_ch_est13.dat"}},
    // clang-format on
};

} // namespace srsgnb
