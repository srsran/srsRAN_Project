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
  uint16_t             K      = code.get_K();
  uint16_t             nPC    = code.get_nPC();
  span<const uint16_t> K_set  = code.get_K_set();
  span<const uint16_t> PC_set = code.get_PC_set();

  uint16_t iPC = 0;
  uint16_t iK  = 0;
  for (uint16_t iKPC = 0; iKPC < (uint16_t)(K + nPC); iKPC++) {
    uint16_t i_o = K_set[iKPC]; // includes parity bits
    if (i_o == PC_set[iPC]) {   // skip
      iPC = iPC + 1;
    } else {
      message[iK] = output_decoder[i_o];
      iK          = iK + 1;
    }
  }
}
