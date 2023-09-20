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
#include "srsran/ran/uci/uci_part2_size_calculator.h"
#include <gtest/gtest.h>

using namespace srsran;

// Fixed size.
TEST(uci_part2_size_calculator, fix_size)
{
  static constexpr unsigned csi_part1_size          = 4;
  static constexpr unsigned expected_csi_part2_size = 1;

  // Create description.
  uci_part2_size_description description(expected_csi_part2_size);

  // Generate random payload with invalid bits.
  std::array<uint8_t, csi_part1_size> csi_part1;
  std::fill(csi_part1.begin(), csi_part1.end(), 0xff);

  unsigned csi_part2_size = uci_part2_get_size(csi_part1, description);

  ASSERT_EQ(csi_part2_size, expected_csi_part2_size);
}

// Test two ports typical CSI report following TS38.212 Tables 6.3.2.1.2-3 and 6.3.2.1.2-4.
//
// CSI Part 1 consists of:
// - CRI: not present
// - RI: 1 bit
// - Wideband CQI for first TB: 4 bit
// - Subband differential CQI for first TB: not present
//
// CSI Part 2 consists of:
// - Wideband CQI for the second TB: not present
// - Layer Indicator: not present
// - PMI wideband information fields X1: not present
// - PMI wideband information fields X2:
//     - 1 layer: 2 bits
//     - 2 layer: 1 bits
TEST(uci_part2_size_calculator, basic_two_ports)
{
  static constexpr unsigned csi_part1_size = 5;

  // Create description.
  uci_part2_size_description         description = {};
  uci_part2_size_description::entry& entry       = description.entries.emplace_back();

  // Setup RI parameter.
  uci_part2_size_description::parameter& parameter = entry.parameters.emplace_back();
  parameter.width                                  = 1;
  parameter.offset                                 = 0;

  // Push map values of CSI Part 2 sizes in function of the RI.
  entry.map.push_back(2);
  entry.map.push_back(1);

  // Make sure the description is consistent with the CSI Part 1 size.
  ASSERT_TRUE(description.is_valid(csi_part1_size));

  // Generate random payload with invalid bits.
  std::array<uint8_t, csi_part1_size> csi_part1;
  std::fill(csi_part1.begin(), csi_part1.end(), 0xff);

  // Test for RI=1 layer.
  {
    // Force RI to 1 layer.
    csi_part1[0] = 0;

    unsigned csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, 2);
  }

  // Test for RI=2 layer.
  {
    // Force RI to 2 layer.
    csi_part1[0] = 1;

    unsigned csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, 1);
  }
}

// Test four ports typical CSI report following TS38.212 Tables 6.3.2.1.2-3 and 6.3.2.1.2-4.
//
// CSI Part 1 consists of:
// - CRI: not present
// - RI: 2 bit
// - Wideband CQI for first TB: 4 bit
// - Subband differential CQI for first TB: not present
//
// CSI Part 2 consists of:
// - Wideband CQI for the second TB: not present
// - Layer Indicator: not present
// - PMI wideband information fields X1:
//     - 1 layer: 2 bits
//     - 2 layer: 3 bits
//     - 3 and 4 layer: 2 bits
// - PMI wideband information fields X2:
//     - 1 layer: 2 bits
//     - 2 layer: 1 bits
//     - 3 and 4 layer: 1 bit
TEST(uci_part2_size_calculator, basic_four_ports)
{
  static constexpr unsigned csi_part1_size = 5;

  // Create description.
  uci_part2_size_description         description = {};
  uci_part2_size_description::entry& entry       = description.entries.emplace_back();

  // Setup RI parameter.
  uci_part2_size_description::parameter& parameter = entry.parameters.emplace_back();
  parameter.width                                  = 2;
  parameter.offset                                 = 0;

  // Push map values of CSI Part 2 sizes in function of the RI.
  entry.map.push_back(4);
  entry.map.push_back(4);
  entry.map.push_back(3);
  entry.map.push_back(3);

  // Make sure the description is consistent with the CSI Part 1 size.
  ASSERT_TRUE(description.is_valid(csi_part1_size));

  // Generate random payload with invalid bits.
  std::array<uint8_t, csi_part1_size> csi_part1;
  std::fill(csi_part1.begin(), csi_part1.end(), 0xff);

  // Test for RI=1 layer.
  {
    csi_part1[0] = 0;
    csi_part1[1] = 0;

    unsigned csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, 4);
  }

  // Test for RI=2 layer.
  {
    csi_part1[0] = 0;
    csi_part1[1] = 1;

    unsigned csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, 4);
  }

  // Test for RI=3 layer.
  {
    csi_part1[0] = 1;
    csi_part1[1] = 0;

    unsigned csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, 3);
  }

  // Test for RI=4 layer.
  {
    csi_part1[0] = 1;
    csi_part1[1] = 1;

    unsigned csi_part2_size = uci_part2_get_size(csi_part1, description);

    ASSERT_EQ(csi_part2_size, 3);
  }
}