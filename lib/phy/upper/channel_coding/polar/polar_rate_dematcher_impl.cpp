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

#include "polar_rate_dematcher_impl.h"
#include "srsran/srsvec/copy.h"

using namespace srsran;

/// Generic deinterleaver.
static void interleaver_rm_rx_c(span<log_likelihood_ratio>       output,
                                span<const log_likelihood_ratio> input,
                                span<const uint16_t>             indices)
{
  srsran_assert(input.size() == indices.size(), "Input spans must have the same size.");
  srsran_assert(input.size() == output.size(), "Input and output spans must have the same size.");

  for (uint32_t j = 0, len = output.size(); j != len; ++j) {
    output[indices[j]] = input[j];
  }
}

/// Undoes bit selection for the rate-dematching block.
/// The output has the codeword length N. It inserts 0 to punctured bits (completely unknown bit)
/// and LLR_INFINITY (to indicate very reliable 0 bit). Repeated symbols are added.
static log_likelihood_ratio*
bit_selection_rm_rx_c(log_likelihood_ratio* e, const uint32_t E, const uint32_t N, const uint32_t K)
{
  log_likelihood_ratio* y = e;

  if (E >= N) {
    // Add repetitions.
    for (uint32_t k = N; k != E; ++k) {
      uint32_t k_N = k % N;
      y[k_N]       = log_likelihood_ratio::promotion_sum(y[k_N], e[k]);
    }
  } else {
    if (16 * K <= 7 * E) {
      // Punctured bits are completely unknown, i.e. llr = 0;
      y = e - (N - E);
      for (uint32_t k = 0; k != N - E; ++k) {
        y[k] = 0;
      }

    } else {
      // Shortened bits are known to be 0, set them to LLR_INFINITY.
      for (uint32_t k = E; k != N; ++k) {
        y[k] = LLR_INFINITY;
      }
    }
  }
  return y;
}

/// Channel deinterleaver.
static void ch_interleaver_rm_rx_c(span<log_likelihood_ratio> e, span<const log_likelihood_ratio> f)
{
  srsran_assert(e.size() == f.size(), "Input and output span must have the same size.");
  unsigned E = e.size();
  // Compute T, i.e., th smallest integer such that T(T+1)/2 >= E. Use the fact that 1+2+,..,+T = T(T+1)/2.
  unsigned S = 1;
  unsigned T = 1;
  while (S < E) {
    S += ++T;
  }

  unsigned i_out = 0;
  for (unsigned r = 0; r != T; ++r) {
    unsigned i_in = r;
    for (unsigned c = 0, c_max = T - r; c != c_max; ++c) {
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
    ch_interleaver_rm_rx_c({e, E}, input.first(E));
  }

  log_likelihood_ratio* y = bit_selection_rm_rx_c(e, E, N, K);
  interleaver_rm_rx_c(output, {y, N}, {blk_interleaver, N});
}
