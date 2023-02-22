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

#include "ldpc_rate_dematcher_neon_impl.h"
#include <arm_neon.h>

using namespace srsran;

void ldpc_rate_dematcher_neon_impl::combine_softbits(span<log_likelihood_ratio>       out,
                                                     span<const log_likelihood_ratio> in0,
                                                     span<const log_likelihood_ratio> in1) const
{
  srsran_assert(out.size() == in0.size(), "All sizes must be equal.");
  srsran_assert(out.size() == in1.size(), "All sizes must be equal.");

  unsigned index               = 0;
  unsigned out_size            = out.size();
  bool     process_16_val_tail = out_size & 0x10;
  bool     process_8_val_tail  = out_size & 0x8;

  const int8x16_t LLR_MAX_s8 = vdupq_n_s8(static_cast<int8_t>(LLR_MAX));
  const int8x16_t LLR_MIN_s8 = vdupq_n_s8(static_cast<int8_t>(LLR_MIN));

  // Process batches of 32 values.
  for (unsigned index_end = (out_size / 32) * 32; index != index_end; index += 32) {
    int8x16x2_t in0_s8 = vld1q_s8_x2(reinterpret_cast<const int8_t*>(&in0[index]));
    int8x16x2_t in1_s8 = vld1q_s8_x2(reinterpret_cast<const int8_t*>(&in1[index]));

    int8x16x2_t result;
    // Process first 16 LLRs.
    result.val[0]      = vqaddq_s8(in0_s8.val[0], in1_s8.val[0]);
    uint8x16_t mask_s8 = vcgtq_s8(result.val[0], LLR_MAX_s8);
    result.val[0]      = vbslq_s8(mask_s8, LLR_MAX_s8, result.val[0]);
    mask_s8            = vcgtq_s8(LLR_MIN_s8, result.val[0]);
    result.val[0]      = vbslq_s8(mask_s8, LLR_MIN_s8, result.val[0]);

    // Process next 16 LLRs.
    result.val[1] = vqaddq_s8(in0_s8.val[1], in1_s8.val[1]);
    mask_s8       = vcgtq_s8(result.val[1], LLR_MAX_s8);
    result.val[1] = vbslq_s8(mask_s8, LLR_MAX_s8, result.val[1]);
    mask_s8       = vcgtq_s8(LLR_MIN_s8, result.val[1]);
    result.val[1] = vbslq_s8(mask_s8, LLR_MIN_s8, result.val[1]);

    vst1q_s8_x2(reinterpret_cast<int8_t*>(&out[index]), result);
  }
  // Process batch of 16 values.
  if (process_16_val_tail) {
    int8x16_t in0_s8 = vld1q_s8(reinterpret_cast<const int8_t*>(&in0[index]));
    int8x16_t in1_s8 = vld1q_s8(reinterpret_cast<const int8_t*>(&in1[index]));
    int8x16_t result = vqaddq_s8(in0_s8, in1_s8);

    uint8x16_t mask_s8 = vcgtq_s8(result, LLR_MAX_s8);
    result             = vbslq_s8(mask_s8, LLR_MAX_s8, result);
    mask_s8            = vcgtq_s8(LLR_MIN_s8, result);
    result             = vbslq_s8(mask_s8, LLR_MIN_s8, result);

    vst1q_s8(reinterpret_cast<int8_t*>(&out[index]), result);
    index += 16;
  }
  // Process batch of remaining 8 values.
  if (process_8_val_tail) {
    int8x8_t in0_s8 = vld1_s8(reinterpret_cast<const int8_t*>(&in0[index]));
    int8x8_t in1_s8 = vld1_s8(reinterpret_cast<const int8_t*>(&in1[index]));
    int8x8_t result = vqadd_s8(in0_s8, in1_s8);

    uint8x8_t mask_s8 = vcgt_s8(result, vget_low_s8(LLR_MAX_s8));
    result            = vbsl_s8(mask_s8, vget_low_s8(LLR_MAX_s8), result);
    mask_s8           = vcgt_s8(vget_low_s8(LLR_MIN_s8), result);
    result            = vbsl_s8(mask_s8, vget_low_s8(LLR_MIN_s8), result);

    vst1_s8(reinterpret_cast<int8_t*>(&out[index]), result);
    index += 8;
  }

  // Process tail LLRs not multiple of 8.
  for (; index != out_size; ++index) {
    out[index] = in0[index] + in1[index];
  }
}
