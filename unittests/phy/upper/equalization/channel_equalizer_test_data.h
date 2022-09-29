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

// This file was generated using the following MATLAB class on 29-Sep-2022:
//   + "srsChEqualizerUnittest.m"

#include "srsgnb/adt/complex.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

template <typename T>
struct re_measurement_exploded {
  unsigned       nof_prb, nof_symbols, nof_slices;
  file_vector<T> measurements;
};

struct ch_estimates_exploded {
  unsigned          nof_prb, nof_symbols, nof_rx_ports, nof_tx_layers;
  float             noise_var;
  file_vector<cf_t> estimates;
};

struct test_case_t {
  re_measurement_exploded<cf_t>  equalized_symbols;
  re_measurement_exploded<float> equalized_noise_vars;
  re_measurement_exploded<cf_t>  transmitted_symbols;
  re_measurement_exploded<cf_t>  received_symbols;
  ch_estimates_exploded          ch_estimates;
  float                          scaling;
  std::string                    equalizer_type;
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

} // namespace srsgnb
