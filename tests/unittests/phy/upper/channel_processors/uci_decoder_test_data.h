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

// This file was generated using the following MATLAB class on 22-Sep-2022:
//   + "srsUCIDecoderUnittest.m"

#include "srsgnb/phy/upper/channel_processors/uci_decoder.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include "srsgnb/ran/modulation_scheme.h"
#include "srsgnb/support/file_vector.h"

namespace srsgnb {

struct test_case_t {
  unsigned                          message_length = 0;
  unsigned                          llr_length     = 0;
  uci_decoder::configuration        config;
  file_vector<uint8_t>              message;
  file_vector<log_likelihood_ratio> llr;
};

static const std::vector<test_case_t> uci_decoder_test_data = {
    // clang-format off
  {1, 2017, {modulation_scheme::BPSK}, {"test_data/uci_decoder_test_output0.dat"}, {"test_data/uci_decoder_test_input0.dat"}},
  {1, 528, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output1.dat"}, {"test_data/uci_decoder_test_input1.dat"}},
  {1, 3124, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output2.dat"}, {"test_data/uci_decoder_test_input2.dat"}},
  {1, 414, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output3.dat"}, {"test_data/uci_decoder_test_input3.dat"}},
  {1, 1224, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output4.dat"}, {"test_data/uci_decoder_test_input4.dat"}},
  {2, 7824, {modulation_scheme::BPSK}, {"test_data/uci_decoder_test_output5.dat"}, {"test_data/uci_decoder_test_input5.dat"}},
  {2, 8100, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output6.dat"}, {"test_data/uci_decoder_test_input6.dat"}},
  {2, 2092, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output7.dat"}, {"test_data/uci_decoder_test_input7.dat"}},
  {2, 138, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output8.dat"}, {"test_data/uci_decoder_test_input8.dat"}},
  {2, 7008, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output9.dat"}, {"test_data/uci_decoder_test_input9.dat"}},
  {3, 7835, {modulation_scheme::BPSK}, {"test_data/uci_decoder_test_output10.dat"}, {"test_data/uci_decoder_test_input10.dat"}},
  {3, 2768, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output11.dat"}, {"test_data/uci_decoder_test_input11.dat"}},
  {3, 5832, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output12.dat"}, {"test_data/uci_decoder_test_input12.dat"}},
  {3, 6120, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output13.dat"}, {"test_data/uci_decoder_test_input13.dat"}},
  {3, 1816, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output14.dat"}, {"test_data/uci_decoder_test_input14.dat"}},
  {4, 3615, {modulation_scheme::BPSK}, {"test_data/uci_decoder_test_output15.dat"}, {"test_data/uci_decoder_test_input15.dat"}},
  {4, 2310, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output16.dat"}, {"test_data/uci_decoder_test_input16.dat"}},
  {4, 6632, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output17.dat"}, {"test_data/uci_decoder_test_input17.dat"}},
  {4, 138, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output18.dat"}, {"test_data/uci_decoder_test_input18.dat"}},
  {4, 176, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output19.dat"}, {"test_data/uci_decoder_test_input19.dat"}},
  {5, 4889, {modulation_scheme::BPSK}, {"test_data/uci_decoder_test_output20.dat"}, {"test_data/uci_decoder_test_input20.dat"}},
  {5, 3986, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output21.dat"}, {"test_data/uci_decoder_test_input21.dat"}},
  {5, 5212, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output22.dat"}, {"test_data/uci_decoder_test_input22.dat"}},
  {5, 4314, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output23.dat"}, {"test_data/uci_decoder_test_input23.dat"}},
  {5, 4008, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output24.dat"}, {"test_data/uci_decoder_test_input24.dat"}},
  {6, 2740, {modulation_scheme::BPSK}, {"test_data/uci_decoder_test_output25.dat"}, {"test_data/uci_decoder_test_input25.dat"}},
  {6, 282, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output26.dat"}, {"test_data/uci_decoder_test_input26.dat"}},
  {6, 3156, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output27.dat"}, {"test_data/uci_decoder_test_input27.dat"}},
  {6, 6426, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output28.dat"}, {"test_data/uci_decoder_test_input28.dat"}},
  {6, 6704, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output29.dat"}, {"test_data/uci_decoder_test_input29.dat"}},
  {7, 6750, {modulation_scheme::BPSK}, {"test_data/uci_decoder_test_output30.dat"}, {"test_data/uci_decoder_test_input30.dat"}},
  {7, 686, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output31.dat"}, {"test_data/uci_decoder_test_input31.dat"}},
  {7, 7576, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output32.dat"}, {"test_data/uci_decoder_test_input32.dat"}},
  {7, 732, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output33.dat"}, {"test_data/uci_decoder_test_input33.dat"}},
  {7, 2312, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output34.dat"}, {"test_data/uci_decoder_test_input34.dat"}},
  {8, 405, {modulation_scheme::BPSK}, {"test_data/uci_decoder_test_output35.dat"}, {"test_data/uci_decoder_test_input35.dat"}},
  {8, 1812, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output36.dat"}, {"test_data/uci_decoder_test_input36.dat"}},
  {8, 6744, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output37.dat"}, {"test_data/uci_decoder_test_input37.dat"}},
  {8, 7812, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output38.dat"}, {"test_data/uci_decoder_test_input38.dat"}},
  {8, 1520, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output39.dat"}, {"test_data/uci_decoder_test_input39.dat"}},
  {9, 7166, {modulation_scheme::BPSK}, {"test_data/uci_decoder_test_output40.dat"}, {"test_data/uci_decoder_test_input40.dat"}},
  {9, 3726, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output41.dat"}, {"test_data/uci_decoder_test_input41.dat"}},
  {9, 7592, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output42.dat"}, {"test_data/uci_decoder_test_input42.dat"}},
  {9, 6882, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output43.dat"}, {"test_data/uci_decoder_test_input43.dat"}},
  {9, 4808, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output44.dat"}, {"test_data/uci_decoder_test_input44.dat"}},
  {10, 1676, {modulation_scheme::BPSK}, {"test_data/uci_decoder_test_output45.dat"}, {"test_data/uci_decoder_test_input45.dat"}},
  {10, 4680, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output46.dat"}, {"test_data/uci_decoder_test_input46.dat"}},
  {10, 3372, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output47.dat"}, {"test_data/uci_decoder_test_input47.dat"}},
  {10, 300, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output48.dat"}, {"test_data/uci_decoder_test_input48.dat"}},
  {10, 7752, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output49.dat"}, {"test_data/uci_decoder_test_input49.dat"}},
  {11, 477, {modulation_scheme::BPSK}, {"test_data/uci_decoder_test_output50.dat"}, {"test_data/uci_decoder_test_input50.dat"}},
  {11, 5596, {modulation_scheme::QPSK}, {"test_data/uci_decoder_test_output51.dat"}, {"test_data/uci_decoder_test_input51.dat"}},
  {11, 4824, {modulation_scheme::QAM16}, {"test_data/uci_decoder_test_output52.dat"}, {"test_data/uci_decoder_test_input52.dat"}},
  {11, 2634, {modulation_scheme::QAM64}, {"test_data/uci_decoder_test_output53.dat"}, {"test_data/uci_decoder_test_input53.dat"}},
  {11, 4072, {modulation_scheme::QAM256}, {"test_data/uci_decoder_test_output54.dat"}, {"test_data/uci_decoder_test_input54.dat"}},
    // clang-format on
};

} // namespace srsgnb
