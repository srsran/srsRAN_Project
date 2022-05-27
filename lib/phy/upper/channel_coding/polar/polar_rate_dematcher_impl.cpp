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

// Generic deinterleaver.
static void interleaver_rm_rx_c(const log_likelihood_ratio* input,
                                log_likelihood_ratio*       output,
                                const uint16_t*             indices,
                                const uint16_t              len)
{
  for (uint32_t j = 0; j != len; ++j) {
    output[indices[j]] = input[j];
  }
}

// Undoes bit selection for the rate-dematching block.
// The output has the codeword length N. It inserts 0 to punctured bits (completely unknown bit)
// and LLR_INFINITY (to indicate very reliable 0 bit). Repeated symbols are added.

static log_likelihood_ratio*
bit_selection_rm_rx_c(log_likelihood_ratio* e, const uint32_t E, const uint32_t N, const uint32_t K)
{
  log_likelihood_ratio* y   = nullptr;
  uint32_t              k_N = 0;

  y = e;
  if (E >= N) { // add repetitions
    y = e;
    for (uint32_t k = N; k != E; ++k) {
      k_N    = k % N;
      y[k_N] = log_likelihood_ratio::promotion_sum(y[k_N], e[k]);
    }
  } else {
    if (16 * K <= 7 * E) { // puncturing bits are completely unknown, i.e. llr = 0;
      y = e - (N - E);
      for (uint32_t k = 0; k != N - E; ++k) {
        y[k] = 0;
      }

    } else { // shortening, bits are know to be 0. i.e., very high llrs
      for (uint32_t k = E; k != N; ++k) {
        y[k] = LLR_INFINITY; /* max value */
      }
    }
  }
  return y;
}

// Channel deinterleaver.
static void ch_interleaver_rm_rx_c(const log_likelihood_ratio* f, log_likelihood_ratio* e, const uint32_t E)
{
  // compute T - Smaller integer such that T(T+1)/2 >= E. Use the fact that 1+2+,..,+T = T(T+1)/2
  uint32_t S = 1;
  uint32_t T = 1;
  while (S < E) {
    ++T;
    S = S + T;
  }

  uint32_t i_out = 0;
  uint32_t i_in  = 0;
  for (uint32_t r = 0; r != T; ++r) {
    i_in = r;
    for (uint32_t c = 0, c_max = T - r; c != c_max; ++c) {
      if (i_in < E) {
        e[i_in] = f[i_out];
        ++i_out;
        i_in += (T - c);
      } else {
        break;
      }
    }
  }
}

void polar_rate_dematcher_impl::rate_dematch(span<log_likelihood_ratio>       output,
                                             span<const log_likelihood_ratio> input,
                                             const polar_code&                code)
{
  unsigned N = code.get_N();
  unsigned E = code.get_E();
  unsigned K = code.get_K();

  const uint16_t* blk_interleaver = code.get_blk_interleaver().data();

  if (code.get_ibil() == polar_code_ibil::not_present) {
    srsvec::copy(span<log_likelihood_ratio>(e, input.size()), input);
  } else {
    ch_interleaver_rm_rx_c(input.data(), e, E);
  }

  log_likelihood_ratio* y = bit_selection_rm_rx_c(e, E, N, K);
  interleaver_rm_rx_c(y, output.data(), blk_interleaver, N);
}

std::unique_ptr<polar_rate_dematcher> srsgnb::create_polar_rate_dematcher()
{
  return std::make_unique<polar_rate_dematcher_impl>();
}
