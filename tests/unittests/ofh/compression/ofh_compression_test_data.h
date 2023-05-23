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

// This file was generated using the following MATLAB class on 17-05-2023:
//   + "srsOFHCompressionUnittest.m"

#include "srsran/adt/complex.h"
#include "srsran/ofh/compression/compression_params.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  std::size_t           nof_prb;
  ofh::compression_type type;
  unsigned              cIQ_width;
  float                 iq_scaling;
  file_vector<cf_t>     symbols;
  file_vector<int16_t>  compressed_IQs;
  file_vector<uint8_t>  compressed_params;
};

static const std::vector<test_case_t> ofh_compression_test_data = {
    // clang-format off
  {25, ofh::compression_type::none, 8, 0.44045, {"test_data/ofh_compression_test_input0.dat"}, {"test_data/ofh_compression_test_c_output0.dat"}, {"test_data/ofh_compression_test_c_param0.dat"}},
  {25, ofh::compression_type::none, 9, 1, {"test_data/ofh_compression_test_input1.dat"}, {"test_data/ofh_compression_test_c_output1.dat"}, {"test_data/ofh_compression_test_c_param1.dat"}},
  {25, ofh::compression_type::none, 14, 1, {"test_data/ofh_compression_test_input2.dat"}, {"test_data/ofh_compression_test_c_output2.dat"}, {"test_data/ofh_compression_test_c_param2.dat"}},
  {25, ofh::compression_type::none, 16, 1, {"test_data/ofh_compression_test_input3.dat"}, {"test_data/ofh_compression_test_c_output3.dat"}, {"test_data/ofh_compression_test_c_param3.dat"}},
  {25, ofh::compression_type::BFP, 8, 0.51688, {"test_data/ofh_compression_test_input4.dat"}, {"test_data/ofh_compression_test_c_output4.dat"}, {"test_data/ofh_compression_test_c_param4.dat"}},
  {25, ofh::compression_type::BFP, 9, 0.80566, {"test_data/ofh_compression_test_input5.dat"}, {"test_data/ofh_compression_test_c_output5.dat"}, {"test_data/ofh_compression_test_c_param5.dat"}},
  {25, ofh::compression_type::BFP, 14, 0.95194, {"test_data/ofh_compression_test_input6.dat"}, {"test_data/ofh_compression_test_c_output6.dat"}, {"test_data/ofh_compression_test_c_param6.dat"}},
  {25, ofh::compression_type::BFP, 16, 0.88037, {"test_data/ofh_compression_test_input7.dat"}, {"test_data/ofh_compression_test_c_output7.dat"}, {"test_data/ofh_compression_test_c_param7.dat"}},
  {275, ofh::compression_type::none, 8, 0.75182, {"test_data/ofh_compression_test_input8.dat"}, {"test_data/ofh_compression_test_c_output8.dat"}, {"test_data/ofh_compression_test_c_param8.dat"}},
  {275, ofh::compression_type::none, 9, 0.53145, {"test_data/ofh_compression_test_input9.dat"}, {"test_data/ofh_compression_test_c_output9.dat"}, {"test_data/ofh_compression_test_c_param9.dat"}},
  {275, ofh::compression_type::none, 14, 1, {"test_data/ofh_compression_test_input10.dat"}, {"test_data/ofh_compression_test_c_output10.dat"}, {"test_data/ofh_compression_test_c_param10.dat"}},
  {275, ofh::compression_type::none, 16, 0.72582, {"test_data/ofh_compression_test_input11.dat"}, {"test_data/ofh_compression_test_c_output11.dat"}, {"test_data/ofh_compression_test_c_param11.dat"}},
  {275, ofh::compression_type::BFP, 8, 0.34735, {"test_data/ofh_compression_test_input12.dat"}, {"test_data/ofh_compression_test_c_output12.dat"}, {"test_data/ofh_compression_test_c_param12.dat"}},
  {275, ofh::compression_type::BFP, 9, 0.67075, {"test_data/ofh_compression_test_input13.dat"}, {"test_data/ofh_compression_test_c_output13.dat"}, {"test_data/ofh_compression_test_c_param13.dat"}},
  {275, ofh::compression_type::BFP, 14, 1, {"test_data/ofh_compression_test_input14.dat"}, {"test_data/ofh_compression_test_c_output14.dat"}, {"test_data/ofh_compression_test_c_param14.dat"}},
  {275, ofh::compression_type::BFP, 16, 0.55838, {"test_data/ofh_compression_test_input15.dat"}, {"test_data/ofh_compression_test_c_output15.dat"}, {"test_data/ofh_compression_test_c_param15.dat"}},
    // clang-format on
};

} // namespace srsran
