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

// This file was generated using the following MATLAB class on 24-Oct-2022:
//   + "srsULSCHDemultiplexUnittest.m"

#include "srsgnb/phy/upper/channel_processors/ulsch_demultiplex.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include "srsgnb/support/file_vector.h"
#include <vector>

namespace srsgnb {

struct test_case_t {
  ulsch_demultiplex::configuration  config;
  file_vector<log_likelihood_ratio> input;
  file_vector<log_likelihood_ratio> output_ulsch;
  file_vector<log_likelihood_ratio> output_harq_ack;
  file_vector<log_likelihood_ratio> output_csi_part1;
  file_vector<log_likelihood_ratio> output_csi_part2;
};

static const std::vector<test_case_t> ulsch_demultiplex_test_data = {
    // clang-format off
  {{modulation_scheme::PI_2_BPSK, 1, 8, 0, 14, 118, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, 1}, {"test_data/ulsch_demultiplex_test_input0.dat"}, {"test_data/ulsch_demultiplex_test_data0.dat"}, {"test_data/ulsch_demultiplex_test_harq0.dat"}, {"test_data/ulsch_demultiplex_test_csi10.dat"}, {"test_data/ulsch_demultiplex_test_csi20.dat"}},
  {{modulation_scheme::PI_2_BPSK, 1, 6, 0, 14, 40, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, 1}, {"test_data/ulsch_demultiplex_test_input1.dat"}, {"test_data/ulsch_demultiplex_test_data1.dat"}, {"test_data/ulsch_demultiplex_test_harq1.dat"}, {"test_data/ulsch_demultiplex_test_csi11.dat"}, {"test_data/ulsch_demultiplex_test_csi21.dat"}},
  {{modulation_scheme::PI_2_BPSK, 3, 8, 0, 14, 48, dmrs_type::TYPE1, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, 2}, {"test_data/ulsch_demultiplex_test_input2.dat"}, {"test_data/ulsch_demultiplex_test_data2.dat"}, {"test_data/ulsch_demultiplex_test_harq2.dat"}, {"test_data/ulsch_demultiplex_test_csi12.dat"}, {"test_data/ulsch_demultiplex_test_csi22.dat"}},
  {{modulation_scheme::PI_2_BPSK, 1, 4, 0, 14, 48, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 1}, {"test_data/ulsch_demultiplex_test_input3.dat"}, {"test_data/ulsch_demultiplex_test_data3.dat"}, {"test_data/ulsch_demultiplex_test_harq3.dat"}, {"test_data/ulsch_demultiplex_test_csi13.dat"}, {"test_data/ulsch_demultiplex_test_csi23.dat"}},
  {{modulation_scheme::PI_2_BPSK, 3, 3, 0, 14, 57, dmrs_type::TYPE1, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, 2}, {"test_data/ulsch_demultiplex_test_input4.dat"}, {"test_data/ulsch_demultiplex_test_data4.dat"}, {"test_data/ulsch_demultiplex_test_harq4.dat"}, {"test_data/ulsch_demultiplex_test_csi14.dat"}, {"test_data/ulsch_demultiplex_test_csi24.dat"}},
  {{modulation_scheme::PI_2_BPSK, 1, 14, 0, 14, 73, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 1}, {"test_data/ulsch_demultiplex_test_input5.dat"}, {"test_data/ulsch_demultiplex_test_data5.dat"}, {"test_data/ulsch_demultiplex_test_harq5.dat"}, {"test_data/ulsch_demultiplex_test_csi15.dat"}, {"test_data/ulsch_demultiplex_test_csi25.dat"}},
  {{modulation_scheme::PI_2_BPSK, 4, 7, 0, 14, 308, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, 3}, {"test_data/ulsch_demultiplex_test_input6.dat"}, {"test_data/ulsch_demultiplex_test_data6.dat"}, {"test_data/ulsch_demultiplex_test_harq6.dat"}, {"test_data/ulsch_demultiplex_test_csi16.dat"}, {"test_data/ulsch_demultiplex_test_csi26.dat"}},
  {{modulation_scheme::PI_2_BPSK, 4, 11, 0, 14, 44, dmrs_type::TYPE1, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, 2}, {"test_data/ulsch_demultiplex_test_input7.dat"}, {"test_data/ulsch_demultiplex_test_data7.dat"}, {"test_data/ulsch_demultiplex_test_harq7.dat"}, {"test_data/ulsch_demultiplex_test_csi17.dat"}, {"test_data/ulsch_demultiplex_test_csi27.dat"}},
  {{modulation_scheme::PI_2_BPSK, 3, 12, 0, 14, 0, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, 3}, {"test_data/ulsch_demultiplex_test_input8.dat"}, {"test_data/ulsch_demultiplex_test_data8.dat"}, {"test_data/ulsch_demultiplex_test_harq8.dat"}, {"test_data/ulsch_demultiplex_test_csi18.dat"}, {"test_data/ulsch_demultiplex_test_csi28.dat"}},
  {{modulation_scheme::PI_2_BPSK, 1, 5, 0, 14, 0, dmrs_type::TYPE1, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, 1}, {"test_data/ulsch_demultiplex_test_input9.dat"}, {"test_data/ulsch_demultiplex_test_data9.dat"}, {"test_data/ulsch_demultiplex_test_harq9.dat"}, {"test_data/ulsch_demultiplex_test_csi19.dat"}, {"test_data/ulsch_demultiplex_test_csi29.dat"}},
  {{modulation_scheme::PI_2_BPSK, 1, 5, 0, 14, 0, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, 1}, {"test_data/ulsch_demultiplex_test_input10.dat"}, {"test_data/ulsch_demultiplex_test_data10.dat"}, {"test_data/ulsch_demultiplex_test_harq10.dat"}, {"test_data/ulsch_demultiplex_test_csi110.dat"}, {"test_data/ulsch_demultiplex_test_csi210.dat"}},
  {{modulation_scheme::PI_2_BPSK, 4, 15, 0, 14, 0, dmrs_type::TYPE1, {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, 2}, {"test_data/ulsch_demultiplex_test_input11.dat"}, {"test_data/ulsch_demultiplex_test_data11.dat"}, {"test_data/ulsch_demultiplex_test_harq11.dat"}, {"test_data/ulsch_demultiplex_test_csi111.dat"}, {"test_data/ulsch_demultiplex_test_csi211.dat"}},
  {{modulation_scheme::QAM16, 2, 3, 0, 14, 128, dmrs_type::TYPE1, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 2}, {"test_data/ulsch_demultiplex_test_input12.dat"}, {"test_data/ulsch_demultiplex_test_data12.dat"}, {"test_data/ulsch_demultiplex_test_harq12.dat"}, {"test_data/ulsch_demultiplex_test_csi112.dat"}, {"test_data/ulsch_demultiplex_test_csi212.dat"}},
  {{modulation_scheme::QAM16, 3, 9, 0, 14, 48, dmrs_type::TYPE1, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, 2}, {"test_data/ulsch_demultiplex_test_input13.dat"}, {"test_data/ulsch_demultiplex_test_data13.dat"}, {"test_data/ulsch_demultiplex_test_harq13.dat"}, {"test_data/ulsch_demultiplex_test_csi113.dat"}, {"test_data/ulsch_demultiplex_test_csi213.dat"}},
  {{modulation_scheme::QAM16, 4, 6, 0, 14, 192, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, 3}, {"test_data/ulsch_demultiplex_test_input14.dat"}, {"test_data/ulsch_demultiplex_test_data14.dat"}, {"test_data/ulsch_demultiplex_test_harq14.dat"}, {"test_data/ulsch_demultiplex_test_csi114.dat"}, {"test_data/ulsch_demultiplex_test_csi214.dat"}},
  {{modulation_scheme::QAM16, 2, 10, 0, 14, 72, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 3}, {"test_data/ulsch_demultiplex_test_input15.dat"}, {"test_data/ulsch_demultiplex_test_data15.dat"}, {"test_data/ulsch_demultiplex_test_harq15.dat"}, {"test_data/ulsch_demultiplex_test_csi115.dat"}, {"test_data/ulsch_demultiplex_test_csi215.dat"}},
  {{modulation_scheme::QAM16, 1, 8, 0, 14, 168, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 1}, {"test_data/ulsch_demultiplex_test_input16.dat"}, {"test_data/ulsch_demultiplex_test_data16.dat"}, {"test_data/ulsch_demultiplex_test_harq16.dat"}, {"test_data/ulsch_demultiplex_test_csi116.dat"}, {"test_data/ulsch_demultiplex_test_csi216.dat"}},
  {{modulation_scheme::QAM16, 4, 11, 0, 14, 48, dmrs_type::TYPE1, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 2}, {"test_data/ulsch_demultiplex_test_input17.dat"}, {"test_data/ulsch_demultiplex_test_data17.dat"}, {"test_data/ulsch_demultiplex_test_harq17.dat"}, {"test_data/ulsch_demultiplex_test_csi117.dat"}, {"test_data/ulsch_demultiplex_test_csi217.dat"}},
  {{modulation_scheme::QAM16, 2, 11, 0, 14, 104, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 3}, {"test_data/ulsch_demultiplex_test_input18.dat"}, {"test_data/ulsch_demultiplex_test_data18.dat"}, {"test_data/ulsch_demultiplex_test_harq18.dat"}, {"test_data/ulsch_demultiplex_test_csi118.dat"}, {"test_data/ulsch_demultiplex_test_csi218.dat"}},
  {{modulation_scheme::QAM16, 4, 2, 0, 14, 64, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0}, 3}, {"test_data/ulsch_demultiplex_test_input19.dat"}, {"test_data/ulsch_demultiplex_test_data19.dat"}, {"test_data/ulsch_demultiplex_test_harq19.dat"}, {"test_data/ulsch_demultiplex_test_csi119.dat"}, {"test_data/ulsch_demultiplex_test_csi219.dat"}},
  {{modulation_scheme::QAM16, 2, 7, 0, 14, 0, dmrs_type::TYPE1, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, 2}, {"test_data/ulsch_demultiplex_test_input20.dat"}, {"test_data/ulsch_demultiplex_test_data20.dat"}, {"test_data/ulsch_demultiplex_test_harq20.dat"}, {"test_data/ulsch_demultiplex_test_csi120.dat"}, {"test_data/ulsch_demultiplex_test_csi220.dat"}},
  {{modulation_scheme::QAM16, 2, 13, 0, 14, 0, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, 3}, {"test_data/ulsch_demultiplex_test_input21.dat"}, {"test_data/ulsch_demultiplex_test_data21.dat"}, {"test_data/ulsch_demultiplex_test_harq21.dat"}, {"test_data/ulsch_demultiplex_test_csi121.dat"}, {"test_data/ulsch_demultiplex_test_csi221.dat"}},
  {{modulation_scheme::QAM16, 1, 12, 0, 14, 0, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 1}, {"test_data/ulsch_demultiplex_test_input22.dat"}, {"test_data/ulsch_demultiplex_test_data22.dat"}, {"test_data/ulsch_demultiplex_test_harq22.dat"}, {"test_data/ulsch_demultiplex_test_csi122.dat"}, {"test_data/ulsch_demultiplex_test_csi222.dat"}},
  {{modulation_scheme::QAM16, 2, 1, 0, 14, 0, dmrs_type::TYPE2, {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0}, 3}, {"test_data/ulsch_demultiplex_test_input23.dat"}, {"test_data/ulsch_demultiplex_test_data23.dat"}, {"test_data/ulsch_demultiplex_test_harq23.dat"}, {"test_data/ulsch_demultiplex_test_csi123.dat"}, {"test_data/ulsch_demultiplex_test_csi223.dat"}},
    // clang-format on
};

} // namespace srsgnb
