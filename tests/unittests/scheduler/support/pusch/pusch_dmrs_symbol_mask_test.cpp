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

#include "lib/scheduler/support/pusch/pusch_dmrs_symbol_mask.h"
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

using PuschDmrsSymbolMaskParams = std::tuple<dmrs_typeA_position, unsigned, dmrs_additional_positions>;

class PuschDmrsSymbolMaskFixture : public ::testing::TestWithParam<PuschDmrsSymbolMaskParams>
{};

const std::map<std::tuple<unsigned, dmrs_additional_positions>, static_vector<unsigned, 4>>
    pusch_dmrs_symbol_mask_typeA_single_table = {
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
        {{13, dmrs_additional_positions::pos0}, {L0}},        {{13, dmrs_additional_positions::pos1}, {L0, 11}},
        {{13, dmrs_additional_positions::pos2}, {L0, 7, 11}}, {{13, dmrs_additional_positions::pos3}, {L0, 5, 8, 11}},
        {{14, dmrs_additional_positions::pos0}, {L0}},        {{14, dmrs_additional_positions::pos1}, {L0, 11}},
        {{14, dmrs_additional_positions::pos2}, {L0, 7, 11}}, {{14, dmrs_additional_positions::pos3}, {L0, 5, 8, 11}}};

TEST_P(PuschDmrsSymbolMaskFixture, ANormal)
{
  // Extract parameters.
  dmrs_typeA_position             typeA_pos           = std::get<0>(GetParam());
  bounded_integer<uint8_t, 1, 14> last_symbol         = std::get<1>(GetParam());
  dmrs_additional_positions       additional_position = std::get<2>(GetParam());

  // Prepare configuration.
  pusch_dmrs_symbol_mask_mapping_type_A_single_configuration config;
  config.typeA_pos           = typeA_pos;
  config.last_symbol         = last_symbol.to_uint();
  config.additional_position = additional_position;

  // Get mask.
  dmrs_symbol_mask mask = pusch_dmrs_symbol_mask_mapping_type_A_single_get(config);

  // Get expected symbol position list from table.
  ASSERT_TRUE(pusch_dmrs_symbol_mask_typeA_single_table.count({last_symbol.to_uint(), additional_position}));
  static_vector<unsigned, 4> symbol_pos_list =
      pusch_dmrs_symbol_mask_typeA_single_table.at({last_symbol.to_uint(), additional_position});

  // Convert list to mask.
  dmrs_symbol_mask expected(14);
  for (unsigned symbol_index : symbol_pos_list) {
    if (symbol_index == L0) {
      symbol_index = to_symbol_index(typeA_pos);
    }
    expected.set(symbol_index);
  }

  // Assert each mask with the expected.
  ASSERT_EQ(mask, expected);
}

INSTANTIATE_TEST_SUITE_P(PuschDmrsSymbolMaskTypeASingle,
                         PuschDmrsSymbolMaskFixture,
                         ::testing::Combine(::testing::Values(dmrs_typeA_position::pos2, dmrs_typeA_position::pos3),
                                            ::testing::Range(5U, 15U),
                                            ::testing::Values(dmrs_additional_positions::pos0,
                                                              dmrs_additional_positions::pos1,
                                                              dmrs_additional_positions::pos2,
                                                              dmrs_additional_positions::pos3)));

} // namespace
