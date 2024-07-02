/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "iq_compression_none_avx512.h"
#include "avx512_helpers.h"
#include "compressed_prb_packer.h"
#include "packing_utils_avx512.h"
#include "quantizer.h"
#include "srsran/srsvec/dot_prod.h"

using namespace srsran;
using namespace ofh;

void iq_compression_none_avx512::compress(span<compressed_prb>         output,
                                          span<const cbf16_t>          input,
                                          const ru_compression_params& params)
{
  // Number of quantized samples per resource block.
  static constexpr size_t NOF_SAMPLES_PER_PRB = 2 * NOF_SUBCARRIERS_PER_RB;

  // Quantizer object.
  quantizer q(params.data_width);

  // Use generic implementation if AVX512 utils don't support requested bit width.
  if (!mm512::iq_width_packing_supported(params.data_width)) {
    iq_compression_none_impl::compress(output, input, params);
    return;
  }

  // Auxiliary arrays used for float to fixed point conversion of the input data.
  std::array<int16_t, NOF_SAMPLES_PER_PRB * MAX_NOF_PRBS> input_quantized;

  span<const bf16_t> float_samples_span(reinterpret_cast<const bf16_t*>(input.data()), input.size() * 2U);
  span<int16_t>      input_quantized_span(input_quantized.data(), input.size() * 2U);
  q.to_fixed_point(input_quantized_span, float_samples_span, iq_scaling);

  log_post_quantization_rms(input_quantized_span);

  unsigned        sample_idx = 0;
  unsigned        rb         = 0;
  const __mmask32 load_mask  = 0x00ffffff;

  // Process one PRB at a time.
  for (size_t rb_index_end = output.size(); rb != rb_index_end; ++rb) {
    // Load 16-bit IQ samples from non-aligned memory.
    __m512i rb_epi16 = _mm512_maskz_loadu_epi16(load_mask, &input_quantized[sample_idx]);

    // Pack using utility function.
    mm512::pack_prb_big_endian(output[rb], rb_epi16, params.data_width);

    sample_idx += NOF_SAMPLES_PER_PRB;
  }
}

void iq_compression_none_avx512::decompress(span<srsran::cf_t>                        output,
                                            span<const srsran::ofh::compressed_prb>   input,
                                            const srsran::ofh::ru_compression_params& params)
{
  // Use generic implementation if AVX512 utils don't support requested bit width.
  if (!mm512::iq_width_packing_supported(params.data_width)) {
    iq_compression_none_impl::decompress(output, input, params);
    return;
  }

  // Quantizer object.
  quantizer q(params.data_width);

  unsigned out_idx = 0;
  for (const auto& c_prb : input) {
    std::array<int16_t, NOF_SUBCARRIERS_PER_RB * 2> unpacked_iq_data;
    // Unpack resource block.
    mm512::unpack_prb_big_endian(unpacked_iq_data, c_prb.get_packed_data(), params.data_width);

    span<cf_t>    output_span = output.subspan(out_idx, NOF_SUBCARRIERS_PER_RB);
    span<int16_t> unpacked_span(unpacked_iq_data.data(), NOF_SUBCARRIERS_PER_RB * 2);

    // Convert to complex samples.
    q.to_float(output_span, unpacked_span, 1);
    out_idx += NOF_SUBCARRIERS_PER_RB;
  }
}
