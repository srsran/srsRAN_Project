/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ldpc_rate_dematcher_avx512_impl.h"
#include <immintrin.h>

using namespace srsgnb;

void ldpc_rate_dematcher_avx512_impl::combine_softbits(span<log_likelihood_ratio>       out,
                                                       span<const log_likelihood_ratio> in0,
                                                       span<const log_likelihood_ratio> in1) const
{
  srsgnb_assert(out.size() == in0.size(), "All sizes must be equal.");
  srsgnb_assert(out.size() == in1.size(), "All sizes must be equal.");

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