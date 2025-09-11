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

#include "iq_compression_bfp_avx512.h"
#include "avx512_helpers.h"
#include "packing_utils_avx512.h"
#include "quantizer.h"
#include "srsran/ofh/compression/compression_properties.h"
#include "srsran/srsvec/prod.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/units.h"

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
/// \param[in] uncompr_samples Pointer to an array of uncompressed 16-bit samples.
/// \param[in] exponent        Exponent used in BFP compression.
/// \param[in] data_width      Bit width of resulting compressed samples.
static void
compress_prb_avx512(span<uint8_t> comp_prb_buffer, const int16_t* uncomp_samples, uint8_t exponent, unsigned data_width)
{
  const __mmask32 load_mask = 0x00ffffff;

  // Load from memory.
  __m512i rb_epi16 = _mm512_maskz_loadu_epi16(load_mask, uncomp_samples);

  // Apply exponent (compress).
  __m512i rb_shifted_epi16 = _mm512_srai_epi16(rb_epi16, exponent);

  // Save exponent.
  std::memcpy(comp_prb_buffer.data(), &exponent, sizeof(uint8_t));

  // Pack compressed samples.
  mm512::pack_prb_big_endian(
      comp_prb_buffer.last(comp_prb_buffer.size() - sizeof(exponent)), rb_shifted_epi16, data_width);
}

void iq_compression_bfp_avx512::compress(span<uint8_t>                buffer,
                                         span<const cbf16_t>          iq_data,
                                         const ru_compression_params& params)
{
  // Use generic implementation if AVX512 utils don't support requested bit width.
  if (!mm512::iq_width_packing_supported(params.data_width)) {
    iq_compression_bfp_impl::compress(buffer, iq_data, params);
    return;
  }

  // AVX512 register size in a number of 16-bit words.
  static constexpr size_t AVX512_REG_SIZE = 32;

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

  // With 3 AVX512 registers we can process 4 PRBs at a time (48 16-bit IQ pairs).
  for (size_t rb_index_end = (nof_prbs / 4) * 4; rb != rb_index_end; rb += 4) {
    // Load input.
    __m512i r0_epi16 = loadu_epi16_avx512(&input_quantized[sample_idx]);
    __m512i r1_epi16 = loadu_epi16_avx512(&input_quantized[sample_idx + AVX512_REG_SIZE]);
    __m512i r2_epi16 = loadu_epi16_avx512(&input_quantized[sample_idx + 2 * AVX512_REG_SIZE]);

    // Determine exponents for each of the four PRBs.
    __m512i exp_epu32 = mm512::determine_bfp_exponent(r0_epi16, r1_epi16, r2_epi16, params.data_width);

    // Exponents are stored in the first bytes of each 128bit lane of the result.
    const auto* exp_byte_ptr = reinterpret_cast<const uint8_t*>(&exp_epu32);

    // Compress the first PRB.
    // Save the exponent.
    span<uint8_t> output_span(&buffer[rb * prb_size], prb_size);
    std::memcpy(output_span.data(), &exp_byte_ptr[0], sizeof(uint8_t));
    // Apply exponent (compress).
    __m512i rb0_shifted_epi16 = _mm512_srai_epi16(r0_epi16, exp_byte_ptr[0]);
    // Pack compressed samples.
    mm512::pack_prb_big_endian(
        output_span.last(output_span.size() - sizeof(uint8_t)), rb0_shifted_epi16, params.data_width);

    // Compress second PRB.
    output_span = span<uint8_t>(&buffer[(rb + 1) * prb_size], prb_size);
    compress_prb_avx512(
        output_span, &input_quantized[sample_idx + NOF_SAMPLES_PER_PRB], exp_byte_ptr[16], params.data_width);

    // Compress third PRB.
    output_span = span<uint8_t>(&buffer[(rb + 2) * prb_size], prb_size);
    compress_prb_avx512(
        output_span, &input_quantized[sample_idx + 2 * NOF_SAMPLES_PER_PRB], exp_byte_ptr[32], params.data_width);

    // Compress fourth PRB.
    output_span = span<uint8_t>(&buffer[(rb + 3) * prb_size], prb_size);
    compress_prb_avx512(
        output_span, &input_quantized[sample_idx + 3 * NOF_SAMPLES_PER_PRB], exp_byte_ptr[48], params.data_width);

    sample_idx += 4 * NOF_SAMPLES_PER_PRB;
  }

  // Process the remaining PRBs (one PRB at a time),
  for (; rb != nof_prbs; ++rb) {
    const __m512i   AVX512_ZERO = _mm512_set1_epi16(0);
    const __mmask32 load_mask   = 0x00ffffff;
    __m512i         rb_epi16    = _mm512_maskz_loadu_epi16(load_mask, &input_quantized[sample_idx]);

    // Determine BFP exponent and extract it from the first byte of the first 128bit lane.
    __m512i     exp_epu32    = mm512::determine_bfp_exponent(rb_epi16, AVX512_ZERO, AVX512_ZERO, params.data_width);
    const auto* exp_byte_ptr = reinterpret_cast<const uint8_t*>(&exp_epu32);

    span<uint8_t> output_span(&buffer[rb * prb_size], prb_size);

    // Save exponent.
    std::memcpy(output_span.data(), &exp_byte_ptr[0], sizeof(uint8_t));

    // Shift and pack a PRB using utility function.
    __m512i rb_shifted_epi16 = _mm512_srai_epi16(rb_epi16, exp_byte_ptr[0]);
    mm512::pack_prb_big_endian(
        output_span.last(output_span.size() - sizeof(uint8_t)), rb_shifted_epi16, params.data_width);

    sample_idx += NOF_SAMPLES_PER_PRB;
  }
}

void iq_compression_bfp_avx512::decompress(span<cbf16_t>                iq_data,
                                           span<const uint8_t>          compressed_data,
                                           const ru_compression_params& params)
{
  // Use generic implementation if AVX512 utils don't support requested bit width.
  if (!mm512::iq_width_packing_supported(params.data_width)) {
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

  const float fixp_gain = (1 << (Q_BIT_WIDTH - 1)) - 1.0f;

  // Determine array size so that AVX512 store operation doesn't write the data out of array bounds.
  constexpr size_t avx512_size_iqs = 32;
  constexpr size_t prb_size        = divide_ceil(NOF_SUBCARRIERS_PER_RB * 2, avx512_size_iqs) * avx512_size_iqs;

  alignas(64) std::array<int16_t, MAX_NOF_PRBS * prb_size>                 unpacked_iq_data;
  alignas(64) std::array<float, MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB * 2> unpacked_iq_scaling;

  unsigned idx = 0;
  for (unsigned c_prb_idx = 0; c_prb_idx != nof_prbs; ++c_prb_idx) {
    // Get view over compressed PRB bytes.
    span<const uint8_t> comp_prb_buffer(&compressed_data[c_prb_idx * comp_prb_size], comp_prb_size);

    // Compute scaling factor, first byte contains the exponent.
    uint8_t exponent = comp_prb_buffer[0];
    float   scaler   = 1 << exponent;

    // Get view over the bytes following the compression parameter.
    comp_prb_buffer = comp_prb_buffer.last(comp_prb_buffer.size() - sizeof(exponent));

    // Unpack resource block.
    span<int16_t> unpacked_prb_span(&unpacked_iq_data[idx], prb_size);
    mm512::unpack_prb_big_endian(unpacked_prb_span, comp_prb_buffer, params.data_width);

    // Save scaling factor.
    std::fill(&unpacked_iq_scaling[idx], &unpacked_iq_scaling[idx] + (NOF_SUBCARRIERS_PER_RB * 2), scaler / fixp_gain);

    idx += (NOF_SUBCARRIERS_PER_RB * 2);
  }
  span<int16_t> unpacked_iq_int16_span(unpacked_iq_data.data(), iq_data.size() * 2);
  span<float>   unpacked_iq_scaling_span(unpacked_iq_scaling.data(), iq_data.size() * 2);

  // Scale unpacked IQ samples using saved exponents and convert to complex samples.
  srsvec::convert(iq_data, unpacked_iq_int16_span, unpacked_iq_scaling_span);
}
