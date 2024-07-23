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

#include "iq_compression_none_avx2.h"
#include "avx2_helpers.h"
#include "compressed_prb_packer.h"
#include "packing_utils_avx2.h"
#include "quantizer.h"

using namespace srsran;
using namespace ofh;

void iq_compression_none_avx2::compress(span<compressed_prb>         output,
                                        span<const cbf16_t>          input,
                                        const ru_compression_params& params)
{
  // Number of quantized samples per resource block.
  static constexpr size_t NOF_SAMPLES_PER_PRB = 2 * NOF_SUBCARRIERS_PER_RB;
  // AVX2 register size in a number of 16-bit words.
  static constexpr size_t AVX2_REG_SIZE = 16;

  // Quantizer object.
  quantizer q(params.data_width);

  // Use generic implementation if AVX512 utils don't support requested bit width.
  if (!mm256::iq_width_packing_supported(params.data_width)) {
    iq_compression_none_impl::compress(output, input, params);
    return;
  }

  // Auxiliary arrays used for float to fixed point conversion of the input data.
  std::array<int16_t, NOF_SAMPLES_PER_PRB * MAX_NOF_PRBS> input_quantized;

  span<const bf16_t> float_samples_span(reinterpret_cast<const bf16_t*>(input.data()), input.size() * 2U);
  span<int16_t>      input_quantized_span(input_quantized.data(), float_samples_span.size());
  q.to_fixed_point(input_quantized_span, float_samples_span, iq_scaling);

  log_post_quantization_rms(input_quantized_span);

  unsigned sample_idx = 0;
  unsigned rb         = 0;
  // One AVX2 register stores 8 16-bit IQ pairs. We can process 2 PRBs at a time by using 3 AVX2 registers.
  for (size_t rb_index_end = (output.size() / 2) * 2; rb != rb_index_end; rb += 2) {
    span<compressed_prb> c_prbs = output.subspan(rb, 2);

    // Load symbols.
    const auto* start_it   = input_quantized.begin() + sample_idx;
    __m256i     rb0_epi16  = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(start_it + 0));
    __m256i     rb01_epi16 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(start_it + AVX2_REG_SIZE));
    __m256i     rb1_epi16  = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(start_it + AVX2_REG_SIZE * 2));

    // Pack 2 PRBs using utility function and save compression parameters.
    mm256::pack_prbs_big_endian(c_prbs, rb0_epi16, rb01_epi16, rb1_epi16, params.data_width);

    sample_idx += (NOF_SAMPLES_PER_PRB * 2);
  }

  // Use generic implementation for the remaining resource blocks.
  for (; rb != output.size(); ++rb) {
    compressed_prb_packer packer(output[rb]);

    span<const int16_t> input_quantized_last_rb(&input_quantized[sample_idx], NOF_SAMPLES_PER_PRB);
    packer.pack(input_quantized_last_rb, params.data_width);

    sample_idx += NOF_SAMPLES_PER_PRB;
  }
}

void iq_compression_none_avx2::decompress(span<cbf16_t>                output,
                                          span<const compressed_prb>   input,
                                          const ru_compression_params& params)
{
  // Use generic implementation if AVX2 utils don't support requested bit width.
  if (!mm256::iq_width_packing_supported(params.data_width)) {
    iq_compression_none_impl::decompress(output, input, params);
    return;
  }

  // Quantizer object.
  quantizer q(params.data_width);

  unsigned out_idx = 0;
  for (const auto& c_prb : input) {
    constexpr size_t avx2_size_iqs = 16;
    constexpr size_t arr_size      = divide_ceil(NOF_SUBCARRIERS_PER_RB * 2, avx2_size_iqs) * avx2_size_iqs;
    alignas(64) std::array<int16_t, arr_size> unpacked_iq_data;

    // Unpack resource block.
    mm256::unpack_prb_big_endian(unpacked_iq_data, c_prb.get_packed_data(), params.data_width);

    span<cbf16_t>       output_span = output.subspan(out_idx, NOF_SUBCARRIERS_PER_RB);
    span<const int16_t> unpacked_span(unpacked_iq_data.data(), NOF_SUBCARRIERS_PER_RB * 2);

    // Convert to complex samples.
    q.to_brain_float(output_span, unpacked_span, 1);
    out_idx += NOF_SUBCARRIERS_PER_RB;
  }
}
