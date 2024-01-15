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

#include "lib/scheduler/support/pdsch/pdsch_dmrs_symbol_mask.h"
#include "srsran/adt/static_vector.h"
#include "fmt/ostream.h"
#include <gtest/gtest.h>
#include <map>

using namespace srsran;

namespace srsran {

std::ostream& operator<<(std::ostream& os, dmrs_typeA_position dmrs_pos)
{
  switch (dmrs_pos) {
    case dmrs_typeA_position::pos2:
      return os << "pos2";
    case dmrs_typeA_position::pos3:
    default:
      return os << "pos3";
  }
}

std::ostream& operator<<(std::ostream& os, dmrs_additional_positions additional_pos)
{
  switch (additional_pos) {
    case dmrs_additional_positions::pos0:
      return os << "addpos0";
    case dmrs_additional_positions::pos1:
      return os << "addpos1";
    case dmrs_additional_positions::pos2:
      return os << "addpos2";
    case dmrs_additional_positions::pos3:
    default:
      return os << "addpos3";
  }
}

std::ostream& operator<<(std::ostream& os, dmrs_symbol_mask mask)
{
  fmt::print(os, "{}", mask);
  return os;
}

} // namespace srsran

namespace {
#define L0 (UINT8_MAX)
#define L1 (UINT8_MAX + 1)

using PdschDmrsSymbolMaskParams = std::tuple<dmrs_typeA_position, unsigned, dmrs_additional_positions, bool, bool>;

class PdschDmrsSymbolMaskFixture : public ::testing::TestWithParam<PdschDmrsSymbolMaskParams>
{};

const std::map<std::pair<unsigned, dmrs_additional_positions>, static_vector<unsigned, 4>>
    pdsch_dmrs_symbol_mask_mapping_type_A_single_table = {
        {{3, dmrs_additional_positions::pos0}, {L0}},         {{3, dmrs_additional_positions::pos1}, {L0}},
        {{3, dmrs_additional_positions::pos2}, {L0}},         {{3, dmrs_additional_positions::pos3}, {L0}},
        {{4, dmrs_additional_positions::pos0}, {L0}},         {{4, dmrs_additional_positions::pos1}, {L0}},
        {{4, dmrs_additional_positions::pos2}, {L0}},         {{4, dmrs_additional_positions::pos3}, {L0}},
        {{5, dmrs_additional_positions::pos0}, {L0}},         {{5, dmrs_additional_positions::pos1}, {L0}},
        {{5, dmrs_additional_positions::pos2}, {L0}},         {{5, dmrs_additional_positions::pos3}, {L0}},
        {{6, dmrs_additional_positions::pos0}, {L0}},         {{6, dmrs_additional_positions::pos1}, {L0}},
        {{6, dmrs_additional_positions::pos2}, {L0}},         {{6, dmrs_additional_positions::pos3}, {L0}},
        {{7, dmrs_additional_positions::pos0}, {L0}},         {{7, dmrs_additional_positions::pos1}, {L0}},
        {{7, dmrs_additional_positions::pos2}, {L0}},         {{7, dmrs_additional_positions::pos3}, {L0}},
        {{8, dmrs_additional_positions::pos0}, {L0}},         {{8, dmrs_additional_positions::pos1}, {L0, 7}},
        {{8, dmrs_additional_positions::pos2}, {L0, 7}},      {{8, dmrs_additional_positions::pos3}, {L0, 7}},
        {{9, dmrs_additional_positions::pos0}, {L0}},         {{9, dmrs_additional_positions::pos1}, {L0, 7}},
        {{9, dmrs_additional_positions::pos2}, {L0, 7}},      {{9, dmrs_additional_positions::pos3}, {L0, 7}},
        {{10, dmrs_additional_positions::pos0}, {L0}},        {{10, dmrs_additional_positions::pos1}, {L0, 9}},
        {{10, dmrs_additional_positions::pos2}, {L0, 6, 9}},  {{10, dmrs_additional_positions::pos3}, {L0, 6, 9}},
        {{11, dmrs_additional_positions::pos0}, {L0}},        {{11, dmrs_additional_positions::pos1}, {L0, 9}},
        {{11, dmrs_additional_positions::pos2}, {L0, 6, 9}},  {{11, dmrs_additional_positions::pos3}, {L0, 6, 9}},
        {{12, dmrs_additional_positions::pos0}, {L0}},        {{12, dmrs_additional_positions::pos1}, {L0, 9}},
        {{12, dmrs_additional_positions::pos2}, {L0, 6, 9}},  {{12, dmrs_additional_positions::pos3}, {L0, 5, 8, 11}},
        {{13, dmrs_additional_positions::pos0}, {L0}},        {{13, dmrs_additional_positions::pos1}, {L0, L1}},
        {{13, dmrs_additional_positions::pos2}, {L0, 7, 11}}, {{13, dmrs_additional_positions::pos3}, {L0, 5, 8, 11}},
        {{14, dmrs_additional_positions::pos0}, {L0}},        {{14, dmrs_additional_positions::pos1}, {L0, L1}},
        {{14, dmrs_additional_positions::pos2}, {L0, 7, 11}}, {{14, dmrs_additional_positions::pos3}, {L0, 5, 8, 11}},
};

TEST_P(PdschDmrsSymbolMaskFixture, ANormal)
{
  // Extract parameters.
  dmrs_typeA_position             typeA_pos                         = std::get<0>(GetParam());
  bounded_integer<uint8_t, 3, 14> last_symbol                       = std::get<1>(GetParam());
  dmrs_additional_positions       additional_position               = std::get<2>(GetParam());
  bool                            lte_crs_match_around              = std::get<3>(GetParam());
  bool                            ue_capable_additional_dmrs_dl_alt = std::get<4>(GetParam());

  // Prepare configuration.
  pdsch_dmrs_symbol_mask_mapping_type_A_single_configuration config;
  config.typeA_pos                         = typeA_pos;
  config.additional_position               = additional_position;
  config.last_symbol                       = last_symbol;
  config.lte_crs_match_around              = lte_crs_match_around;
  config.ue_capable_additional_dmrs_dl_alt = ue_capable_additional_dmrs_dl_alt;

  // Get mask.
  dmrs_symbol_mask mask = pdsch_dmrs_symbol_mask_mapping_type_A_single_get(config);

  // Calculate expected mask.
  dmrs_typeA_position l0 = typeA_pos;
  unsigned            l1 =
      (config.lte_crs_match_around && config.ue_capable_additional_dmrs_dl_alt &&
       config.additional_position == srsran::dmrs_additional_positions::pos1 && l0 == srsran::dmrs_typeA_position::pos3)
                     ? 12
                     : 11;

  // Get expected symbol position list from table.
  ASSERT_TRUE(pdsch_dmrs_symbol_mask_mapping_type_A_single_table.count({last_symbol.to_uint(), additional_position}));
  static_vector<unsigned, 4> symbol_pos_list =
      pdsch_dmrs_symbol_mask_mapping_type_A_single_table.at({last_symbol.to_uint(), additional_position});

  // Convert list to mask.
  dmrs_symbol_mask expected(14);
  for (unsigned symbol_index : symbol_pos_list) {
    if (symbol_index == L0) {
      symbol_index = to_symbol_index(l0);
    }
    if (symbol_index == L1) {
      symbol_index = l1;
    }
    expected.set(symbol_index);
  }

  // Assert each mask with the expected.
  ASSERT_EQ(mask, expected);
}

INSTANTIATE_TEST_SUITE_P(PdschDmrsSymbolMaskTypeASingle,
                         PdschDmrsSymbolMaskFixture,
                         ::testing::Combine(::testing::Values(dmrs_typeA_position::pos2, dmrs_typeA_position::pos3),
                                            ::testing::Range(3U, 15U),
                                            ::testing::Values(dmrs_additional_positions::pos0,
                                                              dmrs_additional_positions::pos1,
                                                              dmrs_additional_positions::pos2,
                                                              dmrs_additional_positions::pos3),
                                            ::testing::Values(true, false),
                                            ::testing::Values(true, false)));

} // namespace
