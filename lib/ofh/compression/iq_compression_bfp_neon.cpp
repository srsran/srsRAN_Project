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

#include "iq_compression_bfp_neon.h"
#include "neon_helpers.h"
#include "packing_utils_neon.h"
#include "quantizer.h"

using namespace srsran;
using namespace ofh;

#define shift_right_x3vector(src, dst, exponent)                                                                       \
  (dst).val[0] = vshrq_n_s16((src).val[0], (exponent));                                                                \
  (dst).val[1] = vshrq_n_s16((src).val[1], (exponent));                                                                \
  (dst).val[2] = vshrq_n_s16((src).val[2], (exponent));

/// Workaround for \c vshrq_n_s16 NEON intrinsic which only accepts an immediate value known at compile time. Note that
/// we are checking values up to 8 because minimum output bit width used in BFP compression is 8 bits.
static inline void shift_right_x3vector_s16(int16x8x3_t src, int16x8x3_t& dst, uint8_t exponent)
{
  switch (exponent) {
    case 1:
      shift_right_x3vector(src, dst, 1);
      break;
    case 2:
      shift_right_x3vector(src, dst, 2);
      break;
    case 3:
      shift_right_x3vector(src, dst, 3);
      break;
    case 4:
      shift_right_x3vector(src, dst, 4);
      break;
    case 5:
      shift_right_x3vector(src, dst, 5);
      break;
    case 6:
      shift_right_x3vector(src, dst, 6);
      break;
    case 7:
      shift_right_x3vector(src, dst, 7);
      break;
    case 8:
      shift_right_x3vector(src, dst, 8);
      break;
    default:
      // Copy source to destination when exponent is 0 or incorrect value is passed.
      dst.val[0] = src.val[0];
      dst.val[1] = src.val[1];
      dst.val[2] = src.val[2];
      break;
  }
}

void iq_compression_bfp_neon::compress(span<srsran::ofh::compressed_prb>         output,
                                       span<const srsran::cf_t>                  input,
                                       const srsran::ofh::ru_compression_params& params)
{
  // Use generic implementation if NEON utils don't support requested bit width.
  if (!neon::iq_width_packing_supported(params.data_width)) {
    iq_compression_bfp_impl::compress(output, input, params);
    return;
  }

  // Auxiliary arrays used for float to fixed point conversion of the input data.
  std::array<int16_t, NOF_SAMPLES_PER_PRB * MAX_NOF_PRBS> input_quantized;

  span<const float> float_samples_span(reinterpret_cast<const float*>(input.data()), input.size() * 2U);
  span<int16_t>     input_quantized_span(input_quantized.data(), input.size() * 2U);

  // Performs conversion of input complex float values to signed 16bit integers.
  quantize_input(input_quantized_span, float_samples_span);

  // Compression algorithm implemented according to Annex A.1.2 in O-RAN.WG4.CUS.
  unsigned sample_idx = 0;
  unsigned rb         = 0;

  // One NEON register can store 8 16bit samples. A PRB is comprised of 24 16bit IQ samples, thus we need three NEON
  // registers to process one PRB.
  //
  // The loop below processes four resource blocks at a time.
  for (size_t rb_index_end = (output.size() / 4) * 4; rb != rb_index_end; rb += 4) {
    // Load samples.
    int16x8x3_t vec_s16x3_0 = vld1q_s16_x3(&input_quantized[sample_idx]);
    int16x8x3_t vec_s16x3_1 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB]);
    int16x8x3_t vec_s16x3_2 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB * 2]);
    int16x8x3_t vec_s16x3_3 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB * 3]);

    // Determine exponents.
    const uint8_t exponent_0 = neon::determine_bfp_exponent(vec_s16x3_0, params.data_width);
    const uint8_t exponent_1 = neon::determine_bfp_exponent(vec_s16x3_1, params.data_width);
    const uint8_t exponent_2 = neon::determine_bfp_exponent(vec_s16x3_2, params.data_width);
    const uint8_t exponent_3 = neon::determine_bfp_exponent(vec_s16x3_3, params.data_width);

    // Shift original IQ samples right.
    int16x8x3_t shifted_data_0, shifted_data_1, shifted_data_2, shifted_data_3;
    shift_right_x3vector_s16(vec_s16x3_0, shifted_data_0, exponent_0);
    shift_right_x3vector_s16(vec_s16x3_1, shifted_data_1, exponent_1);
    shift_right_x3vector_s16(vec_s16x3_2, shifted_data_2, exponent_2);
    shift_right_x3vector_s16(vec_s16x3_3, shifted_data_3, exponent_3);

    // Pack compressed samples of the PRB using utility function and save the exponent.
    neon::pack_prb_big_endian(output[rb], shifted_data_0, params.data_width);
    output[rb].set_compression_param(exponent_0);
    neon::pack_prb_big_endian(output[rb + 1], shifted_data_1, params.data_width);
    output[rb + 1].set_compression_param(exponent_1);
    neon::pack_prb_big_endian(output[rb + 2], shifted_data_2, params.data_width);
    output[rb + 2].set_compression_param(exponent_2);
    neon::pack_prb_big_endian(output[rb + 3], shifted_data_3, params.data_width);
    output[rb + 3].set_compression_param(exponent_3);

    sample_idx += (4 * NOF_SAMPLES_PER_PRB);
  }

  // The loop below processes two resource blocks at a time.
  for (size_t rb_index_end = (output.size() / 2) * 2; rb != rb_index_end; rb += 2) {
    // Load samples.
    int16x8x3_t vec_s16x3_0 = vld1q_s16_x3(&input_quantized[sample_idx]);
    int16x8x3_t vec_s16x3_1 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB]);

    // Determine exponents.
    const uint8_t exponent_0 = neon::determine_bfp_exponent(vec_s16x3_0, params.data_width);
    const uint8_t exponent_1 = neon::determine_bfp_exponent(vec_s16x3_1, params.data_width);

    // Shift original IQ samples right.
    int16x8x3_t shifted_data_0, shifted_data_1;
    shift_right_x3vector_s16(vec_s16x3_0, shifted_data_0, exponent_0);
    shift_right_x3vector_s16(vec_s16x3_1, shifted_data_1, exponent_1);

    // Pack compressed samples of the PRB using utility function.
    neon::pack_prb_big_endian(output[rb], shifted_data_0, params.data_width);
    // Save exponent.
    output[rb].set_compression_param(exponent_0);

    neon::pack_prb_big_endian(output[rb + 1], shifted_data_1, params.data_width);
    output[rb + 1].set_compression_param(exponent_1);

    sample_idx += (2 * NOF_SAMPLES_PER_PRB);
  }

  // Process the last resource block.
  if (rb != output.size()) {
    // Load samples.
    int16x8x3_t vec_s16x3 = vld1q_s16_x3(&input_quantized[sample_idx]);

    // Determine exponent.
    const uint8_t exponent = neon::determine_bfp_exponent(vec_s16x3, params.data_width);

    // Shift original IQ samples right.
    int16x8x3_t shifted_data;
    shift_right_x3vector_s16(vec_s16x3, shifted_data, exponent);

    // Pack compressed samples of the PRB using utility function.
    neon::pack_prb_big_endian(output[rb], shifted_data, params.data_width);
    // Save exponent.
    output[rb].set_compression_param(exponent);

    sample_idx += NOF_SAMPLES_PER_PRB;
    ++rb;
  }

  // Use generic implementation if the requested compression width is not supported by neon utils.
  for (; rb != output.size(); ++rb) {
    const auto* start_it = input_quantized.begin() + sample_idx;
    compress_prb_generic(output[rb], {start_it, NOF_SAMPLES_PER_PRB}, params.data_width);
    sample_idx += NOF_SAMPLES_PER_PRB;
  }
}

void iq_compression_bfp_neon::decompress(span<srsran::cf_t>                        output,
                                         span<const srsran::ofh::compressed_prb>   input,
                                         const srsran::ofh::ru_compression_params& params)
{
  // Use generic implementation if NEON utils don't support requested bit width.
  if (!neon::iq_width_packing_supported(params.data_width)) {
    iq_compression_bfp_impl::decompress(output, input, params);
    return;
  }

  quantizer q_out(Q_BIT_WIDTH);

  unsigned out_idx = 0;
  for (const compressed_prb& c_prb : input) {
    // Compute scaling factor.
    uint8_t exponent = c_prb.get_compression_param();
    int16_t scaler   = 1 << exponent;

    // Determine array size so that NEON store operation doesn't write the data out of array bounds.
    constexpr size_t neon_size_iqs = 8;
    constexpr size_t arr_size      = divide_ceil(NOF_SAMPLES_PER_PRB, neon_size_iqs) * neon_size_iqs;
    alignas(64) std::array<int16_t, arr_size> unpacked_iq_data;
    // Unpack resource block.
    neon::unpack_prb_big_endian(unpacked_iq_data, c_prb.get_packed_data(), params.data_width);

    span<cf_t>          output_span = output.subspan(out_idx, NOF_SUBCARRIERS_PER_RB);
    span<const int16_t> unpacked_span(unpacked_iq_data.data(), NOF_SUBCARRIERS_PER_RB * 2);

    // Convert to complex samples.
    q_out.to_float(output_span, unpacked_span, scaler);
    out_idx += NOF_SUBCARRIERS_PER_RB;
  }
}
