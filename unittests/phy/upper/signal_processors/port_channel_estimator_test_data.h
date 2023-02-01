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

// This file was generated using the following MATLAB class on 01-02-2023:
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
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.0057, 2.0133, 20, 3.276, 0.47278, {"test_data/port_channel_estimator_test_input_rg0.dat"}, {"test_data/port_channel_estimator_test_pilots0.dat"}, {"test_data/port_channel_estimator_test_output_ch_est0.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 1.9752, 1.9798, 20, 26.9899, 0.0019798, {"test_data/port_channel_estimator_test_input_rg1.dat"}, {"test_data/port_channel_estimator_test_pilots1.dat"}, {"test_data/port_channel_estimator_test_output_ch_est1.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 1.9826, 1.9908, 20, 3.9711, 0.39821, {"test_data/port_channel_estimator_test_input_rg2.dat"}, {"test_data/port_channel_estimator_test_pilots2.dat"}, {"test_data/port_channel_estimator_test_output_ch_est2.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.0055, 2.0105, 20, 26.9891, 0.0020105, {"test_data/port_channel_estimator_test_input_rg3.dat"}, {"test_data/port_channel_estimator_test_pilots3.dat"}, {"test_data/port_channel_estimator_test_output_ch_est3.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.0026, 2.0099, 20, 14.8935, 0.032526, {"test_data/port_channel_estimator_test_input_rg4.dat"}, {"test_data/port_channel_estimator_test_pilots4.dat"}, {"test_data/port_channel_estimator_test_output_ch_est4.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 8, 4, {{{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 1.0759, 1.0797, 20, 29.9847, 0.0010797, {"test_data/port_channel_estimator_test_input_rg5.dat"}, {"test_data/port_channel_estimator_test_pilots5.dat"}, {"test_data/port_channel_estimator_test_output_ch_est5.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 8, 4, {{{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 10, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 1.0428, 1.0428, 20, 30, 0.0010428, {"test_data/port_channel_estimator_test_input_rg6.dat"}, {"test_data/port_channel_estimator_test_pilots6.dat"}, {"test_data/port_channel_estimator_test_output_ch_est6.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 1.0098, 1.017, 20, 0.22812, 0.95808, {"test_data/port_channel_estimator_test_input_rg7.dat"}, {"test_data/port_channel_estimator_test_pilots7.dat"}, {"test_data/port_channel_estimator_test_output_ch_est7.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 1.0164, 1.0241, 20, 29.9674, 0.0010241, {"test_data/port_channel_estimator_test_input_rg8.dat"}, {"test_data/port_channel_estimator_test_pilots8.dat"}, {"test_data/port_channel_estimator_test_output_ch_est8.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 2, {{{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.94659, 0.95364, 20, 29.9678, 0.00095364, {"test_data/port_channel_estimator_test_input_rg9.dat"}, {"test_data/port_channel_estimator_test_pilots9.dat"}, {"test_data/port_channel_estimator_test_output_ch_est9.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 2, {{{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 1, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0294, 1.0294, 20, 30, 0.0010294, {"test_data/port_channel_estimator_test_input_rg10.dat"}, {"test_data/port_channel_estimator_test_pilots10.dat"}, {"test_data/port_channel_estimator_test_output_ch_est10.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 1, {{{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0108, 1.0108, 20, 30, 0.0010108, {"test_data/port_channel_estimator_test_input_rg11.dat"}, {"test_data/port_channel_estimator_test_pilots11.dat"}, {"test_data/port_channel_estimator_test_output_ch_est11.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 2, {{{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0011, 1.0061, 20, 29.9786, 0.0010061, {"test_data/port_channel_estimator_test_input_rg12.dat"}, {"test_data/port_channel_estimator_test_pilots12.dat"}, {"test_data/port_channel_estimator_test_output_ch_est12.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 2, {{{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 6, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.98264, 0.98264, 20, 30, 0.00098264, {"test_data/port_channel_estimator_test_input_rg13.dat"}, {"test_data/port_channel_estimator_test_pilots13.dat"}, {"test_data/port_channel_estimator_test_output_ch_est13.dat"}},
    // clang-format on
};

} // namespace srsgnb
