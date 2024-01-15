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

#include "srsran/ran/prach/prach_cyclic_shifts.h"
#include "srsran/adt/span.h"
#include <array>

using namespace srsran;

uint16_t srsran::prach_cyclic_shifts_get(prach_subcarrier_spacing prach_scs,
                                         restricted_set_config    restricted_set,
                                         unsigned                 zero_correlation_zone)
{
  // TS38.211 Table 6.3.3.1-5 First column.
  static const std::array<uint16_t, 16> NOF_CYCLIC_SHIFTS_1_25_UNRESTRICTED = {
      0, 13, 15, 18, 22, 26, 32, 38, 46, 59, 76, 93, 119, 167, 279, 419};
  // TS38.211 Table 6.3.3.1-5 Second column.
  static const std::array<uint16_t, 16> NOF_CYCLIC_SHIFTS_1_25_TYPE_A = {
      15, 18, 22, 26, 32, 38, 46, 55, 68, 82, 100, 128, 158, 202, 237, PRACH_CYCLIC_SHIFTS_RESERVED};
  // TS38.211 Table 6.3.3.1-5 Third column.
  static const std::array<uint16_t, 16> NOF_CYCLIC_SHIFTS_1_25_TYPE_B = {15,
                                                                         18,
                                                                         22,
                                                                         26,
                                                                         32,
                                                                         38,
                                                                         46,
                                                                         55,
                                                                         68,
                                                                         82,
                                                                         100,
                                                                         118,
                                                                         137,
                                                                         PRACH_CYCLIC_SHIFTS_RESERVED,
                                                                         PRACH_CYCLIC_SHIFTS_RESERVED,
                                                                         PRACH_CYCLIC_SHIFTS_RESERVED};

  // TS38.211 Table 6.3.3.1-6 First column.
  static const std::array<uint16_t, 16> NOF_CYCLIC_SHIFTS_5_UNRESTRICTED = {
      0, 13, 26, 33, 38, 41, 49, 55, 64, 76, 93, 119, 139, 209, 279, 419};
  // TS38.211 Table 6.3.3.1-6 Second column.
  static const std::array<uint16_t, 16> NOF_CYCLIC_SHIFTS_5_TYPE_A = {
      36, 57, 72, 81, 89, 94, 103, 112, 121, 132, 137, 152, 173, 195, 216, 237};
  // TS38.211 Table 6.3.3.1-6 Third column.
  static const std::array<uint16_t, 16> NOF_CYCLIC_SHIFTS_5_TYPE_B = {36,
                                                                      57,
                                                                      60,
                                                                      63,
                                                                      65,
                                                                      68,
                                                                      71,
                                                                      77,
                                                                      81,
                                                                      85,
                                                                      97,
                                                                      109,
                                                                      122,
                                                                      137,
                                                                      PRACH_CYCLIC_SHIFTS_RESERVED,
                                                                      PRACH_CYCLIC_SHIFTS_RESERVED};

  // TS38.211 Table 6.3.3.1-7 First column.
  static const std::array<uint16_t, 16> NOF_CYCLIC_SHIFTS_OTHER_UNRESTRICTED = {
      0, 2, 4, 6, 8, 10, 12, 13, 15, 17, 19, 23, 27, 34, 46, 69};

  span<const uint16_t> table = {};

  if (prach_scs == prach_subcarrier_spacing::kHz1_25) {
    switch (restricted_set) {
      case restricted_set_config::UNRESTRICTED:
        table = NOF_CYCLIC_SHIFTS_1_25_UNRESTRICTED;
        break;
      case restricted_set_config::TYPE_A:
        table = NOF_CYCLIC_SHIFTS_1_25_TYPE_A;
        break;
      case restricted_set_config::TYPE_B:
        table = NOF_CYCLIC_SHIFTS_1_25_TYPE_B;
        break;
    }
  } else if (prach_scs == prach_subcarrier_spacing::kHz5) {
    switch (restricted_set) {
      case restricted_set_config::UNRESTRICTED:
        table = NOF_CYCLIC_SHIFTS_5_UNRESTRICTED;
        break;
      case restricted_set_config::TYPE_A:
        table = NOF_CYCLIC_SHIFTS_5_TYPE_A;
        break;
      case restricted_set_config::TYPE_B:
        table = NOF_CYCLIC_SHIFTS_5_TYPE_B;
        break;
    }
  } else if (restricted_set == restricted_set_config::UNRESTRICTED) {
    table = NOF_CYCLIC_SHIFTS_OTHER_UNRESTRICTED;
  }

  if (zero_correlation_zone >= table.size()) {
    return PRACH_CYCLIC_SHIFTS_RESERVED;
  }

  return table[zero_correlation_zone];
}