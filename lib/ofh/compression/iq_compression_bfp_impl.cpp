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

#include "iq_compression_bfp_impl.h"
#include "packing_utils_generic.h"
#include "srsran/ofh/compression/compression_properties.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/support/units.h"

using namespace srsran;
using namespace ofh;

void iq_compression_bfp_impl::quantize_input(span<int16_t> out, span<const bf16_t> in)
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

void iq_compression_bfp_impl::compress_prb_generic(span<uint8_t>       comp_prb_buffer,
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

  // Save exponent.
  std::memcpy(comp_prb_buffer.data(), &exponent, sizeof(exponent));
  comp_prb_buffer = comp_prb_buffer.last(comp_prb_buffer.size() - sizeof(exponent));

  bit_buffer buffer = bit_buffer::from_bytes(comp_prb_buffer);
  pack_bytes(buffer, compressed_samples, data_width);
}

void iq_compression_bfp_impl::compress(span<uint8_t>                buffer,
                                       span<const cbf16_t>          iq_data,
                                       const ru_compression_params& params)
{
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

  for (unsigned i = 0; i != nof_prbs; ++i) {
    const auto* in_start_it = input_quantized.begin() + NOF_SAMPLES_PER_PRB * i;
    auto*       out_it      = &buffer[i * prb_size];
    // Compress one resource block.
    compress_prb_generic({out_it, prb_size}, {in_start_it, NOF_SAMPLES_PER_PRB}, params.data_width);
  }
}

void iq_compression_bfp_impl::decompress_prb_generic(span<cbf16_t>       output,
                                                     span<const uint8_t> comp_prb,
                                                     const quantizer&    q_in,
                                                     unsigned            data_width)
{
  // Quantizer.
  quantizer q_out(Q_BIT_WIDTH);

  // Compute scaling factor, first byte contains the exponent.
  uint8_t exponent = comp_prb[0];
  int16_t scaler   = 1 << exponent;

  comp_prb             = comp_prb.last(comp_prb.size() - sizeof(exponent));
  auto bit_buff_reader = bit_buffer_reader::from_bytes(comp_prb);

  for (unsigned i = 0, read_pos = 0; i != NOF_SUBCARRIERS_PER_RB; ++i) {
    int16_t re = q_in.sign_extend(unpack_bits(bit_buff_reader, read_pos, data_width));
    int16_t im = q_in.sign_extend(unpack_bits(bit_buff_reader, read_pos + data_width, data_width));
    read_pos += (data_width * 2);

    float scaled_re = q_out.to_float(re * scaler);
    float scaled_im = q_out.to_float(im * scaler);
    output[i]       = {scaled_re, scaled_im};
  }
}

void iq_compression_bfp_impl::decompress(span<cbf16_t>                iq_data,
                                         span<const uint8_t>          compressed_data,
                                         const ru_compression_params& params)
{
  // Quantizer.
  quantizer q_in(params.data_width);

  // Number of output PRBs.
  unsigned nof_prbs = iq_data.size() / NOF_SUBCARRIERS_PER_RB;

  // Size in bytes of one compressed PRB using the given compression parameters.
  unsigned prb_size = get_compressed_prb_size(params).value();

  srsran_assert(compressed_data.size() >= nof_prbs * prb_size,
                "Input does not contain enough bytes to decompress {} PRBs",
                nof_prbs);

  unsigned out_idx = 0;
  for (unsigned c_prb_idx = 0; c_prb_idx != nof_prbs; ++c_prb_idx) {
    span<const uint8_t> comp_prb(&compressed_data[c_prb_idx * prb_size], prb_size);

    span<cbf16_t> out_prb_samples = iq_data.subspan(out_idx, NOF_SUBCARRIERS_PER_RB);
    // Decompress resource block.
    decompress_prb_generic(out_prb_samples, comp_prb, q_in, params.data_width);
    out_idx += NOF_SUBCARRIERS_PER_RB;
  }
}
