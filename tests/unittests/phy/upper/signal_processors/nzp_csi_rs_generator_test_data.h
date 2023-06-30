/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

// This file was generated using the following MATLAB class on 23-06-2023:
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
  {{{0, 466, 2, 0}, cyclic_prefix::NORMAL, 27, 97, 1, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 395, 0.3, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output0.dat"}},
  {{{0, 96, 7, 0}, cyclic_prefix::NORMAL, 14, 14, 1, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 236, 4.3, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output1.dat"}},
  {{{2, 672, 8, 1}, cyclic_prefix::NORMAL, 46, 55, 1, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 774, 4, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output2.dat"}},
  {{{2, 155, 9, 3}, cyclic_prefix::NORMAL, 8, 101, 1, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 89, 6.8, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output3.dat"}},
  {{{2, 420, 7, 3}, cyclic_prefix::EXTENDED, 86, 179, 1, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 680, 9.2, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output4.dat"}},
  {{{2, 446, 9, 3}, cyclic_prefix::EXTENDED, 7, 58, 1, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 908, 5.2, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output5.dat"}},
  {{{0, 125, 5, 0}, cyclic_prefix::NORMAL, 63, 163, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 659, 2.9, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output6.dat"}},
  {{{0, 481, 0, 0}, cyclic_prefix::NORMAL, 12, 224, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 786, 1.5, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output7.dat"}},
  {{{0, 986, 3, 0}, cyclic_prefix::NORMAL, 9, 259, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 838, 2.2, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output8.dat"}},
  {{{0, 465, 5, 0}, cyclic_prefix::NORMAL, 139, 130, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 642, 8, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output9.dat"}},
  {{{0, 290, 4, 0}, cyclic_prefix::NORMAL, 177, 88, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 749, 0.4, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output10.dat"}},
  {{{0, 358, 6, 0}, cyclic_prefix::NORMAL, 186, 48, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 652, 2.2, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output11.dat"}},
  {{{0, 45, 7, 0}, cyclic_prefix::NORMAL, 52, 182, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 758, 3.7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output12.dat"}},
  {{{0, 691, 1, 0}, cyclic_prefix::NORMAL, 13, 15, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 410, 7.2, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output13.dat"}},
  {{{0, 246, 8, 0}, cyclic_prefix::NORMAL, 60, 194, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 32, 6.3, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output14.dat"}},
  {{{0, 449, 0, 0}, cyclic_prefix::NORMAL, 6, 153, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 1021, 9.3, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output15.dat"}},
  {{{0, 923, 2, 0}, cyclic_prefix::NORMAL, 37, 6, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 211, 7.4, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output16.dat"}},
  {{{0, 509, 0, 0}, cyclic_prefix::NORMAL, 83, 75, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 788, 2.7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output17.dat"}},
  {{{2, 180, 6, 0}, cyclic_prefix::NORMAL, 24, 230, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 692, 6, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output18.dat"}},
  {{{2, 17, 4, 0}, cyclic_prefix::NORMAL, 63, 138, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 1006, 7.4, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output19.dat"}},
  {{{2, 197, 0, 0}, cyclic_prefix::NORMAL, 74, 134, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 408, 6.5, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output20.dat"}},
  {{{2, 215, 4, 0}, cyclic_prefix::NORMAL, 46, 215, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 531, 9, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output21.dat"}},
  {{{2, 846, 3, 0}, cyclic_prefix::EXTENDED, 4, 262, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 57, 0.1, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output22.dat"}},
  {{{2, 12, 4, 0}, cyclic_prefix::EXTENDED, 2, 268, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 420, 6.5, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output23.dat"}},
  {{{2, 297, 0, 1}, cyclic_prefix::EXTENDED, 18, 245, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 272, 3.3, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output24.dat"}},
  {{{2, 490, 9, 0}, cyclic_prefix::EXTENDED, 106, 16, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 712, 1.6, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output25.dat"}},
  {{{2, 350, 6, 3}, cyclic_prefix::NORMAL, 58, 196, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 493, 7.3, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output26.dat"}},
  {{{2, 24, 9, 0}, cyclic_prefix::NORMAL, 27, 86, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 263, 4.1, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output27.dat"}},
  {{{2, 455, 6, 3}, cyclic_prefix::NORMAL, 32, 193, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 733, 7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output28.dat"}},
  {{{2, 165, 0, 1}, cyclic_prefix::NORMAL, 32, 212, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 89, 1.8, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output29.dat"}},
  {{{2, 412, 3, 2}, cyclic_prefix::EXTENDED, 57, 56, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 618, 0.1, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output30.dat"}},
  {{{2, 402, 0, 2}, cyclic_prefix::EXTENDED, 77, 79, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 379, 1.1, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output31.dat"}},
  {{{2, 553, 8, 3}, cyclic_prefix::EXTENDED, 111, 31, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 2, 7.8, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output32.dat"}},
  {{{2, 684, 6, 0}, cyclic_prefix::EXTENDED, 12, 219, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 69, 9.7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output33.dat"}},
  {{{2, 979, 5, 3}, cyclic_prefix::NORMAL, 16, 103, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 342, 7.6, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output34.dat"}},
  {{{2, 200, 1, 0}, cyclic_prefix::NORMAL, 6, 200, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 535, 5.4, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output35.dat"}},
  {{{2, 702, 9, 0}, cyclic_prefix::NORMAL, 22, 103, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 570, 7.4, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output36.dat"}},
  {{{2, 820, 0, 0}, cyclic_prefix::NORMAL, 100, 76, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 783, 3.9, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output37.dat"}},
  {{{2, 258, 8, 0}, cyclic_prefix::EXTENDED, 23, 235, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 272, 7.8, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output38.dat"}},
  {{{2, 378, 0, 2}, cyclic_prefix::EXTENDED, 55, 212, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 911, 5.7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output39.dat"}},
  {{{2, 824, 9, 3}, cyclic_prefix::EXTENDED, 26, 237, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 1019, 3.8, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output40.dat"}},
  {{{2, 537, 4, 1}, cyclic_prefix::EXTENDED, 44, 68, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 345, 7.8, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output41.dat"}},
  {{{0, 800, 1, 0}, cyclic_prefix::NORMAL, 9, 102, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 194, 9.7, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output42.dat"}},
  {{{0, 361, 4, 0}, cyclic_prefix::NORMAL, 229, 4, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 709, 4, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output43.dat"}},
  {{{0, 965, 0, 0}, cyclic_prefix::NORMAL, 48, 170, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 30, 1.1, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output44.dat"}},
  {{{0, 704, 0, 0}, cyclic_prefix::NORMAL, 20, 252, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 12, 4.1, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output45.dat"}},
  {{{0, 756, 8, 0}, cyclic_prefix::NORMAL, 59, 157, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 606, 0.3, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output46.dat"}},
  {{{0, 376, 7, 0}, cyclic_prefix::NORMAL, 133, 11, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 221, 8.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output47.dat"}},
  {{{0, 976, 0, 0}, cyclic_prefix::NORMAL, 20, 236, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 929, 5.9, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output48.dat"}},
  {{{0, 635, 3, 0}, cyclic_prefix::NORMAL, 34, 9, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 993, 6.7, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output49.dat"}},
  {{{0, 422, 2, 0}, cyclic_prefix::NORMAL, 9, 262, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 529, 9.1, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output50.dat"}},
  {{{0, 895, 4, 0}, cyclic_prefix::NORMAL, 25, 164, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 881, 1.7, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output51.dat"}},
  {{{0, 126, 6, 0}, cyclic_prefix::NORMAL, 147, 39, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 783, 5.2, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output52.dat"}},
  {{{0, 232, 1, 0}, cyclic_prefix::NORMAL, 24, 234, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 473, 7.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output53.dat"}},
  {{{2, 840, 6, 3}, cyclic_prefix::NORMAL, 21, 124, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 842, 4.8, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output54.dat"}},
  {{{2, 31, 0, 3}, cyclic_prefix::NORMAL, 92, 107, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 524, 3.8, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output55.dat"}},
  {{{2, 557, 8, 3}, cyclic_prefix::NORMAL, 103, 121, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 550, 8.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output56.dat"}},
  {{{2, 378, 3, 0}, cyclic_prefix::NORMAL, 69, 171, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 988, 6.7, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output57.dat"}},
  {{{2, 508, 8, 0}, cyclic_prefix::EXTENDED, 7, 82, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 655, 0.6, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output58.dat"}},
  {{{2, 871, 6, 1}, cyclic_prefix::EXTENDED, 22, 149, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 846, 3.7, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output59.dat"}},
  {{{2, 723, 3, 1}, cyclic_prefix::EXTENDED, 53, 89, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 38, 1.8, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output60.dat"}},
  {{{2, 773, 6, 2}, cyclic_prefix::EXTENDED, 38, 34, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 875, 1.2, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output61.dat"}},
  {{{2, 603, 8, 1}, cyclic_prefix::NORMAL, 64, 164, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 308, 3.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output62.dat"}},
  {{{2, 802, 5, 0}, cyclic_prefix::NORMAL, 4, 250, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 910, 4.2, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output63.dat"}},
  {{{2, 979, 8, 0}, cyclic_prefix::NORMAL, 71, 5, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 452, 4.9, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output64.dat"}},
  {{{2, 608, 6, 1}, cyclic_prefix::NORMAL, 158, 24, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 340, 8.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output65.dat"}},
  {{{2, 46, 1, 0}, cyclic_prefix::EXTENDED, 0, 252, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 95, 4.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output66.dat"}},
  {{{2, 198, 9, 2}, cyclic_prefix::EXTENDED, 137, 26, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 865, 1.2, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output67.dat"}},
  {{{2, 311, 4, 0}, cyclic_prefix::EXTENDED, 33, 227, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 935, 9, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output68.dat"}},
  {{{2, 420, 8, 3}, cyclic_prefix::EXTENDED, 25, 207, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 194, 4.2, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output69.dat"}},
  {{{2, 255, 5, 3}, cyclic_prefix::NORMAL, 15, 246, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 610, 7.8, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output70.dat"}},
  {{{2, 558, 7, 3}, cyclic_prefix::NORMAL, 4, 173, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 955, 9.9, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output71.dat"}},
  {{{2, 1019, 4, 2}, cyclic_prefix::NORMAL, 37, 101, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 247, 2, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output72.dat"}},
  {{{2, 260, 2, 2}, cyclic_prefix::NORMAL, 197, 59, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 724, 8.7, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output73.dat"}},
  {{{2, 909, 6, 3}, cyclic_prefix::EXTENDED, 14, 142, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 277, 9.8, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output74.dat"}},
  {{{2, 407, 9, 1}, cyclic_prefix::EXTENDED, 8, 242, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 460, 0.1, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output75.dat"}},
  {{{2, 880, 9, 2}, cyclic_prefix::EXTENDED, 165, 66, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 440, 7.9, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output76.dat"}},
  {{{2, 371, 5, 1}, cyclic_prefix::EXTENDED, 96, 169, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 178, 0.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output77.dat"}},
  {{{0, 974, 1, 0}, cyclic_prefix::NORMAL, 1, 263, 4, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 356, 5.1, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output78.dat"}},
  {{{0, 983, 4, 0}, cyclic_prefix::NORMAL, 2, 266, 4, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 279, 6.9, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output79.dat"}},
  {{{0, 686, 2, 0}, cyclic_prefix::NORMAL, 166, 24, 4, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 450, 6, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output80.dat"}},
  {{{0, 697, 1, 0}, cyclic_prefix::NORMAL, 10, 87, 4, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 544, 10, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output81.dat"}},
  {{{2, 718, 3, 1}, cyclic_prefix::NORMAL, 64, 188, 4, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 785, 5, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output82.dat"}},
  {{{2, 440, 6, 3}, cyclic_prefix::NORMAL, 76, 103, 4, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 895, 7.8, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output83.dat"}},
  {{{2, 489, 7, 1}, cyclic_prefix::NORMAL, 110, 122, 4, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 730, 9.5, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output84.dat"}},
  {{{2, 338, 0, 3}, cyclic_prefix::NORMAL, 42, 192, 4, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 595, 5.3, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output85.dat"}},
  {{{2, 883, 4, 0}, cyclic_prefix::EXTENDED, 23, 216, 4, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 438, 2.4, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output86.dat"}},
  {{{2, 89, 0, 0}, cyclic_prefix::EXTENDED, 33, 97, 4, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 420, 7.7, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output87.dat"}},
  {{{2, 813, 2, 0}, cyclic_prefix::EXTENDED, 60, 194, 4, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 264, 9, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output88.dat"}},
  {{{2, 428, 7, 2}, cyclic_prefix::EXTENDED, 29, 39, 4, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 136, 4.2, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output89.dat"}},
  {{{0, 148, 3, 0}, cyclic_prefix::NORMAL, 120, 59, 5, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 330, 7.7, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output90.dat"}},
  {{{0, 171, 0, 0}, cyclic_prefix::NORMAL, 14, 248, 5, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 880, 2.9, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output91.dat"}},
  {{{0, 445, 6, 0}, cyclic_prefix::NORMAL, 90, 155, 5, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 813, 1.7, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output92.dat"}},
  {{{0, 610, 5, 0}, cyclic_prefix::NORMAL, 18, 224, 5, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 702, 1.9, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output93.dat"}},
  {{{2, 310, 3, 0}, cyclic_prefix::NORMAL, 104, 13, 5, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 566, 6.7, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output94.dat"}},
  {{{2, 765, 6, 0}, cyclic_prefix::NORMAL, 112, 111, 5, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 824, 2.8, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output95.dat"}},
  {{{2, 691, 2, 3}, cyclic_prefix::NORMAL, 3, 163, 5, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 353, 7.9, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output96.dat"}},
  {{{2, 839, 9, 3}, cyclic_prefix::NORMAL, 101, 13, 5, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 371, 9.2, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output97.dat"}},
  {{{2, 431, 9, 3}, cyclic_prefix::EXTENDED, 74, 63, 5, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 366, 8.6, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output98.dat"}},
  {{{2, 882, 7, 3}, cyclic_prefix::EXTENDED, 113, 122, 5, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 106, 3.7, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output99.dat"}},
  {{{2, 864, 4, 2}, cyclic_prefix::EXTENDED, 148, 63, 5, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 70, 3.9, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output100.dat"}},
  {{{2, 884, 9, 1}, cyclic_prefix::EXTENDED, 22, 203, 5, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 884, 5.4, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output101.dat"}},
    // clang-format on
};

} // namespace srsran
