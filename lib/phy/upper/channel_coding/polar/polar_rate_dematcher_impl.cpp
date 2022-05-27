/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "polar_rate_dematcher_impl.h"
#include "srsgnb/srsvec/copy.h"

using namespace srsgnb;

// Generic deinterleaver (int8_t).
static void interleaver_rm_rx_c(const int8_t* input, int8_t* output, const uint16_t* indices, const uint16_t len)
{
  for (uint32_t j = 0; j < len; j++) {
    output[indices[j]] = input[j];
  }
}

// Undoes bit selection for the rate-dematching block (int8_t).
// The output has the codeword length N. It inserts 0 to punctured bits (completely unknown bit)
// and 127 (to indicate very reliable 0 bit). Repeated symbols are added.

static int8_t* bit_selection_rm_rx_c(int8_t* e, const uint32_t E, const uint32_t N, const uint32_t K)
{
  int8_t*  y   = nullptr;
  uint32_t k_N = 0;
  long     tmp = 0;

  y = e;
  if (E >= N) { // add repetitions
    y = e;
    for (uint32_t k = N; k < E; k++) {
      k_N = k % N;
      tmp = (long)y[k_N] + e[k];
      // control saturation
      if (tmp > 127) {
        tmp = 127;
      }
      if (tmp < -127) {
        tmp = -127;
      }
      y[k_N] = (int8_t)tmp;
    }
  } else {
    if (16 * K <= 7 * E) { // puncturing bits are completely unknown, i.e. llr = 0;
      y = e - (N - E);
      for (uint32_t k = 0; k < N - E; k++) {
        y[k] = 0;
      }

    } else { // shortening, bits are know to be 0. i.e., very high llrs
      for (uint32_t k = E; k < N; k++) {
        y[k] = 127; /* max value */
      }
    }
  }
  return y;
}

// Channel deinterleaver (int8_t).
static void ch_interleaver_rm_rx_c(const int8_t* f, int8_t* e, const uint32_t E)
{
  // compute T - Smaller integer such that T(T+1)/2 >= E. Use the fact that 1+2+,..,+T = T(T+1)/2
  uint32_t S = 1;
  uint32_t T = 1;
  while (S < E) {
    T++;
    S = S + T;
  }

  uint32_t i_out = 0;
  uint32_t i_in  = 0;
  for (uint32_t r = 0; r < T; r++) {
    i_in = r;
    for (uint32_t c = 0; c < T - r; c++) {
      if (i_in < E) {
        e[i_in] = f[i_out];
        i_out++;
        i_in = i_in + (T - c);
      } else {
        break;
      }
    }
  }
}

void polar_rate_dematcher_impl::rate_dematch(span<int8_t> output, span<const int8_t> input, const polar_code& code)
{
  unsigned N = code.get_N();
  unsigned E = code.get_E();
  unsigned K = code.get_K();

  const uint16_t* blk_interleaver = code.get_blk_interleaver().data();

  if (code.get_ibil() == polar_code_ibil::not_present) {
    srsvec::copy(span<int8_t>(e, input.size()), input);
  } else {
    ch_interleaver_rm_rx_c(input.data(), e, E);
  }

  int8_t* y = bit_selection_rm_rx_c(e, E, N, K);
  interleaver_rm_rx_c(y, output.data(), blk_interleaver, N);
}

std::unique_ptr<polar_rate_dematcher> srsgnb::create_polar_rate_dematcher()
{
  return std::make_unique<polar_rate_dematcher_impl>();
}
