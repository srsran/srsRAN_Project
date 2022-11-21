/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "polar_deallocator_impl.h"

using namespace srsgnb;

void polar_deallocator_impl::deallocate(span<uint8_t>       message,
                                        span<const uint8_t> output_decoder,
                                        const polar_code&   code)
{
  const bounded_bitset<polar_code::NMAX>& K_set  = code.get_K_set();
  span<const uint16_t>                    PC_set = code.get_PC_set();

  K_set.for_each(0, K_set.size(), [&PC_set, &message, &output_decoder, iPC = 0, iK = 0](uint16_t i_o) mutable {
    if (i_o == PC_set[iPC]) { // skip
      iPC = iPC + 1;
    } else {
      message[iK] = output_decoder[i_o];
      iK          = iK + 1;
    }
  });
}
