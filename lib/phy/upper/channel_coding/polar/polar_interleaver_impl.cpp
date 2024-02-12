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

#include "polar_interleaver_impl.h"

using namespace srsran;

const std::array<uint8_t, polar_interleaver_impl::K_MAX_IL> polar_interleaver_impl::pattern = {
    0,   2,   4,   7,   9,   14,  19,  20,  24,  25,  26,  28,  31,  34,  42,  45,  49,  50,  51,  53,  54,
    56,  58,  59,  61,  62,  65,  66,  67,  69,  70,  71,  72,  76,  77,  81,  82,  83,  87,  88,  89,  91,
    93,  95,  98,  101, 104, 106, 108, 110, 111, 113, 115, 118, 119, 120, 122, 123, 126, 127, 129, 132, 134,
    138, 139, 140, 1,   3,   5,   8,   10,  15,  21,  27,  29,  32,  35,  43,  46,  52,  55,  57,  60,  63,
    68,  73,  78,  84,  90,  92,  94,  96,  99,  102, 105, 107, 109, 112, 114, 116, 121, 124, 128, 130, 133,
    135, 141, 6,   11,  16,  22,  30,  33,  36,  44,  47,  64,  74,  79,  85,  97,  100, 103, 117, 125, 131,
    136, 142, 12,  17,  23,  37,  48,  75,  80,  86,  137, 143, 13,  18,  38,  144, 39,  145, 40,  146, 41,
    147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163};

void polar_interleaver_impl::interleave(span<uint8_t>               out,
                                        span<const uint8_t>         in,
                                        polar_interleaver_direction direction)
{
  srsran_assert(in.size() == out.size(), "Input and output span sizes must be identical");
  unsigned K = in.size();

  uint32_t k = 0;
  for (uint32_t m = 0; m < K_MAX_IL; m++) {
    if (pattern[m] >= K_MAX_IL - K) {
      uint32_t pi_k = pattern[m] - (K_MAX_IL - K);
      if (direction == polar_interleaver_direction::tx) {
        out[k] = in[pi_k];
      } else {
        out[pi_k] = in[k];
      }
      k++;
    }
  }
}
