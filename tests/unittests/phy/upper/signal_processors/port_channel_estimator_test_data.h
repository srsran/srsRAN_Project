/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

// This file was generated using the following MATLAB class on 14-02-2023:
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
  float                                                   ta_us          = 0;
  file_vector<resource_grid_reader_spy::expected_entry_t> grid;
  file_vector<cf_t>                                       pilots;
  file_vector<resource_grid_reader_spy::expected_entry_t> estimates;
};

static const std::vector<test_case_t> port_channel_estimator_test_data = {
    // clang-format off
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.005, 2.0125, 20, 11.3581, 0.073502, 0.52083, {"test_data/port_channel_estimator_test_input_rg0.dat"}, {"test_data/port_channel_estimator_test_pilots0.dat"}, {"test_data/port_channel_estimator_test_output_ch_est0.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.0515, 2.0562, 20, 26.99, 0.0020562, 0.10579, {"test_data/port_channel_estimator_test_input_rg1.dat"}, {"test_data/port_channel_estimator_test_pilots1.dat"}, {"test_data/port_channel_estimator_test_output_ch_est1.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 1.9905, 1.998, 20, 18.9998, 0.01256, 0.11393, {"test_data/port_channel_estimator_test_input_rg2.dat"}, {"test_data/port_channel_estimator_test_pilots2.dat"}, {"test_data/port_channel_estimator_test_output_ch_est2.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 2.016, 2.0208, 20, 26.9898, 0.0020208, 0.32552, {"test_data/port_channel_estimator_test_input_rg3.dat"}, {"test_data/port_channel_estimator_test_pilots3.dat"}, {"test_data/port_channel_estimator_test_output_ch_est3.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0}, {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {}, nullopt, {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}}}, {0}, 1.4125}, 52, 1.9941, 2.0019, 20, 14.8448, 0.032753, 0.32552, {"test_data/port_channel_estimator_test_input_rg4.dat"}, {"test_data/port_channel_estimator_test_pilots4.dat"}, {"test_data/port_channel_estimator_test_output_ch_est4.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 8, 4, {{{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 0.97774, 0.98216, 20, 29.9804, 0.00098216, 0.13021, {"test_data/port_channel_estimator_test_input_rg5.dat"}, {"test_data/port_channel_estimator_test_pilots5.dat"}, {"test_data/port_channel_estimator_test_output_ch_est5.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 8, 4, {{{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 10, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 1.0147, 1.0147, 20, 30, 0.0010147, 0.69173, {"test_data/port_channel_estimator_test_input_rg6.dat"}, {"test_data/port_channel_estimator_test_pilots6.dat"}, {"test_data/port_channel_estimator_test_output_ch_est6.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, {}, nullopt, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 1.0233, 1.0313, 20, 18.3221, 0.015059, 0.21159, {"test_data/port_channel_estimator_test_input_rg7.dat"}, {"test_data/port_channel_estimator_test_pilots7.dat"}, {"test_data/port_channel_estimator_test_output_ch_est7.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 14, {{{1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 7, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}, {0}, 1}, 52, 0.99889, 1.0064, 20, 29.9673, 0.0010064, 0.67546, {"test_data/port_channel_estimator_test_input_rg8.dat"}, {"test_data/port_channel_estimator_test_pilots8.dat"}, {"test_data/port_channel_estimator_test_output_ch_est8.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 2, {{{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.97811, 0.98253, 20, 29.9804, 0.00098253, 0.14648, {"test_data/port_channel_estimator_test_input_rg9.dat"}, {"test_data/port_channel_estimator_test_pilots9.dat"}, {"test_data/port_channel_estimator_test_output_ch_est9.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 0, 2, {{{1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 1, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0183, 1.0183, 20, 30, 0.0010183, 1.1312, {"test_data/port_channel_estimator_test_input_rg10.dat"}, {"test_data/port_channel_estimator_test_pilots10.dat"}, {"test_data/port_channel_estimator_test_output_ch_est10.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 1, {{{0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.99837, 0.99837, 20, 30, 0.00099837, -0.016276, {"test_data/port_channel_estimator_test_input_rg11.dat"}, {"test_data/port_channel_estimator_test_pilots11.dat"}, {"test_data/port_channel_estimator_test_output_ch_est11.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 2, {{{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {}, nullopt, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 1.0084, 1.0133, 20, 29.9789, 0.0010133, 0.63477, {"test_data/port_channel_estimator_test_input_rg12.dat"}, {"test_data/port_channel_estimator_test_pilots12.dat"}, {"test_data/port_channel_estimator_test_output_ch_est12.dat"}},
  {{subcarrier_spacing::kHz15, cyclic_prefix::NORMAL, 5, 2, {{{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 6, {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0}}}, {0}, 1}, 52, 0.98953, 0.98953, 20, 30, 0.00098953, 1.0742, {"test_data/port_channel_estimator_test_input_rg13.dat"}, {"test_data/port_channel_estimator_test_pilots13.dat"}, {"test_data/port_channel_estimator_test_output_ch_est13.dat"}},
    // clang-format on
};

} // namespace srsgnb
