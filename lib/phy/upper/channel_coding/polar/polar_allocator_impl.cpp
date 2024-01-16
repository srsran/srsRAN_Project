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

#include "polar_allocator_impl.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

void polar_allocator_impl::allocate(span<uint8_t> input_encoder, span<const uint8_t> message, const polar_code& code)
{
  unsigned                                N          = code.get_N();
  unsigned                                nPC        = code.get_nPC();
  span<const uint16_t>                    PC_set     = code.get_PC_set();
  const bounded_bitset<polar_code::NMAX>& K_set_mask = code.get_K_set();

  assert(input_encoder.size() == N);
  srsvec::zero(input_encoder);

  if (nPC == 0) {
    K_set_mask.for_each(0, K_set_mask.size(), [&input_encoder, &message, i = 0](size_t i_o) mutable {
      input_encoder[i_o] = message[i++];
    });
  } else {
    unsigned y0  = 0;
    unsigned y1  = 0;
    unsigned y2  = 0;
    unsigned y3  = 0;
    unsigned y4  = 0;
    unsigned iPC = 0;
    unsigned iK  = 0;
    for (unsigned i_o = 0; i_o != N; ++i_o) {
      // Circ. shift register.
      unsigned tmpy0 = y0;
      y0             = y1;
      y1             = y2;
      y2             = y3;
      y3             = y4;
      y4             = tmpy0;
      if (K_set_mask.test(i_o)) { // information bit
        if (i_o == PC_set[iPC]) { // parity bit
          iPC++;
          input_encoder[i_o] = y0;
        } else {
          input_encoder[i_o] = message[iK];
          y0                 = y0 ^ message[iK];
          iK++;
        }
      }
    }
  }
}
