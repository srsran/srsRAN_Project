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

// This file was generated using the following MATLAB class on 14-09-2023 (seed 0):
//   + "srsChEqualizerUnittest.m"

#include "srsran/adt/complex.h"
#include "srsran/support/file_vector.h"

namespace srsran {

template <typename T>
struct re_exploded {
  unsigned       nof_prb, nof_symbols, nof_slices;
  file_vector<T> measurements;
};

struct ch_estimates_exploded {
  unsigned          nof_prb, nof_symbols, nof_rx_ports, nof_tx_layers;
  float             noise_var;
  file_vector<cf_t> estimates;
};

struct test_case_t {
  re_exploded<cf_t>     equalized_symbols;
  re_exploded<float>    equalized_noise_vars;
  re_exploded<cf_t>     transmitted_symbols;
  re_exploded<cf_t>     received_symbols;
  ch_estimates_exploded ch_estimates;
  float                 scaling;
  std::string           equalizer_type;
};

static const std::vector<test_case_t> channel_equalizer_test_data = {
    // clang-format off
{  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_symbols0.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_noise_vars0.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_check_tx_symbols0.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_input_rx_symbols0.dat"}},  {25, 14, 1, 1, 0.1, {"test_data/channel_equalizer_test_input_ch_estimates0.dat"}}, 1.2, "MMSE"},
{  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_symbols2.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_noise_vars2.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_check_tx_symbols2.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_input_rx_symbols2.dat"}},  {25, 14, 1, 1, 0.1, {"test_data/channel_equalizer_test_input_ch_estimates2.dat"}}, 1.2, "ZF"},
{  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_symbols4.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_noise_vars4.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_check_tx_symbols4.dat"}},  {25, 14, 2, {"test_data/channel_equalizer_test_input_rx_symbols4.dat"}},  {25, 14, 2, 1, 0.1, {"test_data/channel_equalizer_test_input_ch_estimates4.dat"}}, 1.2, "MMSE"},
{  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_symbols6.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_noise_vars6.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_check_tx_symbols6.dat"}},  {25, 14, 2, {"test_data/channel_equalizer_test_input_rx_symbols6.dat"}},  {25, 14, 2, 1, 0.1, {"test_data/channel_equalizer_test_input_ch_estimates6.dat"}}, 1.2, "ZF"},
{  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_symbols8.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_noise_vars8.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_check_tx_symbols8.dat"}},  {25, 14, 3, {"test_data/channel_equalizer_test_input_rx_symbols8.dat"}},  {25, 14, 3, 1, 0.1, {"test_data/channel_equalizer_test_input_ch_estimates8.dat"}}, 1.2, "MMSE"},
{  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_symbols10.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_noise_vars10.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_check_tx_symbols10.dat"}},  {25, 14, 3, {"test_data/channel_equalizer_test_input_rx_symbols10.dat"}},  {25, 14, 3, 1, 0.1, {"test_data/channel_equalizer_test_input_ch_estimates10.dat"}}, 1.2, "ZF"},
{  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_symbols12.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_noise_vars12.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_check_tx_symbols12.dat"}},  {25, 14, 4, {"test_data/channel_equalizer_test_input_rx_symbols12.dat"}},  {25, 14, 4, 1, 0.1, {"test_data/channel_equalizer_test_input_ch_estimates12.dat"}}, 1.2, "MMSE"},
{  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_symbols14.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_output_eq_noise_vars14.dat"}},  {25, 14, 1, {"test_data/channel_equalizer_test_check_tx_symbols14.dat"}},  {25, 14, 4, {"test_data/channel_equalizer_test_input_rx_symbols14.dat"}},  {25, 14, 4, 1, 0.1, {"test_data/channel_equalizer_test_input_ch_estimates14.dat"}}, 1.2, "ZF"},
{  {25, 14, 2, {"test_data/channel_equalizer_test_output_eq_symbols16.dat"}},  {25, 14, 2, {"test_data/channel_equalizer_test_output_eq_noise_vars16.dat"}},  {25, 14, 2, {"test_data/channel_equalizer_test_check_tx_symbols16.dat"}},  {25, 14, 2, {"test_data/channel_equalizer_test_input_rx_symbols16.dat"}},  {25, 14, 2, 2, 0.1, {"test_data/channel_equalizer_test_input_ch_estimates16.dat"}}, 1.2, "MMSE"},
{  {25, 14, 2, {"test_data/channel_equalizer_test_output_eq_symbols18.dat"}},  {25, 14, 2, {"test_data/channel_equalizer_test_output_eq_noise_vars18.dat"}},  {25, 14, 2, {"test_data/channel_equalizer_test_check_tx_symbols18.dat"}},  {25, 14, 2, {"test_data/channel_equalizer_test_input_rx_symbols18.dat"}},  {25, 14, 2, 2, 0.1, {"test_data/channel_equalizer_test_input_ch_estimates18.dat"}}, 1.2, "ZF"},
    // clang-format on
};

} // namespace srsran
