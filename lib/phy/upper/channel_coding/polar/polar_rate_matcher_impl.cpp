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

#include "polar_rate_matcher_impl.h"
#include "srsran/srsvec/copy.h"

using namespace srsran;

/*!
 * generic interleaver
 */
static void interleaver_rm_tx(const uint8_t* input, uint8_t* output, const uint16_t* indices, const uint16_t len)
{
  for (uint32_t j = 0; j < len; j++) {
    output[j] = input[indices[j]];
  }
}

/*!
 * Bit selection for the polar rate-matching block. ye has length N, but there is EMAX memory allocated to it.
 */
static span<uint8_t> bit_selection_rm_tx(uint8_t* y, const uint32_t N, const uint32_t E, const uint32_t K)
{
  uint8_t* e = y;

  if (E >= N) { // repetition
    for (uint32_t k = N; k < E; k++) {
      uint32_t k_N = k % N;
      e[k]         = y[k_N];
    }
  } else {
    if (16 * K <= 7 * E) { // puncturing the first N-E bits
      e = y + (N - E);
    } // else shortening the last N-E bits
  }
  return {e, E};
}

/*!
 * Channel interleaver.
 */
static void ch_interleaver_rm_tx(const uint8_t* e, uint8_t* f, const uint32_t E)
{
  // compute T - Smaller integer such that T(T+1)/2 >= E. Use the fact that 1+2+,..,+T = T(T+1)/2
  uint32_t S = 1;
  uint32_t T = 1;
  while (S < E) {
    T++;
    S = S + T;
  }

  uint32_t i_out = 0;
  for (uint32_t r = 0; r < T; r++) {
    uint32_t i_in = r;
    for (uint32_t c = 0; c < T - r; c++) {
      if (i_in < E) {
        f[i_out] = e[i_in];
        i_out++;
        i_in = i_in + (T - c);
      } else {
        break;
      }
    }
  }
}

void polar_rate_matcher_impl::rate_match(span<uint8_t> output, span<const uint8_t> input, const polar_code& code)
{
  const span<const uint16_t> blk_interleaver = code.get_blk_interleaver();
  unsigned                   N               = code.get_N();
  unsigned                   E               = code.get_E();
  unsigned                   K               = code.get_K();

  uint8_t* y = y_e.data();

  // Run Tx interleaver
  interleaver_rm_tx(input.data(), y, blk_interleaver.data(), N);

  // Select
  span<uint8_t> e = bit_selection_rm_tx(y, N, E, K); // moves the pointer if puncturing e = y + (N-E), otherwise e = y;

  if (code.get_ibil() == polar_code_ibil::not_present) {
    srsvec::copy(output, e);
  } else {
    ch_interleaver_rm_tx(e.data(), output.data(), E);
  }
}
