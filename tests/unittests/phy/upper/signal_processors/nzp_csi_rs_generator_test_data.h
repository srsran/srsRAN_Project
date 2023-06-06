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

// This file was generated using the following MATLAB class on 06-06-2023:
//   + "srsNZPCSIRSGeneratorUnittest.m"

#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/signal_processors/nzp_csi_rs_generator.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  nzp_csi_rs_generator::config_t                          config;
  file_vector<resource_grid_writer_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> nzp_csi_rs_generator_test_data = {
    // clang-format off
  {{{0, 131, 1, 0}, cyclic_prefix::NORMAL, 203, 16, 1, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 80, 4.7, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output0.dat"}},
  {{{0, 669, 8, 0}, cyclic_prefix::NORMAL, 3, 268, 1, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 381, 7.1, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output1.dat"}},
  {{{2, 205, 4, 0}, cyclic_prefix::NORMAL, 72, 17, 1, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 91, 0.5, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output2.dat"}},
  {{{2, 850, 0, 0}, cyclic_prefix::NORMAL, 13, 233, 1, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 368, 4, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output3.dat"}},
  {{{2, 537, 4, 0}, cyclic_prefix::EXTENDED, 11, 243, 1, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 375, 2.2, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output4.dat"}},
  {{{2, 407, 1, 1}, cyclic_prefix::EXTENDED, 22, 223, 1, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 265, 7.1, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output5.dat"}},
  {{{0, 823, 1, 0}, cyclic_prefix::NORMAL, 16, 240, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 159, 6.9, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output6.dat"}},
  {{{0, 621, 7, 0}, cyclic_prefix::NORMAL, 191, 77, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 464, 4.3, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output7.dat"}},
  {{{0, 59, 4, 0}, cyclic_prefix::NORMAL, 40, 67, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 524, 6.4, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output8.dat"}},
  {{{0, 269, 7, 0}, cyclic_prefix::NORMAL, 21, 138, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 371, 1.3, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output9.dat"}},
  {{{0, 490, 7, 0}, cyclic_prefix::NORMAL, 81, 161, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 401, 8.9, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output10.dat"}},
  {{{0, 591, 3, 0}, cyclic_prefix::NORMAL, 102, 15, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 747, 6.1, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output11.dat"}},
  {{{0, 257, 0, 0}, cyclic_prefix::NORMAL, 51, 208, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 86, 7.7, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output12.dat"}},
  {{{0, 810, 8, 0}, cyclic_prefix::NORMAL, 54, 163, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 594, 1.3, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output13.dat"}},
  {{{0, 117, 5, 0}, cyclic_prefix::NORMAL, 67, 116, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 127, 3.5, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output14.dat"}},
  {{{0, 618, 6, 0}, cyclic_prefix::NORMAL, 31, 61, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 126, 2.6, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output15.dat"}},
  {{{0, 870, 7, 0}, cyclic_prefix::NORMAL, 23, 115, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 560, 8.1, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output16.dat"}},
  {{{0, 484, 4, 0}, cyclic_prefix::NORMAL, 63, 195, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 226, 3, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output17.dat"}},
  {{{2, 90, 0, 0}, cyclic_prefix::NORMAL, 13, 245, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 833, 9.3, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output18.dat"}},
  {{{2, 145, 8, 2}, cyclic_prefix::NORMAL, 31, 87, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 861, 4.7, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output19.dat"}},
  {{{2, 1009, 1, 0}, cyclic_prefix::NORMAL, 12, 175, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 924, 9.2, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output20.dat"}},
  {{{2, 334, 9, 0}, cyclic_prefix::NORMAL, 119, 120, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 237, 2.5, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output21.dat"}},
  {{{2, 547, 4, 3}, cyclic_prefix::EXTENDED, 36, 219, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 714, 3.6, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output22.dat"}},
  {{{2, 543, 3, 3}, cyclic_prefix::EXTENDED, 45, 206, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 605, 0.8, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output23.dat"}},
  {{{2, 329, 8, 2}, cyclic_prefix::EXTENDED, 38, 216, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 409, 9.6, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output24.dat"}},
  {{{2, 398, 6, 3}, cyclic_prefix::EXTENDED, 21, 129, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 246, 2.2, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output25.dat"}},
  {{{2, 330, 0, 1}, cyclic_prefix::NORMAL, 58, 166, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 620, 4.9, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output26.dat"}},
  {{{2, 317, 2, 2}, cyclic_prefix::NORMAL, 184, 59, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 131, 8.2, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output27.dat"}},
  {{{2, 509, 8, 2}, cyclic_prefix::NORMAL, 49, 195, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 646, 9.3, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output28.dat"}},
  {{{2, 685, 1, 1}, cyclic_prefix::NORMAL, 55, 12, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 342, 5.6, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output29.dat"}},
  {{{2, 49, 5, 0}, cyclic_prefix::EXTENDED, 46, 123, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 880, 5.4, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output30.dat"}},
  {{{2, 87, 4, 0}, cyclic_prefix::EXTENDED, 32, 128, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 71, 7.3, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output31.dat"}},
  {{{2, 304, 5, 3}, cyclic_prefix::EXTENDED, 53, 208, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 497, 4.7, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output32.dat"}},
  {{{2, 776, 2, 3}, cyclic_prefix::EXTENDED, 35, 222, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 5, 1.6, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output33.dat"}},
  {{{2, 878, 3, 3}, cyclic_prefix::NORMAL, 81, 190, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 940, 5.2, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output34.dat"}},
  {{{2, 389, 0, 3}, cyclic_prefix::NORMAL, 17, 66, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 1011, 4.7, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output35.dat"}},
  {{{2, 765, 4, 2}, cyclic_prefix::NORMAL, 119, 124, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 613, 4.3, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output36.dat"}},
  {{{2, 317, 7, 0}, cyclic_prefix::NORMAL, 47, 188, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 900, 0.7, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output37.dat"}},
  {{{2, 640, 0, 2}, cyclic_prefix::EXTENDED, 4, 268, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 310, 5.5, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output38.dat"}},
  {{{2, 373, 5, 0}, cyclic_prefix::EXTENDED, 25, 219, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 1014, 3.8, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output39.dat"}},
  {{{2, 34, 7, 3}, cyclic_prefix::EXTENDED, 46, 93, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 664, 9.7, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output40.dat"}},
  {{{2, 887, 5, 0}, cyclic_prefix::EXTENDED, 85, 114, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 293, 6.9, make_nzp_csi(1)}, {"test_data/nzp_csi_rs_generator_test_output41.dat"}},
  {{{0, 254, 6, 0}, cyclic_prefix::NORMAL, 63, 190, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 267, 2, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output42.dat"}},
  {{{0, 217, 7, 0}, cyclic_prefix::NORMAL, 16, 164, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 586, 4.1, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output43.dat"}},
  {{{0, 562, 8, 0}, cyclic_prefix::NORMAL, 52, 126, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 424, 3.3, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output44.dat"}},
  {{{0, 972, 7, 0}, cyclic_prefix::NORMAL, 198, 28, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 277, 6.9, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output45.dat"}},
  {{{0, 796, 9, 0}, cyclic_prefix::NORMAL, 12, 151, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 57, 2.3, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output46.dat"}},
  {{{0, 831, 8, 0}, cyclic_prefix::NORMAL, 49, 42, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 27, 3.4, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output47.dat"}},
  {{{0, 978, 0, 0}, cyclic_prefix::NORMAL, 18, 195, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 163, 7.5, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output48.dat"}},
  {{{0, 379, 6, 0}, cyclic_prefix::NORMAL, 10, 187, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 990, 2.5, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output49.dat"}},
  {{{0, 109, 9, 0}, cyclic_prefix::NORMAL, 31, 224, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 1022, 2.2, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output50.dat"}},
  {{{0, 23, 3, 0}, cyclic_prefix::NORMAL, 66, 155, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 480, 4.5, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output51.dat"}},
  {{{0, 200, 8, 0}, cyclic_prefix::NORMAL, 31, 189, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 194, 2.6, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output52.dat"}},
  {{{0, 917, 9, 0}, cyclic_prefix::NORMAL, 85, 26, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 90, 6.4, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output53.dat"}},
  {{{2, 177, 7, 2}, cyclic_prefix::NORMAL, 18, 24, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 261, 1.6, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output54.dat"}},
  {{{2, 662, 7, 3}, cyclic_prefix::NORMAL, 262, 6, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 8, 8.2, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output55.dat"}},
  {{{2, 942, 0, 1}, cyclic_prefix::NORMAL, 76, 167, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 79, 7.3, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output56.dat"}},
  {{{2, 714, 8, 3}, cyclic_prefix::NORMAL, 104, 166, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 827, 9.7, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output57.dat"}},
  {{{2, 716, 6, 0}, cyclic_prefix::EXTENDED, 21, 109, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 664, 5.6, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output58.dat"}},
  {{{2, 50, 7, 0}, cyclic_prefix::EXTENDED, 211, 38, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 56, 2.7, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output59.dat"}},
  {{{2, 954, 0, 0}, cyclic_prefix::EXTENDED, 234, 38, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 330, 0.3, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output60.dat"}},
  {{{2, 229, 9, 2}, cyclic_prefix::EXTENDED, 81, 39, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 942, 4, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output61.dat"}},
  {{{2, 614, 5, 0}, cyclic_prefix::NORMAL, 234, 28, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 550, 8.2, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output62.dat"}},
  {{{2, 716, 9, 3}, cyclic_prefix::NORMAL, 232, 28, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 49, 9.5, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output63.dat"}},
  {{{2, 206, 3, 1}, cyclic_prefix::NORMAL, 95, 10, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 500, 1.8, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output64.dat"}},
  {{{2, 25, 6, 2}, cyclic_prefix::NORMAL, 4, 199, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 621, 0.1, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output65.dat"}},
  {{{2, 452, 6, 0}, cyclic_prefix::EXTENDED, 33, 45, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 999, 7.3, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output66.dat"}},
  {{{2, 329, 3, 2}, cyclic_prefix::EXTENDED, 105, 58, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 154, 8.5, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output67.dat"}},
  {{{2, 611, 3, 0}, cyclic_prefix::EXTENDED, 81, 96, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 67, 3, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output68.dat"}},
  {{{2, 149, 9, 3}, cyclic_prefix::EXTENDED, 0, 270, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 432, 7, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output69.dat"}},
  {{{2, 927, 1, 2}, cyclic_prefix::NORMAL, 83, 97, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 357, 5.8, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output70.dat"}},
  {{{2, 984, 4, 3}, cyclic_prefix::NORMAL, 79, 131, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 716, 9.1, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output71.dat"}},
  {{{2, 719, 6, 0}, cyclic_prefix::NORMAL, 174, 81, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 803, 9.4, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output72.dat"}},
  {{{2, 343, 6, 0}, cyclic_prefix::NORMAL, 17, 129, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 958, 5.3, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output73.dat"}},
  {{{2, 244, 3, 0}, cyclic_prefix::EXTENDED, 37, 176, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 54, 7.9, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output74.dat"}},
  {{{2, 631, 0, 3}, cyclic_prefix::EXTENDED, 119, 75, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 976, 2.4, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output75.dat"}},
  {{{2, 408, 4, 1}, cyclic_prefix::EXTENDED, 9, 165, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 975, 1.9, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output76.dat"}},
  {{{2, 369, 9, 3}, cyclic_prefix::EXTENDED, 58, 41, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 187, 4.2, make_nzp_csi(2)}, {"test_data/nzp_csi_rs_generator_test_output77.dat"}},
  {{{0, 214, 4, 0}, cyclic_prefix::NORMAL, 1, 163, 4, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 102, 7.3, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output78.dat"}},
  {{{0, 333, 2, 0}, cyclic_prefix::NORMAL, 15, 251, 4, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 102, 2.6, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output79.dat"}},
  {{{0, 334, 3, 0}, cyclic_prefix::NORMAL, 4, 231, 4, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 142, 2.1, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output80.dat"}},
  {{{0, 586, 8, 0}, cyclic_prefix::NORMAL, 3, 89, 4, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 430, 6.9, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output81.dat"}},
  {{{2, 108, 5, 3}, cyclic_prefix::NORMAL, 226, 19, 4, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 281, 4.8, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output82.dat"}},
  {{{2, 562, 2, 0}, cyclic_prefix::NORMAL, 90, 136, 4, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 119, 2.3, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output83.dat"}},
  {{{2, 707, 4, 2}, cyclic_prefix::NORMAL, 14, 254, 4, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 10, 9.3, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output84.dat"}},
  {{{2, 418, 0, 2}, cyclic_prefix::NORMAL, 69, 199, 4, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 687, 4.7, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output85.dat"}},
  {{{2, 79, 4, 2}, cyclic_prefix::EXTENDED, 144, 111, 4, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 553, 2.6, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output86.dat"}},
  {{{2, 234, 1, 2}, cyclic_prefix::EXTENDED, 111, 89, 4, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 483, 7.8, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output87.dat"}},
  {{{2, 287, 6, 2}, cyclic_prefix::EXTENDED, 179, 36, 4, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 22, 4.7, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output88.dat"}},
  {{{2, 739, 8, 2}, cyclic_prefix::EXTENDED, 168, 21, 4, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 692, 0.4, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output89.dat"}},
  {{{0, 346, 4, 0}, cyclic_prefix::NORMAL, 67, 90, 5, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 425, 9.7, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output90.dat"}},
  {{{0, 459, 2, 0}, cyclic_prefix::NORMAL, 26, 207, 5, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 371, 4.2, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output91.dat"}},
  {{{0, 752, 9, 0}, cyclic_prefix::NORMAL, 68, 203, 5, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 145, 2.2, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output92.dat"}},
  {{{0, 961, 8, 0}, cyclic_prefix::NORMAL, 113, 67, 5, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 635, 9.7, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output93.dat"}},
  {{{2, 97, 2, 1}, cyclic_prefix::NORMAL, 63, 194, 5, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 744, 0.5, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output94.dat"}},
  {{{2, 1023, 8, 1}, cyclic_prefix::NORMAL, 9, 234, 5, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 1016, 3.9, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output95.dat"}},
  {{{2, 881, 4, 0}, cyclic_prefix::NORMAL, 60, 155, 5, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 269, 8.3, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output96.dat"}},
  {{{2, 397, 7, 3}, cyclic_prefix::NORMAL, 11, 100, 5, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 247, 5.6, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output97.dat"}},
  {{{2, 576, 1, 1}, cyclic_prefix::EXTENDED, 41, 205, 5, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 760, 1.7, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output98.dat"}},
  {{{2, 955, 2, 2}, cyclic_prefix::EXTENDED, 21, 238, 5, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 72, 8.5, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output99.dat"}},
  {{{2, 589, 0, 0}, cyclic_prefix::EXTENDED, 51, 219, 5, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 795, 3.4, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output100.dat"}},
  {{{2, 1015, 1, 3}, cyclic_prefix::EXTENDED, 98, 54, 5, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 29, 3.3, make_nzp_csi(4)}, {"test_data/nzp_csi_rs_generator_test_output101.dat"}},
    // clang-format on
};

} // namespace srsran
