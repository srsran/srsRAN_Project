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

#include "srsran/ran/uci/uci_mapping.h"
#include <array>
#include <cmath>
#include <cstdint>

using namespace srsran;

// Implements Table 9.3-1, TS 38.213.
static const std::array<float, 16> beta_offset_harq_ack_values = {
    // clang-format off
   /* Idx 0 */  1.000,
   /* Idx 1 */  2.000,
   /* Idx 2 */  2.500,
   /* Idx 3 */  3.125,
   /* Idx 4 */  4.000,
   /* Idx 5 */  5.000,
   /* Idx 6 */  6.250,
   /* Idx 7 */  8.000,
   /* Idx 8 */  10.000,
   /* Idx 9 */  12.625,
   /* Idx 10 */ 15.875,
   /* Idx 11 */ 20.000,
   /* Idx 12 */ 31.000,
   /* Idx 13 */ 50.000,
   /* Idx 14 */ 80.000,
   /* Idx 15 */ 126.000
    // clang-format on
};

// Implements Table 9.3-2, TS 38.213.
static const std::array<float, 19> beta_offset_csi_values = {
    // clang-format off
   /* Idx 0 */  1.125,
   /* Idx 1 */  1.250,
   /* Idx 2 */  1.375,
   /* Idx 3 */  1.625,
   /* Idx 4 */  1.750,
   /* Idx 5 */  2.000,
   /* Idx 6 */  2.250,
   /* Idx 7 */  2.500,
   /* Idx 8 */  2.875,
   /* Idx 9 */  3.125,
   /* Idx 10 */ 3.500,
   /* Idx 11 */ 4.000,
   /* Idx 12 */ 5.000,
   /* Idx 13 */ 6.250,
   /* Idx 14 */ 8.000,
   /* Idx 15 */ 10.000,
   /* Idx 16 */ 12.625,
   /* Idx 17 */ 15.875,
   /* Idx 18 */ 20.000,
    // clang-format on
};

float srsran::beta_harq_ack_to_float(unsigned beta_uint_val)
{
  // NOTE: ensure \c beta_uint_val < beta_offset_harq_ack_values.size() before calling this method.
  return beta_offset_harq_ack_values[static_cast<size_t>(beta_uint_val)];
}

float srsran::beta_csi_to_float(unsigned beta_uint_val)
{
  // NOTE: ensure \c beta_uint_val < beta_offset_csi_values.size() before calling this method.
  return beta_offset_csi_values[static_cast<size_t>(beta_uint_val)];
}
