/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "iq_compression_bfp_avx2.h"
#include "avx2_helpers.h"
#include "packing_utils_avx2.h"
#include "quantizer.h"
#include "srsran/srsvec/prod.h"

using namespace srsran;
using namespace ofh;

void iq_compression_bfp_avx2::compress(span<compressed_prb>         output,
                                       span<const cbf16_t>          input,
                                       const ru_compression_params& params)
{
  // Use generic implementation if AVX2 utils don't support requested bit width.
  if (!mm256::iq_width_packing_supported(params.data_width)) {
    iq_compression_bfp_impl::compress(output, input, params);
    return;
  }

  // AVX2 register size in a number of 16bit words.
  static constexpr size_t AVX2_REG_SIZE = 16;

  // Auxiliary arrays used for float to fixed point conversion of the input data.
  std::array<int16_t, NOF_SAMPLES_PER_PRB * MAX_NOF_PRBS> input_quantized;

  span<const bf16_t> float_samples_span(reinterpret_cast<const bf16_t*>(input.data()), input.size() * 2U);
  span<int16_t>      input_quantized_span(input_quantized.data(), float_samples_span.size());
  // Performs conversion of input complex float values to signed 16-bit integers.
  quantize_input(input_quantized_span, float_samples_span);

  // Compression algorithm implemented according to O-RAN.WG4.CUS Annex A.1.2.
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

    // Pack 2 PRBs using utility function and save compression parameters.
    mm256::pack_prbs_big_endian(c_prbs, rb0_sr_epi16, rb01_sr_epi16, rb1_sr_epi16, params.data_width);
    c_prbs[0].set_compression_param(exponent_0);
    c_prbs[1].set_compression_param(exponent_1);

    sample_idx += (NOF_SAMPLES_PER_PRB * 2);
  }

  // Use generic implementation for the remaining resource blocks.
  for (; rb != output.size(); ++rb) {
    const auto* start_it = input_quantized.begin() + sample_idx;
    compress_prb_generic(output[rb], {start_it, NOF_SAMPLES_PER_PRB}, params.data_width);
    sample_idx += NOF_SAMPLES_PER_PRB;
  }
}

void iq_compression_bfp_avx2::decompress(span<cbf16_t>                output,
                                         span<const compressed_prb>   input,
                                         const ru_compression_params& params)
{
  // Use generic implementation if AVX2 utils don't support requested bit width.
  if (!mm256::iq_width_packing_supported(params.data_width)) {
    iq_compression_bfp_impl::decompress(output, input, params);
    return;
  }

  quantizer q_out(Q_BIT_WIDTH);

  // Determine array size so that AVX2 store operation doesn't write the data out of array bounds.
  constexpr size_t avx2_size_iqs = 16;
  constexpr size_t prb_size      = divide_ceil(NOF_SUBCARRIERS_PER_RB * 2, avx2_size_iqs) * avx2_size_iqs;

  alignas(64) std::array<int16_t, MAX_NOF_PRBS * prb_size>                   unpacked_iq_data;
  alignas(64) std::array<int16_t, MAX_NOF_PRBS * NOF_SUBCARRIERS_PER_RB * 2> unpacked_iq_scaling;

  unsigned idx = 0;
  for (const auto& c_prb : input) {
    // Compute scaling factor.
    uint8_t exponent = c_prb.get_compression_param();
    int16_t scaler   = 1 << exponent;

    // Unpack resource block.
    span<int16_t> unpacked_prb_span(&unpacked_iq_data[idx], prb_size);
    mm256::unpack_prb_big_endian(unpacked_prb_span, c_prb.get_packed_data(), params.data_width);

    // Save scaling factor.
    std::fill(&unpacked_iq_scaling[idx], &unpacked_iq_scaling[idx + NOF_SUBCARRIERS_PER_RB * 2], scaler);

    idx += (NOF_SUBCARRIERS_PER_RB * 2);
  }

  span<int16_t> unpacked_iq_int16_span(unpacked_iq_data.data(), output.size() * 2);
  span<int16_t> unpacked_iq_scaling_span(unpacked_iq_scaling.data(), output.size() * 2);
  // Scale unpacked IQ samples using saved exponents.
  srsvec::prod(unpacked_iq_int16_span, unpacked_iq_scaling_span, unpacked_iq_int16_span);
  // Convert to complex samples.
  q_out.to_brain_float(output, unpacked_iq_int16_span, 1);
}
