/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "polar_allocator_impl.h"
#include "srsgnb/srsvec/zero.h"

using namespace srsgnb;

void polar_allocator_impl::allocate(bit_buffer& message, bit_buffer& input_encoder, const polar_code& code)
{
  uint16_t             N      = code.get_N();
  uint16_t             K      = code.get_K();
  uint16_t             nPC    = code.get_nPC();
  span<const uint16_t> K_set  = code.get_K_set();
  span<const uint16_t> PC_set = code.get_PC_set();

  assert(input_encoder.size() == N);
  srsvec::zero(input_encoder);

  uint16_t i_o = 0;
  if (nPC == 0) {
    for (uint16_t i = 0; i < K; i++) {
      i_o                = K_set[i];
      input_encoder[i_o] = message[i];
    }
  } else {
    uint16_t y0   = 0;
    uint16_t y1   = 0;
    uint16_t y2   = 0;
    uint16_t y3   = 0;
    uint16_t y4   = 0;
    uint16_t iKPC = 0;
    uint16_t iPC  = 0;
    uint16_t iK   = 0;
    for (i_o = 0; i_o < N; i_o++) {
      // circ. shift register
      uint16_t tmpy0 = y0;
      y0             = y1;
      y1             = y2;
      y2             = y3;
      y3             = y4;
      y4             = tmpy0;
      if (i_o == K_set[iKPC]) { // information bit
        iKPC = iKPC + 1;
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

std::unique_ptr<polar_allocator> srsgnb::create_polar_allocator()
{
  return std::make_unique<polar_allocator_impl>();
}
