/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

// This file was generated using the following MATLAB class on 15-01-2025 (seed 0):
//   + "srsPRSGeneratorUnittest.m"

#include "../../../support/resource_grid_test_doubles.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator_configuration.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/support/file_vector.h"

namespace srsran {

struct test_case_t {
  prs_generator_configuration                             config;
  file_vector<resource_grid_writer_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> prs_generator_test_data = {
    // clang-format off
  {{{0, 584, 7, 0}, cyclic_prefix::NORMAL, 3447, static_cast<prs_comb_size>(2), 0, static_cast<prs_num_symbols>(2), 9, 2022, {75, 143}, 21.4145, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output0.dat"}},
  {{{0, 46, 1, 0}, cyclic_prefix::NORMAL, 2621, static_cast<prs_comb_size>(2), 1, static_cast<prs_num_symbols>(4), 4, 739, {16, 96}, 24.7266, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output1.dat"}},
  {{{0, 832, 7, 0}, cyclic_prefix::NORMAL, 3279, static_cast<prs_comb_size>(2), 0, static_cast<prs_num_symbols>(6), 0, 1264, {84, 264}, 22.8184, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output2.dat"}},
  {{{0, 1009, 8, 0}, cyclic_prefix::NORMAL, 932, static_cast<prs_comb_size>(2), 1, static_cast<prs_num_symbols>(12), 0, 1154, {81, 173}, 20.2567, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output3.dat"}},
  {{{0, 802, 6, 0}, cyclic_prefix::NORMAL, 74, static_cast<prs_comb_size>(4), 3, static_cast<prs_num_symbols>(4), 0, 1686, {0, 272}, 23.7173, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output4.dat"}},
  {{{0, 631, 1, 0}, cyclic_prefix::NORMAL, 171, static_cast<prs_comb_size>(4), 2, static_cast<prs_num_symbols>(12), 0, 776, {39, 95}, 25.7545, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output5.dat"}},
  {{{0, 678, 3, 0}, cyclic_prefix::NORMAL, 3140, static_cast<prs_comb_size>(6), 5, static_cast<prs_num_symbols>(6), 6, 574, {12, 272}, 21.8976, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output6.dat"}},
  {{{0, 597, 5, 0}, cyclic_prefix::NORMAL, 2390, static_cast<prs_comb_size>(6), 3, static_cast<prs_num_symbols>(12), 1, 271, {111, 235}, 23.59, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output7.dat"}},
  {{{0, 423, 4, 0}, cyclic_prefix::NORMAL, 3631, static_cast<prs_comb_size>(12), 11, static_cast<prs_num_symbols>(12), 2, 307, {55, 167}, 21.711, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output8.dat"}},
  {{{1, 789, 9, 0}, cyclic_prefix::NORMAL, 3453, static_cast<prs_comb_size>(2), 1, static_cast<prs_num_symbols>(2), 9, 740, {0, 220}, 20.3629, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output9.dat"}},
  {{{1, 242, 6, 0}, cyclic_prefix::NORMAL, 1955, static_cast<prs_comb_size>(2), 1, static_cast<prs_num_symbols>(4), 2, 1761, {129, 209}, 21.4164, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output10.dat"}},
  {{{1, 917, 1, 1}, cyclic_prefix::NORMAL, 2006, static_cast<prs_comb_size>(2), 1, static_cast<prs_num_symbols>(6), 1, 115, {22, 70}, 20.8207, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output11.dat"}},
  {{{1, 928, 5, 0}, cyclic_prefix::NORMAL, 860, static_cast<prs_comb_size>(2), 1, static_cast<prs_num_symbols>(12), 0, 262, {58, 158}, 24.238, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output12.dat"}},
  {{{1, 915, 4, 1}, cyclic_prefix::NORMAL, 3324, static_cast<prs_comb_size>(4), 0, static_cast<prs_num_symbols>(4), 4, 866, {124, 272}, 21.1194, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output13.dat"}},
  {{{1, 748, 1, 0}, cyclic_prefix::NORMAL, 3833, static_cast<prs_comb_size>(4), 3, static_cast<prs_num_symbols>(12), 1, 1651, {25, 225}, 24.3311, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output14.dat"}},
  {{{1, 338, 5, 0}, cyclic_prefix::NORMAL, 4010, static_cast<prs_comb_size>(6), 4, static_cast<prs_num_symbols>(6), 2, 715, {22, 254}, 24.1875, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output15.dat"}},
  {{{1, 756, 8, 0}, cyclic_prefix::NORMAL, 1342, static_cast<prs_comb_size>(6), 2, static_cast<prs_num_symbols>(12), 2, 181, {121, 269}, 25.8193, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output16.dat"}},
  {{{1, 676, 5, 1}, cyclic_prefix::NORMAL, 2558, static_cast<prs_comb_size>(12), 7, static_cast<prs_num_symbols>(12), 0, 810, {128, 220}, 20.4035, precoding_configuration::make_wideband(make_identity(1))}, {"test_data/prs_generator_test_output17.dat"}},
    // clang-format on
};

} // namespace srsran
