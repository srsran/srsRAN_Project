/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/ofh/compression/compression_properties.h"
#include "srsran/support/math/math_utils.h"

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

void iq_compression_bfp_neon::compress(span<uint8_t>                buffer,
                                       span<const cbf16_t>          iq_data,
                                       const ru_compression_params& params)
{
  // Use generic implementation if NEON utils don't support requested bit width.
  if (!neon::iq_width_packing_supported(params.data_width)) {
    iq_compression_bfp_impl::compress(buffer, iq_data, params);
    return;
  }

  // Number of input PRBs.
  unsigned nof_prbs = (iq_data.size() / NOF_SUBCARRIERS_PER_RB);

  // Size in bytes of one compressed PRB using the given compression parameters.
  unsigned prb_size = get_compressed_prb_size(params).value();

  srsran_assert(buffer.size() >= prb_size * nof_prbs, "Output buffer doesn't have enough space to decompress PRBs");

  // Auxiliary arrays used for float to fixed point conversion of the input data.
  std::array<int16_t, NOF_SAMPLES_PER_PRB * MAX_NOF_PRBS> input_quantized;

  span<const bf16_t> float_samples_span(reinterpret_cast<const bf16_t*>(iq_data.data()), iq_data.size() * 2U);
  span<int16_t>      input_quantized_span(input_quantized.data(), float_samples_span.size());

  // Performs conversion of input brain float values to signed 16-bit integers.
  quantize_input(input_quantized_span, float_samples_span);

  // Compression algorithm implemented according to Annex A.1.2 in O-RAN.WG4.CUS.
  unsigned sample_idx = 0;
  unsigned rb         = 0;

  // One NEON register can store 8 16-bit samples. A PRB is comprised of 24 16-bit IQ samples, thus we need three NEON
  // registers to process one PRB.
  //
  // The loop below processes four resource blocks at a time.
  for (size_t rb_index_end = (nof_prbs / 4) * 4; rb != rb_index_end; rb += 4) {
    // Load samples.
    int16x8x3_t vec_s16x3_0 = vld1q_s16_x3(&input_quantized[sample_idx]);
    int16x8x3_t vec_s16x3_1 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB]);
    int16x8x3_t vec_s16x3_2 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB * 2]);
    int16x8x3_t vec_s16x3_3 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB * 3]);

    uint8_t exponent_0 = 0;
    uint8_t exponent_1 = 0;
    uint8_t exponent_2 = 0;
    uint8_t exponent_3 = 0;
    // Determine exponents.
    if (SRSRAN_LIKELY(params.data_width != MAX_IQ_WIDTH)) {
      exponent_0 = neon::determine_bfp_exponent(vec_s16x3_0, params.data_width);
      exponent_1 = neon::determine_bfp_exponent(vec_s16x3_1, params.data_width);
      exponent_2 = neon::determine_bfp_exponent(vec_s16x3_2, params.data_width);
      exponent_3 = neon::determine_bfp_exponent(vec_s16x3_3, params.data_width);
    }

    // Shift original IQ samples right.
    int16x8x3_t shifted_data_0, shifted_data_1, shifted_data_2, shifted_data_3;
    shift_right_x3vector_s16(vec_s16x3_0, shifted_data_0, exponent_0);
    shift_right_x3vector_s16(vec_s16x3_1, shifted_data_1, exponent_1);
    shift_right_x3vector_s16(vec_s16x3_2, shifted_data_2, exponent_2);
    shift_right_x3vector_s16(vec_s16x3_3, shifted_data_3, exponent_3);

    // Pack compressed samples of the PRBs using utility function and save the exponent.
    span<uint8_t> output_span(&buffer[rb * prb_size], prb_size);
    std::memcpy(output_span.data(), &exponent_0, sizeof(uint8_t));
    neon::pack_prb_big_endian(
        output_span.last(output_span.size() - sizeof(uint8_t)), shifted_data_0, params.data_width);

    output_span = span<uint8_t>(&buffer[(rb + 1) * prb_size], prb_size);
    std::memcpy(output_span.data(), &exponent_1, sizeof(uint8_t));
    neon::pack_prb_big_endian(
        output_span.last(output_span.size() - sizeof(uint8_t)), shifted_data_1, params.data_width);

    output_span = span<uint8_t>(&buffer[(rb + 2) * prb_size], prb_size);
    std::memcpy(output_span.data(), &exponent_2, sizeof(uint8_t));
    neon::pack_prb_big_endian(
        output_span.last(output_span.size() - sizeof(uint8_t)), shifted_data_2, params.data_width);

    output_span = span<uint8_t>(&buffer[(rb + 3) * prb_size], prb_size);
    std::memcpy(output_span.data(), &exponent_3, sizeof(uint8_t));
    neon::pack_prb_big_endian(
        output_span.last(output_span.size() - sizeof(uint8_t)), shifted_data_3, params.data_width);

    sample_idx += (4 * NOF_SAMPLES_PER_PRB);
  }

  // The loop below processes two resource blocks at a time.
  for (size_t rb_index_end = (nof_prbs / 2) * 2; rb != rb_index_end; rb += 2) {
    // Load samples.
    int16x8x3_t vec_s16x3_0 = vld1q_s16_x3(&input_quantized[sample_idx]);
    int16x8x3_t vec_s16x3_1 = vld1q_s16_x3(&input_quantized[sample_idx + NOF_SAMPLES_PER_PRB]);

    // Determine exponents.
    uint8_t exponent_0 = 0;
    uint8_t exponent_1 = 0;
    if (SRSRAN_LIKELY(params.data_width != MAX_IQ_WIDTH)) {
      exponent_0 = neon::determine_bfp_exponent(vec_s16x3_0, params.data_width);
      exponent_1 = neon::determine_bfp_exponent(vec_s16x3_1, params.data_width);
    }

    // Shift original IQ samples right.
    int16x8x3_t shifted_data_0, shifted_data_1;
    shift_right_x3vector_s16(vec_s16x3_0, shifted_data_0, exponent_0);
    shift_right_x3vector_s16(vec_s16x3_1, shifted_data_1, exponent_1);

    // Save exponent of the first compressed PRB and pack its compressed IQ samples using utility function.
    span<uint8_t> output_span(&buffer[rb * prb_size], prb_size);
    std::memcpy(output_span.data(), &exponent_0, sizeof(uint8_t));
    neon::pack_prb_big_endian(
        output_span.last(output_span.size() - sizeof(uint8_t)), shifted_data_0, params.data_width);

    // Save exponent of the first compressed PRB and pack its compressed IQ samples using utility function.
    output_span = span<uint8_t>(&buffer[(rb + 1) * prb_size], prb_size);
    std::memcpy(output_span.data(), &exponent_1, sizeof(uint8_t));
    neon::pack_prb_big_endian(
        output_span.last(output_span.size() - sizeof(uint8_t)), shifted_data_1, params.data_width);

    sample_idx += (2 * NOF_SAMPLES_PER_PRB);
  }

  // Process the last resource block.
  if (rb != nof_prbs) {
    // Load samples.
    int16x8x3_t vec_s16x3 = vld1q_s16_x3(&input_quantized[sample_idx]);

    // Determine exponent.
    uint8_t exponent = 0;
    if (SRSRAN_LIKELY(params.data_width != MAX_IQ_WIDTH)) {
      exponent = neon::determine_bfp_exponent(vec_s16x3, params.data_width);
    }

    // Shift original IQ samples right.
    int16x8x3_t shifted_data;
    shift_right_x3vector_s16(vec_s16x3, shifted_data, exponent);

    // Save exponent of the compressed PRB and pack its compressed IQ samples using utility function.
    span<uint8_t> output_span(&buffer[rb * prb_size], prb_size);
    std::memcpy(output_span.data(), &exponent, sizeof(uint8_t));
    neon::pack_prb_big_endian(output_span.last(output_span.size() - sizeof(uint8_t)), shifted_data, params.data_width);

    sample_idx += NOF_SAMPLES_PER_PRB;
    ++rb;
  }
}

void iq_compression_bfp_neon::decompress(span<cbf16_t>                iq_data,
                                         span<const uint8_t>          compressed_data,
                                         const ru_compression_params& params)
{
  // Use generic implementation if NEON utils don't support requested bit width.
  if (!neon::iq_width_packing_supported(params.data_width)) {
    iq_compression_bfp_impl::decompress(iq_data, compressed_data, params);
    return;
  }

  // Number of output PRBs.
  unsigned nof_prbs = iq_data.size() / NOF_SUBCARRIERS_PER_RB;

  // Size in bytes of one compressed PRB using the given compression parameters.
  unsigned comp_prb_size = get_compressed_prb_size(params).value();

  srsran_assert(compressed_data.size() >= nof_prbs * comp_prb_size,
                "Input does not contain enough bytes to decompress {} PRBs",
                nof_prbs);

  quantizer q_out(Q_BIT_WIDTH);

  unsigned out_idx = 0;
  for (unsigned c_prb_idx = 0; c_prb_idx != nof_prbs; ++c_prb_idx) {
    // Get view over compressed PRB bytes.
    span<const uint8_t> comp_prb_buffer(&compressed_data[c_prb_idx * comp_prb_size], comp_prb_size);

    // Compute scaling factor, first byte contains the exponent.
    uint8_t exponent = comp_prb_buffer[0];
    float   scaler   = 1 << exponent;

    // Get view over the bytes following the compression parameter.
    comp_prb_buffer = comp_prb_buffer.last(comp_prb_buffer.size() - sizeof(exponent));

    // Determine array size so that NEON store operation doesn't write the data out of array bounds.
    constexpr size_t neon_size_iqs = 8;
    constexpr size_t arr_size      = divide_ceil(NOF_SAMPLES_PER_PRB, neon_size_iqs) * neon_size_iqs;
    alignas(64) std::array<int16_t, arr_size> unpacked_iq_data;

    // Unpack resource block.
    neon::unpack_prb_big_endian(unpacked_iq_data, comp_prb_buffer, params.data_width);

    span<cbf16_t>       output_span = iq_data.subspan(out_idx, NOF_SUBCARRIERS_PER_RB);
    span<const int16_t> unpacked_span(unpacked_iq_data.data(), NOF_SUBCARRIERS_PER_RB * 2);

    // Convert to complex samples.
    q_out.to_brain_float(output_span, unpacked_span, scaler);
    out_idx += NOF_SUBCARRIERS_PER_RB;
  }
}
