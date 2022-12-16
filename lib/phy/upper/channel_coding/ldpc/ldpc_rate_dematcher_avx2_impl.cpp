/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ldpc_rate_dematcher_avx2_impl.h"
#include <immintrin.h>

using namespace srsgnb;

void ldpc_rate_dematcher_avx2_impl::combine_softbits(span<srsgnb::log_likelihood_ratio>       out,
                                                     span<const srsgnb::log_likelihood_ratio> in0,
                                                     span<const srsgnb::log_likelihood_ratio> in1) const
{
  unsigned index = 0;

  __m256i LLR_MAX_epi8 = _mm256_set1_epi8(static_cast<int8_t>(LLR_MAX));
  __m256i LLR_MIN_epi8 = _mm256_set1_epi8(static_cast<int8_t>(LLR_MIN));

  for (unsigned index_end = (out.size() / 32) * 32; index != index_end; index += 32) {
    __m256i in0_avx = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&in0[index]));
    __m256i in1_avx = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&in1[index]));

    __m256i result = _mm256_adds_epi8(in0_avx, in1_avx);

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