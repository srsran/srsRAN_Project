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

#include "iq_compression_bfp_avx2.h"
#include "avx2_helpers.h"
#include "packing_utils_avx2.h"
#include "quantizer.h"
#include "srsran/ofh/compression/compression_properties.h"
#include "srsran/srsvec/prod.h"
#include "srsran/support/math/math_utils.h"

using namespace srsran;
using namespace ofh;

void iq_compression_bfp_avx2::compress(span<uint8_t>                buffer,
                                       span<const cbf16_t>          iq_data,
                                       const ru_compression_params& params)
{
  // Use generic implementation if AVX2 utils don't support requested bit width.
  if (!mm256::iq_width_packing_supported(params.data_width)) {
    iq_compression_bfp_impl::compress(buffer, iq_data, params);
    return;
  }

  // AVX2 register size in a number of 16bit words.
  static constexpr size_t AVX2_REG_SIZE = 16;

  // Number of input PRBs.
  unsigned nof_prbs = (iq_data.size() / NOF_SUBCARRIERS_PER_RB);

  // Size in bytes of one compressed PRB using the given compression parameters.
  unsigned prb_size = get_compressed_prb_size(params).value();

  srsran_assert(buffer.size() >= prb_size * nof_prbs, "Output buffer doesn't have enough space to decompress PRBs");

  // Auxiliary arrays used for float to fixed point conversion of the input data.
  std::array<int16_t, NOF_SAMPLES_PER_PRB * MAX_NOF_PRBS> input_quantized;

  span<const bf16_t> float_samples_span(reinterpret_cast<const bf16_t*>(iq_data.data()), iq_data.size() * 2U);
  span<int16_t>      input_quantized_span(input_quantized.data(), float_samples_span.size());
  // Performs conversion of input complex float values to signed 16-bit integers.
  quantize_input(input_quantized_span, float_samples_span);

  // Compression algorithm implemented according to O-RAN.WG4.CUS Annex A.1.2.
  unsigned sample_idx = 0;
  unsigned rb         = 0;

  // One AVX2 register stores 8 16-bit IQ pairs. We can process 2 PRBs at a time by using 3 AVX2 registers.
  for (size_t rb_index_end = (nof_prbs / 2) * 2; rb != rb_index_end; rb += 2) {
    // Get view over bytes corresponding to two PRBs processed in this iteration.
    span<uint8_t> comp_prb0_buffer(&buffer[rb * prb_size], prb_size);
    span<uint8_t> comp_prb1_buffer(&buffer[(rb + 1) * prb_size], prb_size);

    // Load symbols.
    const auto* start_it   = input_quantized.begin() + sample_idx;
    __m256i     rb0_epi16  = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(start_it + 0));
    __m256i     rb01_epi16 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(start_it + AVX2_REG_SIZE));
    __m256i     rb1_epi16  = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(start_it + AVX2_REG_SIZE * 2));

    std::array<unsigned, 2> max_abs;
    mm256::calculate_max_abs(max_abs, rb0_epi16, rb01_epi16, rb1_epi16);
    // Find exponents for each of the 2 PRBs.
    // There is no vector lzcnt instruction in AVX2, so we use scalars here.
    uint8_t exponent_0 = determine_exponent(max_abs[0], params.data_width);
    uint8_t exponent_1 = determine_exponent(max_abs[1], params.data_width);

    // Shift original IQ samples right.
    __m256i rb0_sr_epi16    = _mm256_srai_epi16(rb0_epi16, exponent_0);
    __m256i rb1_sr_epi16    = _mm256_srai_epi16(rb1_epi16, exponent_1);
    __m256i rb01_sr_0_epi16 = _mm256_srai_epi16(rb01_epi16, exponent_0);
    __m256i rb01_sr_1_epi16 = _mm256_srai_epi16(rb01_epi16, exponent_1);
    __m256i rb01_sr_epi16   = _mm256_permute2f128_si256(rb01_sr_0_epi16, rb01_sr_1_epi16, 0x30);

    // Write compression parameters to the output buffer.
    std::memcpy(comp_prb0_buffer.data(), &exponent_0, sizeof(uint8_t));
    std::memcpy(comp_prb1_buffer.data(), &exponent_1, sizeof(uint8_t));

    comp_prb0_buffer = comp_prb0_buffer.last(comp_prb0_buffer.size() - sizeof(uint8_t));
    comp_prb1_buffer = comp_prb1_buffer.last(comp_prb1_buffer.size() - sizeof(uint8_t));

    // Pack 2 PRBs using utility function and save compression parameters.
    mm256::pack_prbs_big_endian(
        comp_prb0_buffer, comp_prb1_buffer, rb0_sr_epi16, rb01_sr_epi16, rb1_sr_epi16, params.data_width);

    sample_idx += (NOF_SAMPLES_PER_PRB * 2);
  }

  // Use generic implementation for the remaining resource blocks.
  for (; rb != nof_prbs; ++rb) {
    // Get view over buffer bytes corresponding to one PRB.
    span<uint8_t> comp_prb_buffer(&buffer[rb * prb_size], prb_size);

    const auto* start_it = input_quantized.begin() + sample_idx;
    compress_prb_generic(comp_prb_buffer, {start_it, NOF_SAMPLES_PER_PRB}, params.data_width);
    sample_idx += NOF_SAMPLES_PER_PRB;
  }
}

void iq_compression_bfp_avx2::decompress(span<cbf16_t>                iq_data,
                                         span<const uint8_t>          compressed_data,
                                         const ru_compression_params& params)
{
  // Use generic implementation if AVX2 utils don't support requested bit width.
  if (!mm256::iq_width_packing_supported(params.data_width)) {
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

  // Determine array size so that AVX2 store operation doesn't write the data out of array bounds.
  constexpr size_t avx2_size_iqs = 16;
  constexpr size_t prb_size      = divide_ceil(NOF_SUBCARRIERS_PER_RB * 2, avx2_size_iqs) * avx2_size_iqs;

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
    mm256::unpack_prb_big_endian(unpacked_prb_span, comp_prb_buffer, params.data_width);

    // Save scaling factor.
    std::fill(&unpacked_iq_scaling[idx], &unpacked_iq_scaling[idx] + (NOF_SUBCARRIERS_PER_RB * 2), scaler / fixp_gain);

    idx += (NOF_SUBCARRIERS_PER_RB * 2);
  }
  span<int16_t> unpacked_iq_int16_span(unpacked_iq_data.data(), iq_data.size() * 2);
  span<float>   unpacked_iq_scaling_span(unpacked_iq_scaling.data(), iq_data.size() * 2);

  // Scale unpacked IQ samples using saved exponents and convert to complex samples.
  srsvec::convert(iq_data, unpacked_iq_int16_span, unpacked_iq_scaling_span);
}
