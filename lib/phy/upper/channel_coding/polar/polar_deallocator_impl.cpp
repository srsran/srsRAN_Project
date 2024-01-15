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

#include "polar_deallocator_impl.h"

using namespace srsran;

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
