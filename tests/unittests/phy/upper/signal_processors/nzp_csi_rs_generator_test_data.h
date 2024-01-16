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
  {{{0, 927, 1, 0}, cyclic_prefix::NORMAL, 15, 249, 1, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 99, 2.8, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output0.dat"}},
  {{{0, 980, 9, 0}, cyclic_prefix::NORMAL, 220, 46, 1, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 980, 4.9, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output1.dat"}},
  {{{2, 1003, 4, 1}, cyclic_prefix::NORMAL, 61, 33, 1, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 418, 6, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output2.dat"}},
  {{{2, 617, 7, 0}, cyclic_prefix::NORMAL, 24, 63, 1, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 303, 3.2, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output3.dat"}},
  {{{2, 237, 4, 3}, cyclic_prefix::EXTENDED, 69, 171, 1, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 405, 3.7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output4.dat"}},
  {{{2, 38, 8, 3}, cyclic_prefix::EXTENDED, 19, 249, 1, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::three, 101, 2.7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output5.dat"}},
  {{{0, 122, 9, 0}, cyclic_prefix::NORMAL, 46, 177, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 654, 5.5, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output6.dat"}},
  {{{0, 556, 7, 0}, cyclic_prefix::NORMAL, 128, 144, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 223, 1.1, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output7.dat"}},
  {{{0, 65, 4, 0}, cyclic_prefix::NORMAL, 54, 124, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 781, 6.3, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output8.dat"}},
  {{{0, 955, 9, 0}, cyclic_prefix::NORMAL, 30, 55, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 712, 1, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output9.dat"}},
  {{{0, 440, 8, 0}, cyclic_prefix::NORMAL, 126, 109, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 406, 8.1, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output10.dat"}},
  {{{0, 386, 2, 0}, cyclic_prefix::NORMAL, 54, 216, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 335, 6.8, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output11.dat"}},
  {{{0, 853, 7, 0}, cyclic_prefix::NORMAL, 193, 48, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 1013, 5.2, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output12.dat"}},
  {{{0, 602, 1, 0}, cyclic_prefix::NORMAL, 87, 57, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 766, 8.3, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output13.dat"}},
  {{{0, 922, 5, 0}, cyclic_prefix::NORMAL, 31, 231, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 600, 2.5, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output14.dat"}},
  {{{0, 85, 6, 0}, cyclic_prefix::NORMAL, 67, 181, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 912, 9.9, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output15.dat"}},
  {{{0, 595, 9, 0}, cyclic_prefix::NORMAL, 1, 160, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 123, 8.7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output16.dat"}},
  {{{0, 865, 2, 0}, cyclic_prefix::NORMAL, 76, 152, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 32, 6.2, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output17.dat"}},
  {{{2, 479, 9, 0}, cyclic_prefix::NORMAL, 180, 31, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 753, 5.7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output18.dat"}},
  {{{2, 611, 2, 3}, cyclic_prefix::NORMAL, 49, 40, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 916, 0.8, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output19.dat"}},
  {{{2, 55, 4, 1}, cyclic_prefix::NORMAL, 238, 7, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 201, 1, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output20.dat"}},
  {{{2, 467, 1, 0}, cyclic_prefix::NORMAL, 0, 271, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 304, 0.7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output21.dat"}},
  {{{2, 47, 5, 0}, cyclic_prefix::EXTENDED, 41, 208, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 92, 0.9, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output22.dat"}},
  {{{2, 926, 5, 1}, cyclic_prefix::EXTENDED, 198, 33, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 346, 3, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output23.dat"}},
  {{{2, 10, 0, 1}, cyclic_prefix::EXTENDED, 54, 183, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 538, 7.3, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output24.dat"}},
  {{{2, 800, 2, 3}, cyclic_prefix::EXTENDED, 46, 190, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_odd_RB, 406, 0.7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output25.dat"}},
  {{{2, 345, 6, 0}, cyclic_prefix::NORMAL, 7, 203, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 130, 5.5, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output26.dat"}},
  {{{2, 911, 7, 3}, cyclic_prefix::NORMAL, 3, 201, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 74, 0.9, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output27.dat"}},
  {{{2, 965, 6, 3}, cyclic_prefix::NORMAL, 169, 39, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 113, 1.2, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output28.dat"}},
  {{{2, 336, 6, 2}, cyclic_prefix::NORMAL, 39, 205, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 757, 2.4, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output29.dat"}},
  {{{2, 993, 8, 2}, cyclic_prefix::EXTENDED, 90, 27, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 378, 6.9, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output30.dat"}},
  {{{2, 808, 3, 2}, cyclic_prefix::EXTENDED, 18, 59, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 790, 2.1, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output31.dat"}},
  {{{2, 565, 2, 1}, cyclic_prefix::EXTENDED, 46, 176, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 155, 7.9, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output32.dat"}},
  {{{2, 301, 2, 1}, cyclic_prefix::EXTENDED, 11, 146, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::dot5_even_RB, 415, 1.1, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output33.dat"}},
  {{{2, 803, 2, 3}, cyclic_prefix::NORMAL, 103, 166, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 442, 7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output34.dat"}},
  {{{2, 443, 6, 2}, cyclic_prefix::NORMAL, 224, 33, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 191, 2.7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output35.dat"}},
  {{{2, 499, 7, 2}, cyclic_prefix::NORMAL, 44, 110, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 38, 6.8, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output36.dat"}},
  {{{2, 462, 6, 0}, cyclic_prefix::NORMAL, 80, 19, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 791, 7, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output37.dat"}},
  {{{2, 133, 0, 3}, cyclic_prefix::EXTENDED, 112, 6, 2, {0}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 671, 7.3, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output38.dat"}},
  {{{2, 111, 6, 1}, cyclic_prefix::EXTENDED, 31, 38, 2, {0}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 100, 1.5, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output39.dat"}},
  {{{2, 200, 3, 0}, cyclic_prefix::EXTENDED, 40, 89, 2, {6}, 0, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 257, 9, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output40.dat"}},
  {{{2, 569, 1, 3}, cyclic_prefix::EXTENDED, 16, 61, 2, {6}, 8, 0, csi_rs_cdm_type::no_CDM, csi_rs_freq_density_type::one, 935, 7.1, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/nzp_csi_rs_generator_test_output41.dat"}},
  {{{0, 528, 7, 0}, cyclic_prefix::NORMAL, 217, 45, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 553, 6.8, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output42.dat"}},
  {{{0, 828, 7, 0}, cyclic_prefix::NORMAL, 124, 36, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 333, 5.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output43.dat"}},
  {{{0, 425, 1, 0}, cyclic_prefix::NORMAL, 4, 72, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 945, 6.6, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output44.dat"}},
  {{{0, 167, 9, 0}, cyclic_prefix::NORMAL, 32, 217, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 450, 2.6, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output45.dat"}},
  {{{0, 431, 1, 0}, cyclic_prefix::NORMAL, 27, 199, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 861, 7.4, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output46.dat"}},
  {{{0, 181, 9, 0}, cyclic_prefix::NORMAL, 183, 75, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 229, 3.8, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output47.dat"}},
  {{{0, 655, 1, 0}, cyclic_prefix::NORMAL, 185, 16, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 355, 6.7, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output48.dat"}},
  {{{0, 642, 0, 0}, cyclic_prefix::NORMAL, 20, 248, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 763, 8.2, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output49.dat"}},
  {{{0, 631, 9, 0}, cyclic_prefix::NORMAL, 71, 99, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 1007, 9.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output50.dat"}},
  {{{0, 1012, 7, 0}, cyclic_prefix::NORMAL, 118, 94, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 250, 3, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output51.dat"}},
  {{{0, 540, 4, 0}, cyclic_prefix::NORMAL, 80, 166, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 597, 5.6, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output52.dat"}},
  {{{0, 524, 0, 0}, cyclic_prefix::NORMAL, 75, 197, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 363, 9.8, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output53.dat"}},
  {{{2, 32, 3, 2}, cyclic_prefix::NORMAL, 25, 182, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 235, 7.2, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output54.dat"}},
  {{{2, 604, 6, 2}, cyclic_prefix::NORMAL, 89, 16, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 462, 2.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output55.dat"}},
  {{{2, 876, 2, 3}, cyclic_prefix::NORMAL, 10, 200, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 856, 1.4, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output56.dat"}},
  {{{2, 374, 8, 0}, cyclic_prefix::NORMAL, 65, 139, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 898, 3.6, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output57.dat"}},
  {{{2, 986, 0, 1}, cyclic_prefix::EXTENDED, 1, 265, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 683, 5.9, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output58.dat"}},
  {{{2, 369, 6, 0}, cyclic_prefix::EXTENDED, 0, 222, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 85, 9.8, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output59.dat"}},
  {{{2, 236, 4, 0}, cyclic_prefix::EXTENDED, 63, 36, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 264, 3.4, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output60.dat"}},
  {{{2, 356, 1, 0}, cyclic_prefix::EXTENDED, 3, 241, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_odd_RB, 952, 4, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output61.dat"}},
  {{{2, 350, 7, 1}, cyclic_prefix::NORMAL, 30, 217, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 702, 9, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output62.dat"}},
  {{{2, 310, 0, 1}, cyclic_prefix::NORMAL, 156, 56, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 739, 8.8, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output63.dat"}},
  {{{2, 72, 9, 0}, cyclic_prefix::NORMAL, 15, 219, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 556, 9.9, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output64.dat"}},
  {{{2, 859, 4, 1}, cyclic_prefix::NORMAL, 80, 130, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 275, 7.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output65.dat"}},
  {{{2, 662, 3, 0}, cyclic_prefix::EXTENDED, 110, 41, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 371, 7.9, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output66.dat"}},
  {{{2, 684, 1, 1}, cyclic_prefix::EXTENDED, 147, 9, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 308, 9.4, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output67.dat"}},
  {{{2, 293, 8, 0}, cyclic_prefix::EXTENDED, 16, 245, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 905, 9.5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output68.dat"}},
  {{{2, 745, 5, 3}, cyclic_prefix::EXTENDED, 117, 10, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::dot5_even_RB, 661, 5.3, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output69.dat"}},
  {{{2, 959, 8, 1}, cyclic_prefix::NORMAL, 15, 232, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 607, 8.8, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output70.dat"}},
  {{{2, 684, 2, 0}, cyclic_prefix::NORMAL, 6, 179, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 416, 6.7, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output71.dat"}},
  {{{2, 830, 4, 3}, cyclic_prefix::NORMAL, 27, 207, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 995, 9.9, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output72.dat"}},
  {{{2, 398, 4, 2}, cyclic_prefix::NORMAL, 159, 70, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 904, 9.2, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output73.dat"}},
  {{{2, 613, 1, 1}, cyclic_prefix::EXTENDED, 12, 246, 3, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 210, 9, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output74.dat"}},
  {{{2, 903, 2, 3}, cyclic_prefix::EXTENDED, 58, 185, 3, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 125, 4.1, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output75.dat"}},
  {{{2, 733, 2, 3}, cyclic_prefix::EXTENDED, 23, 245, 3, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 399, 5, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output76.dat"}},
  {{{2, 854, 6, 0}, cyclic_prefix::EXTENDED, 37, 158, 3, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 467, 7.2, precoding_configuration::make_wideband(make_identity(2))}, {"test_data/nzp_csi_rs_generator_test_output77.dat"}},
  {{{0, 403, 0, 0}, cyclic_prefix::NORMAL, 0, 63, 4, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 193, 1.5, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output78.dat"}},
  {{{0, 179, 1, 0}, cyclic_prefix::NORMAL, 97, 165, 4, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 961, 2.3, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output79.dat"}},
  {{{0, 385, 5, 0}, cyclic_prefix::NORMAL, 13, 75, 4, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 446, 1.8, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output80.dat"}},
  {{{0, 977, 4, 0}, cyclic_prefix::NORMAL, 8, 262, 4, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 7, 6.9, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output81.dat"}},
  {{{2, 423, 3, 0}, cyclic_prefix::NORMAL, 150, 74, 4, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 1019, 1.9, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output82.dat"}},
  {{{2, 200, 9, 3}, cyclic_prefix::NORMAL, 22, 219, 4, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 746, 5, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output83.dat"}},
  {{{2, 365, 0, 2}, cyclic_prefix::NORMAL, 101, 162, 4, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 198, 4.4, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output84.dat"}},
  {{{2, 40, 9, 1}, cyclic_prefix::NORMAL, 35, 209, 4, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 188, 5, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output85.dat"}},
  {{{2, 1018, 8, 2}, cyclic_prefix::EXTENDED, 7, 262, 4, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 413, 9.4, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output86.dat"}},
  {{{2, 237, 3, 3}, cyclic_prefix::EXTENDED, 44, 193, 4, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 774, 10, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output87.dat"}},
  {{{2, 547, 9, 2}, cyclic_prefix::EXTENDED, 12, 35, 4, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 311, 5.9, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output88.dat"}},
  {{{2, 922, 5, 1}, cyclic_prefix::EXTENDED, 83, 120, 4, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 729, 0.2, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output89.dat"}},
  {{{0, 1015, 7, 0}, cyclic_prefix::NORMAL, 1, 267, 5, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 541, 0.6, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output90.dat"}},
  {{{0, 616, 8, 0}, cyclic_prefix::NORMAL, 3, 269, 5, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 419, 0.1, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output91.dat"}},
  {{{0, 212, 2, 0}, cyclic_prefix::NORMAL, 17, 91, 5, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 765, 7.5, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output92.dat"}},
  {{{0, 346, 8, 0}, cyclic_prefix::NORMAL, 115, 152, 5, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 914, 3.6, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output93.dat"}},
  {{{2, 641, 5, 1}, cyclic_prefix::NORMAL, 69, 178, 5, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 96, 8.8, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output94.dat"}},
  {{{2, 301, 1, 3}, cyclic_prefix::NORMAL, 1, 253, 5, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 595, 6.4, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output95.dat"}},
  {{{2, 885, 0, 2}, cyclic_prefix::NORMAL, 26, 223, 5, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 711, 2.2, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output96.dat"}},
  {{{2, 719, 9, 2}, cyclic_prefix::NORMAL, 12, 123, 5, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 58, 6.3, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output97.dat"}},
  {{{2, 707, 3, 1}, cyclic_prefix::EXTENDED, 7, 258, 5, {0}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 976, 0.8, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output98.dat"}},
  {{{2, 793, 9, 0}, cyclic_prefix::EXTENDED, 17, 214, 5, {0}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 155, 8.5, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output99.dat"}},
  {{{2, 277, 2, 1}, cyclic_prefix::EXTENDED, 151, 90, 5, {6}, 0, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 841, 5.8, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output100.dat"}},
  {{{2, 292, 6, 3}, cyclic_prefix::EXTENDED, 24, 218, 5, {6}, 8, 0, csi_rs_cdm_type::fd_CDM2, csi_rs_freq_density_type::one, 456, 4.7, precoding_configuration::make_wideband(make_identity(4))}, {"test_data/nzp_csi_rs_generator_test_output101.dat"}},
    // clang-format on
};

} // namespace srsran
