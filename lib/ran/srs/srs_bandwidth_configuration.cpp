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

#include "srsran/ran/srs/srs_bandwidth_configuration.h"
#include "srsran/adt/optional.h"
#include "srsran/adt/span.h"

using namespace srsran;

// TS38.211 Table 6.4.1.4.3-1.
static const std::array<srs_configuration, 64> table0 = {
    {{4, 1},   {8, 1},   {12, 1},  {16, 1},  {16, 1},  {20, 1},  {24, 1},  {24, 1},  {28, 1},  {32, 1},  {36, 1},
     {40, 1},  {48, 1},  {48, 1},  {52, 1},  {56, 1},  {60, 1},  {64, 1},  {72, 1},  {72, 1},  {76, 1},  {80, 1},
     {88, 1},  {96, 1},  {96, 1},  {104, 1}, {112, 1}, {120, 1}, {120, 1}, {120, 1}, {128, 1}, {128, 1}, {128, 1},
     {132, 1}, {136, 1}, {144, 1}, {144, 1}, {144, 1}, {144, 1}, {152, 1}, {160, 1}, {160, 1}, {160, 1}, {168, 1},
     {176, 1}, {184, 1}, {192, 1}, {192, 1}, {192, 1}, {192, 1}, {208, 1}, {216, 1}, {224, 1}, {240, 1}, {240, 1},
     {240, 1}, {240, 1}, {256, 1}, {256, 1}, {256, 1}, {264, 1}, {272, 1}, {272, 1}, {272, 1}}};

// TS38.211 Table 6.4.1.4.3-1.
static const std::array<srs_configuration, 64> table1 = {
    {{4, 1},  {4, 2},   {4, 3},   {4, 4},   {8, 2},   {4, 5},   {4, 6},   {12, 2},  {4, 7},   {16, 2},  {12, 3},
     {20, 2}, {16, 3},  {24, 2},  {4, 13},  {28, 2},  {20, 3},  {32, 2},  {24, 3},  {36, 2},  {4, 19},  {40, 2},
     {44, 2}, {32, 3},  {48, 2},  {52, 2},  {56, 2},  {60, 2},  {40, 3},  {24, 5},  {64, 2},  {64, 2},  {16, 8},
     {44, 3}, {68, 2},  {72, 2},  {48, 3},  {48, 3},  {16, 9},  {76, 2},  {80, 2},  {80, 2},  {32, 5},  {84, 2},
     {88, 2}, {92, 2},  {96, 2},  {96, 2},  {64, 3},  {24, 8},  {104, 2}, {108, 2}, {112, 2}, {120, 2}, {80, 3},
     {48, 5}, {24, 10}, {128, 2}, {128, 2}, {16, 16}, {132, 2}, {136, 2}, {68, 4},  {16, 17}}};

// TS38.211 Table 6.4.1.4.3-1.
static const std::array<srs_configuration, 64> table2 = {
    {{4, 1},  {4, 1},  {4, 1},  {4, 1},  {4, 2},  {4, 1},  {4, 1},  {4, 3},  {4, 1},  {8, 2},  {4, 3},
     {4, 5},  {8, 2},  {12, 2}, {4, 1},  {4, 7},  {4, 5},  {16, 2}, {12, 2}, {12, 3}, {4, 1},  {20, 2},
     {4, 11}, {16, 2}, {24, 2}, {4, 13}, {28, 2}, {20, 3}, {8, 5},  {12, 2}, {32, 2}, {16, 4}, {8, 2},
     {4, 11}, {4, 17}, {36, 2}, {24, 2}, {16, 3}, {8, 2},  {4, 19}, {40, 2}, {20, 4}, {16, 2}, {28, 3},
     {44, 2}, {4, 23}, {48, 2}, {24, 4}, {16, 4}, {8, 3},  {52, 2}, {36, 3}, {56, 2}, {60, 2}, {20, 4},
     {16, 3}, {12, 2}, {64, 2}, {32, 4}, {8, 2},  {44, 3}, {68, 2}, {4, 17}, {8, 2}}};

// TS38.211 Table 6.4.1.4.3-1.
static const std::array<srs_configuration, 64> table3 = {
    {{4, 1},  {4, 1},  {4, 1}, {4, 1}, {4, 1}, {4, 1},  {4, 1}, {4, 1},  {4, 1},  {4, 2},  {4, 1},  {4, 1},  {4, 2},
     {4, 3},  {4, 1},  {4, 1}, {4, 1}, {4, 4}, {4, 3},  {4, 3}, {4, 1},  {4, 5},  {4, 1},  {4, 4},  {4, 6},  {4, 1},
     {4, 7},  {4, 5},  {4, 2}, {4, 3}, {4, 8}, {4, 4},  {4, 2}, {4, 1},  {4, 1},  {4, 9},  {12, 2}, {4, 4},  {4, 2},
     {4, 1},  {4, 10}, {4, 5}, {4, 4}, {4, 7}, {4, 11}, {4, 1}, {4, 12}, {4, 6},  {4, 4},  {4, 2},  {4, 13}, {4, 9},
     {4, 14}, {4, 15}, {4, 5}, {8, 2}, {4, 3}, {4, 16}, {4, 8}, {4, 2},  {4, 11}, {4, 17}, {4, 1},  {4, 2}}};

optional<srs_configuration> srsran::srs_configuration_get(uint8_t c_srs, uint8_t b_srs)
{
  span<const srs_configuration> table;

  switch (b_srs) {
    case 0:
      table = table0;
      break;
    case 1:
      table = table1;
      break;
    case 2:
      table = table2;
      break;
    case 3:
      table = table3;
      break;
  }
  if (c_srs < table.size()) {
    return table[c_srs];
  }

  return nullopt;
}