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

#include "ldpc_rate_dematcher_avx512_impl.h"
#include <immintrin.h>

using namespace srsran;

void ldpc_rate_dematcher_avx512_impl::combine_softbits(span<log_likelihood_ratio>       out,
                                                       span<const log_likelihood_ratio> in0,
                                                       span<const log_likelihood_ratio> in1) const
{
  srsran_assert(out.size() == in0.size(), "All sizes must be equal.");
  srsran_assert(out.size() == in1.size(), "All sizes must be equal.");

  unsigned index = 0;

  __m512i LLR_MAX_epi8 = _mm512_set1_epi8(static_cast<int8_t>(LLR_MAX));
  __m512i LLR_MIN_epi8 = _mm512_set1_epi8(static_cast<int8_t>(LLR_MIN));

  for (unsigned index_end = (out.size() / 64) * 64; index != index_end; index += 64) {
    __m512i in0_epi8 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(&in0[index]));
    __m512i in1_epi8 = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(&in1[index]));

    __m512i result = _mm512_adds_epi8(in0_epi8, in1_epi8);

    __mmask64 mask_epi8 = _mm512_cmpgt_epi8_mask(result, LLR_MAX_epi8);
    result              = _mm512_mask_blend_epi8(mask_epi8, result, LLR_MAX_epi8);

    mask_epi8 = _mm512_cmpgt_epi8_mask(LLR_MIN_epi8, result);
    result    = _mm512_mask_blend_epi8(mask_epi8, result, LLR_MIN_epi8);

    _mm512_storeu_si512(reinterpret_cast<__m512i*>(&out[index]), result);
  }

  for (unsigned index_end = out.size(); index != index_end; ++index) {
    out[index] = in0[index] + in1[index];
  }
}