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

#include "iq_compression_none_neon.h"
#include "compressed_prb_packer.h"
#include "neon_helpers.h"
#include "packing_utils_neon.h"
#include "quantizer.h"

using namespace srsran;
using namespace ofh;

void iq_compression_none_neon::compress(span<srsran::ofh::compressed_prb>         output,
                                        span<const srsran::cbf16_t>               input,
                                        const srsran::ofh::ru_compression_params& params)
{
  // Number of quantized samples per resource block.
  static constexpr size_t NOF_SAMPLES_PER_PRB = 2 * NOF_SUBCARRIERS_PER_RB;

  // Use generic implementation if NEON utils don't support requested bit width.
  if (!neon::iq_width_packing_supported(params.data_width)) {
    iq_compression_none_impl::compress(output, input, params);
    return;
  }

  // Quantizer object.
  quantizer q(params.data_width);

  // Auxiliary arrays used for float to fixed point conversion of the input data.
  std::array<int16_t, NOF_SAMPLES_PER_PRB * MAX_NOF_PRBS> input_quantized;

  span<const bf16_t> float_samples_span(reinterpret_cast<const bf16_t*>(input.data()), input.size() * 2U);
  span<int16_t>      input_quantized_span(input_quantized.data(), float_samples_span.size());
  q.to_fixed_point(input_quantized_span, float_samples_span, iq_scaling);

  log_post_quantization_rms(input_quantized_span);

  unsigned sample_idx = 0;
  unsigned rb         = 0;
  // One NEON register can store 8 16-bit samples. A PRB comprises 24 16-bit IQ samples, thus we need three NEON
  // registers to process one PRB.
  //
  // The loop below processes four resource blocks at a time.
  for (size_t rb_index_end = (output.size() / 4) * 4; rb != rb_index_end; rb += 4) {
    // Load samples.
    int16x8x3_t vec_s16x3_0 = vld1q_s16_x3(&input_quantized[sample_idx]);
    int16x8x3_t vec_s16x3_1 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB]);
    int16x8x3_t vec_s16x3_2 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB * 2]);
    int16x8x3_t vec_s16x3_3 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB * 3]);

    // Pack samples of the PRB using utility function.
    neon::pack_prb_big_endian(output[rb], vec_s16x3_0, params.data_width);
    neon::pack_prb_big_endian(output[rb + 1], vec_s16x3_1, params.data_width);
    neon::pack_prb_big_endian(output[rb + 2], vec_s16x3_2, params.data_width);
    neon::pack_prb_big_endian(output[rb + 3], vec_s16x3_3, params.data_width);

    sample_idx += (4 * NOF_SAMPLES_PER_PRB);
  }

  // The loop below processes two resource blocks at a time.
  for (size_t rb_index_end = (output.size() / 2) * 2; rb != rb_index_end; rb += 2) {
    // Load samples.
    int16x8x3_t vec_s16x3_0 = vld1q_s16_x3(&input_quantized[sample_idx]);
    int16x8x3_t vec_s16x3_1 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB]);
    // Pack samples of the PRB using utility function.
    neon::pack_prb_big_endian(output[rb], vec_s16x3_0, params.data_width);
    neon::pack_prb_big_endian(output[rb + 1], vec_s16x3_1, params.data_width);
    sample_idx += (2 * NOF_SAMPLES_PER_PRB);
  }

  // Process the last resource block.
  if (rb != output.size()) {
    // Load samples.
    int16x8x3_t vec_s16x3 = vld1q_s16_x3(&input_quantized[sample_idx]);

    // Pack samples of the PRB using utility function.
    neon::pack_prb_big_endian(output[rb], vec_s16x3, params.data_width);

    sample_idx += NOF_SAMPLES_PER_PRB;
    ++rb;
  }

  // Use generic implementation if the requested compression width is not supported by neon utils.
  for (; rb != output.size(); ++rb) {
    compressed_prb_packer packer(output[rb]);
    packer.pack({&input_quantized[sample_idx], NOF_SAMPLES_PER_PRB}, params.data_width);
    sample_idx += NOF_SAMPLES_PER_PRB;
  }
}

void iq_compression_none_neon::decompress(span<cbf16_t>                output,
                                          span<const compressed_prb>   input,
                                          const ru_compression_params& params)
{
  // Use generic implementation if NEON utils don't support requested bit width.
  if (!neon::iq_width_packing_supported(params.data_width)) {
    iq_compression_none_impl::decompress(output, input, params);
    return;
  }

  quantizer q_out(params.data_width);

  unsigned out_idx = 0;
  for (const compressed_prb& c_prb : input) {
    // Unpack resource block.
    std::array<int16_t, NOF_SUBCARRIERS_PER_RB * 2> unpacked_iq_data;
    neon::unpack_prb_big_endian(unpacked_iq_data, c_prb.get_packed_data(), params.data_width);

    span<cbf16_t> output_span = output.subspan(out_idx, NOF_SUBCARRIERS_PER_RB);
    // Convert to complex samples.
    q_out.to_brain_float(output_span, unpacked_iq_data, 1);
    out_idx += NOF_SUBCARRIERS_PER_RB;
  }
}
