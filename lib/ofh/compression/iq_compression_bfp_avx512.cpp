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

#include "iq_compression_bfp_avx512.h"
#include "avx512_helpers.h"
#include "packing_utils_avx512.h"
#include "quantizer.h"

using namespace srsran;
using namespace ofh;

/// Loads packed 16-bit integers from non-aligned memory.
static inline __m512i loadu_epi16_avx512(const void* mem_address)
{
  const __mmask32 mask       = 0xffffffff;
  const __m512i   zero_epi16 = _mm512_set1_epi64(0);
  return _mm512_mask_loadu_epi16(zero_epi16, mask, mem_address);
}

/// \brief Compresses samples of a single resource block using AVX512 intrinsics.
///
/// \param[out] compressed_prb Compressed PRB (stores compressed packed values).
/// \param[in] uncompr_samples Pointer to an array of uncompressed 16bit samples.
/// \param[in] exponent        Exponent used in BFP compression.
/// \param[in] data_width      Bit width of resulting compressed samples.
static void
compress_prb_avx512(compressed_prb& c_prb, const int16_t* uncompr_samples, uint8_t exponent, unsigned data_width)
{
  const __mmask32 load_mask = 0x00ffffff;

  // Load from memory.
  __m512i rb_epi16 = _mm512_maskz_loadu_epi16(load_mask, uncompr_samples);

  // Apply exponent (compress).
  __m512i rb_shifted_epi16 = _mm512_srai_epi16(rb_epi16, exponent);

  // Pack compressed samples.
  mm512::pack_prb_big_endian(c_prb, rb_shifted_epi16, data_width);
}

void iq_compression_bfp_avx512::compress(span<compressed_prb>         output,
                                         span<const cf_t>             input,
                                         const ru_compression_params& params)
{
  // Use generic implementation if AVX512 utils don't support requested bit width.
  if (!mm512::iq_width_packing_supported(params.data_width)) {
    iq_compression_bfp_impl::compress(output, input, params);
    return;
  }

  // AVX512 register size in a number of 16bit words.
  static constexpr size_t AVX512_REG_SIZE = 32;

  // Auxiliary arrays used for float to fixed point conversion of the input data.
  std::array<int16_t, NOF_SAMPLES_PER_PRB * MAX_NOF_PRBS> input_quantized;

  span<const float> float_samples_span(reinterpret_cast<const float*>(input.data()), input.size() * 2U);
  span<int16_t>     input_quantized_span(input_quantized.data(), input.size() * 2U);
  // Performs conversion of input complex float values to signed 16bit integers.
  quantize_input(input_quantized_span, float_samples_span);

  // Compression algorithm implemented according to Annex A.1.2 in O-RAN.WG4.CUS.
  unsigned sample_idx = 0;
  unsigned rb         = 0;

  // With 3 AVX512 registers we can process 4 PRBs at a time (48 16bit IQ pairs).
  for (size_t rb_index_end = (output.size() / 4) * 4; rb != rb_index_end; rb += 4) {
    // Load input.
    __m512i r0_epi16 = loadu_epi16_avx512(&input_quantized[sample_idx]);
    __m512i r1_epi16 = loadu_epi16_avx512(&input_quantized[sample_idx + AVX512_REG_SIZE]);
    __m512i r2_epi16 = loadu_epi16_avx512(&input_quantized[sample_idx + 2 * AVX512_REG_SIZE]);

    // Determine exponents for each of the four PRBs.
    __m512i exp_epu32 = mm512::determine_bfp_exponent(r0_epi16, r1_epi16, r2_epi16, params.data_width);

    // Exponents are stored in the first bytes of each 128bit lane of the result.
    const auto* exp_byte_ptr = reinterpret_cast<const uint8_t*>(&exp_epu32);
    output[rb].set_compression_param(exp_byte_ptr[0]);
    output[rb + 1].set_compression_param(exp_byte_ptr[16]);
    output[rb + 2].set_compression_param(exp_byte_ptr[32]);
    output[rb + 3].set_compression_param(exp_byte_ptr[48]);

    // Compress and pack the first PRB using utility function.
    __m512i rb0_shifted_epi16 = _mm512_srai_epi16(r0_epi16, exp_byte_ptr[0]);
    mm512::pack_prb_big_endian(output[rb], rb0_shifted_epi16, params.data_width);

    // Compress second PRB.
    compress_prb_avx512(
        output[rb + 1], &input_quantized[sample_idx + NOF_SAMPLES_PER_PRB], exp_byte_ptr[16], params.data_width);
    // Compress third PRB.
    compress_prb_avx512(
        output[rb + 2], &input_quantized[sample_idx + 2 * NOF_SAMPLES_PER_PRB], exp_byte_ptr[32], params.data_width);
    // Compress fourth PRB.
    compress_prb_avx512(
        output[rb + 3], &input_quantized[sample_idx + 3 * NOF_SAMPLES_PER_PRB], exp_byte_ptr[48], params.data_width);

    sample_idx += 4 * NOF_SAMPLES_PER_PRB;
  }

  // Process the remaining PRBs (one PRB at a time),
  for (size_t rb_index_end = output.size(); rb != rb_index_end; ++rb) {
    const __m512i   AVX512_ZERO = _mm512_set1_epi16(0);
    const __mmask32 load_mask   = 0x00ffffff;
    __m512i         rb_epi16    = _mm512_maskz_loadu_epi16(load_mask, &input_quantized[sample_idx]);

    // Determine BFP exponent and extract it from the first byte of the first 128bit lane.
    __m512i     exp_epu32    = mm512::determine_bfp_exponent(rb_epi16, AVX512_ZERO, AVX512_ZERO, params.data_width);
    const auto* exp_byte_ptr = reinterpret_cast<const uint8_t*>(&exp_epu32);
    output[rb].set_compression_param(exp_byte_ptr[0]);

    // Shift and pack the first PRB using utility function.
    __m512i rb_shifted_epi16 = _mm512_srai_epi16(rb_epi16, exp_byte_ptr[0]);
    mm512::pack_prb_big_endian(output[rb], rb_shifted_epi16, params.data_width);

    sample_idx += NOF_SAMPLES_PER_PRB;
  }
}

void iq_compression_bfp_avx512::decompress(span<cf_t>                   output,
                                           span<const compressed_prb>   input,
                                           const ru_compression_params& params)
{
  // Use generic implementation if AVX512 utils don't support requested bit width.
  if (!mm512::iq_width_packing_supported(params.data_width)) {
    iq_compression_bfp_impl::decompress(output, input, params);
    return;
  }

  quantizer q_out(Q_BIT_WIDTH);

  unsigned out_idx = 0;
  for (const auto& c_prb : input) {
    // Compute scaling factor.
    uint8_t exponent = c_prb.get_compression_param();
    int16_t scaler   = 1 << exponent;

    // Determine array size so that AVX512 store operation doesn't write the data out of array bounds.
    constexpr size_t avx512_size_iqs = 32;
    constexpr size_t arr_size        = divide_ceil(NOF_SUBCARRIERS_PER_RB * 2, avx512_size_iqs) * avx512_size_iqs;
    alignas(64) std::array<int16_t, arr_size> unpacked_iq_data;
    // Unpack resource block.
    mm512::unpack_prb_big_endian(unpacked_iq_data, c_prb.get_packed_data(), params.data_width);

    span<cf_t>    output_span = output.subspan(out_idx, NOF_SUBCARRIERS_PER_RB);
    span<int16_t> unpacked_span(unpacked_iq_data.data(), NOF_SUBCARRIERS_PER_RB * 2);

    // Convert to complex samples.
    q_out.to_float(output_span, unpacked_span, scaler);
    out_idx += NOF_SUBCARRIERS_PER_RB;
  }
}
