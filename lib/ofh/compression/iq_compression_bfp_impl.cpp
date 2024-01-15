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

#include "iq_compression_bfp_impl.h"
#include "compressed_prb_packer.h"
#include "compressed_prb_unpacker.h"
#include "srsran/srsvec/dot_prod.h"

using namespace srsran;
using namespace ofh;

void iq_compression_bfp_impl::quantize_input(span<int16_t> out, span<const float> in)
{
  srsran_assert(in.size() == out.size(), "Input and output spans must have the same size");

  // Quantizer object.
  quantizer q(Q_BIT_WIDTH);

  // Convert input to int16_t representation.
  q.to_fixed_point(out, in, iq_scaling);

  if (SRSRAN_UNLIKELY(logger.debug.enabled() && !out.empty())) {
    // Calculate and print RMS of quantized samples.
    float sum_squares = srsvec::dot_prod(out, out, 0);
    float rms         = std::sqrt(sum_squares / out.size());
    if (std::isnormal(rms)) {
      logger.debug("Quantized IQ samples RMS value of '{}'", rms);
    }
  }
}

void iq_compression_bfp_impl::compress_prb_generic(compressed_prb&     c_prb,
                                                   span<const int16_t> input_quantized,
                                                   unsigned            data_width)
{
  // Determine maximum absolute value and the exponent.
  const auto* start_it = input_quantized.begin();
  const auto* max_it   = std::max_element(start_it, start_it + NOF_SAMPLES_PER_PRB);
  const auto* min_it   = std::min_element(start_it, start_it + NOF_SAMPLES_PER_PRB);
  unsigned    max_abs  = std::max(std::abs(*max_it), std::abs(*min_it) - 1);

  uint8_t exponent = determine_exponent(max_abs, data_width);

  // Auxiliary arrays to store compressed samples before packing.
  std::array<int16_t, NOF_SAMPLES_PER_PRB> compressed_samples;

  // Compress data.
  for (unsigned i = 0; i != NOF_SAMPLES_PER_PRB; ++i) {
    compressed_samples[i] = input_quantized[i] >> exponent;
  }

  c_prb.set_compression_param(exponent);

  compressed_prb_packer packer(c_prb);
  packer.pack(compressed_samples, data_width);
}

void iq_compression_bfp_impl::compress(span<compressed_prb>         output,
                                       span<const cf_t>             input,
                                       const ru_compression_params& params)
{
  // Auxiliary arrays used for float to fixed point conversion of the input data.
  std::array<int16_t, NOF_SAMPLES_PER_PRB * MAX_NOF_PRBS> input_quantized;

  span<const float> float_samples_span(reinterpret_cast<const float*>(input.data()), input.size() * 2U);
  span<int16_t>     input_quantized_span(input_quantized.data(), input.size() * 2U);
  // Performs conversion of input complex float values to signed 16bit integers.
  quantize_input(input_quantized_span, float_samples_span);

  unsigned sample_idx = 0;
  for (auto& c_prb : output) {
    const auto* start_it = input_quantized.begin() + sample_idx;
    // Compress one resource block.
    compress_prb_generic(c_prb, {start_it, NOF_SAMPLES_PER_PRB}, params.data_width);
    sample_idx += NOF_SAMPLES_PER_PRB;
  }
}

void iq_compression_bfp_impl::decompress_prb_generic(span<cf_t>            output,
                                                     const compressed_prb& c_prb,
                                                     const quantizer&      q_in,
                                                     unsigned              data_width)
{
  // Quantizer.
  quantizer q_out(Q_BIT_WIDTH);

  uint8_t exponent = c_prb.get_compression_param();
  int16_t scaler   = 1 << exponent;

  compressed_prb_unpacker unpacker(c_prb);
  for (unsigned i = 0, read_pos = 0; i != NOF_SUBCARRIERS_PER_RB; ++i) {
    int16_t re = q_in.sign_extend(unpacker.unpack(read_pos, data_width));
    int16_t im = q_in.sign_extend(unpacker.unpack(read_pos + data_width, data_width));
    read_pos += (data_width * 2);

    float scaled_re = q_out.to_float(re * scaler);
    float scaled_im = q_out.to_float(im * scaler);
    output[i]       = {scaled_re, scaled_im};
  }
}

void iq_compression_bfp_impl::decompress(span<cf_t>                   output,
                                         span<const compressed_prb>   input,
                                         const ru_compression_params& params)
{
  // Quantizer.
  quantizer q_in(params.data_width);

  unsigned out_idx = 0;
  for (const auto& c_prb : input) {
    span<cf_t> out_rb_samples = output.subspan(out_idx, NOF_SUBCARRIERS_PER_RB);
    // Decompress resource block.
    decompress_prb_generic(out_rb_samples, c_prb, q_in, params.data_width);
    out_idx += NOF_SUBCARRIERS_PER_RB;
  }
}
