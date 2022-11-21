/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "polar_allocator_impl.h"
#include "srsgnb/srsvec/zero.h"

using namespace srsgnb;

void polar_allocator_impl::allocate(span<uint8_t> input_encoder, span<const uint8_t> message, const polar_code& code)
{
  uint16_t                                N          = code.get_N();
  uint16_t                                nPC        = code.get_nPC();
  span<const uint16_t>                    PC_set     = code.get_PC_set();
  const bounded_bitset<polar_code::NMAX>& K_set_mask = code.get_K_set();

  assert(input_encoder.size() == N);
  srsvec::zero(input_encoder);

  if (nPC == 0) {
    K_set_mask.for_each(0, K_set_mask.size(), [&input_encoder, &message, i = 0](size_t i_o) mutable {
      input_encoder[i_o] = message[i++];
    });
  } else {
    uint16_t y0  = 0;
    uint16_t y1  = 0;
    uint16_t y2  = 0;
    uint16_t y3  = 0;
    uint16_t y4  = 0;
    uint16_t iPC = 0;
    uint16_t iK  = 0;
    for (uint16_t i_o = 0; i_o < N; i_o++) {
      // circ. shift register
      uint16_t tmpy0 = y0;
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
