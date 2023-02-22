/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ldpc_rate_dematcher_avx2_impl.h"
#include <immintrin.h>

using namespace srsran;

void ldpc_rate_dematcher_avx2_impl::combine_softbits(span<srsran::log_likelihood_ratio>       out,
                                                     span<const srsran::log_likelihood_ratio> in0,
                                                     span<const srsran::log_likelihood_ratio> in1) const
{
  srsran_assert(out.size() == in0.size(), "All sizes must be equal.");
  srsran_assert(out.size() == in1.size(), "All sizes must be equal.");

  unsigned index = 0;

  __m256i LLR_MAX_epi8 = _mm256_set1_epi8(static_cast<int8_t>(LLR_MAX));
  __m256i LLR_MIN_epi8 = _mm256_set1_epi8(static_cast<int8_t>(LLR_MIN));

  for (unsigned index_end = (out.size() / 32) * 32; index != index_end; index += 32) {
    __m256i in0_epi8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&in0[index]));
    __m256i in1_epi8 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&in1[index]));

    __m256i result = _mm256_adds_epi8(in0_epi8, in1_epi8);

    __m256i mask_epi8 = _mm256_cmpgt_epi8(result, LLR_MAX_epi8);
    result            = _mm256_blendv_epi8(result, LLR_MAX_epi8, mask_epi8);

    mask_epi8 = _mm256_cmpgt_epi8(LLR_MIN_epi8, result);
    result    = _mm256_blendv_epi8(result, LLR_MIN_epi8, mask_epi8);

    _mm256_storeu_si256(reinterpret_cast<__m256i*>(&out[index]), result);
  }

  for (unsigned index_end = out.size(); index != index_end; ++index) {
    out[index] = in0[index] + in1[index];
  }
}