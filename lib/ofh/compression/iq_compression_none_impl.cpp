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

#include "iq_compression_none_impl.h"
#include "packing_utils_generic.h"
#include "quantizer.h"
#include "srsran/ofh/compression/compression_properties.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/support/units.h"

using namespace srsran;
using namespace ofh;

void iq_compression_none_impl::compress(span<uint8_t>                buffer,
                                        span<const cbf16_t>          iq_data,
                                        const ru_compression_params& params)
{
  // Quantizer object.
  quantizer q(params.data_width);

  // Number of input PRBs.
  unsigned nof_prbs = (iq_data.size() / NOF_SUBCARRIERS_PER_RB);

  // Size in bytes of one compressed PRB using the given compression parameters.
  unsigned prb_size = get_compressed_prb_size(params).value();

  srsran_assert(buffer.size() >= prb_size * nof_prbs, "Output buffer doesn't have enough space to decompress PRBs");

  span<const bf16_t> float_samples(reinterpret_cast<const bf16_t*>(iq_data.data()), iq_data.size() * 2);

  unsigned in_sample_idx = 0;
  for (unsigned rb = 0; rb != nof_prbs; ++rb) {
    // Auxiliary buffer used for brain float to int16_t conversion.
    std::array<int16_t, NOF_SUBCARRIERS_PER_RB * 2> conv_buffer;

    // Scale input IQ data to the range [-1: +1) and convert it to int16_t.
    q.to_fixed_point(conv_buffer, float_samples.subspan(in_sample_idx, NOF_SUBCARRIERS_PER_RB * 2), iq_scaling);

    // View over output bytes corresponding to the current PRB.
    span<uint8_t> out_compr_prb(&buffer[rb * prb_size], prb_size);

    bit_buffer bit_buf = bit_buffer::from_bytes(out_compr_prb);
    pack_bytes(bit_buf, conv_buffer, params.data_width);

    in_sample_idx += (NOF_SUBCARRIERS_PER_RB * 2);
  }
}

void iq_compression_none_impl::decompress(span<cbf16_t>                output,
                                          span<const uint8_t>          input,
                                          const ru_compression_params& params)
{
  // Quantizer object.
  quantizer q(params.data_width);

  // Number of output PRBs.
  unsigned nof_prbs = output.size() / NOF_SUBCARRIERS_PER_RB;

  // Size in bytes of one compressed PRB using the given compression parameters.
  unsigned prb_size = get_compressed_prb_size(params).value();

  srsran_assert(
      input.size() >= nof_prbs * prb_size, "Input does not contain enough bytes to decompress {} PRBs", nof_prbs);

  unsigned out_idx = 0;
  for (unsigned c_prb_idx = 0, e = nof_prbs; c_prb_idx != e; ++c_prb_idx) {
    span<const uint8_t> c_prb(&input[c_prb_idx * prb_size], prb_size);

    bit_buffer_reader bits_reader = bit_buffer_reader::from_bytes(c_prb);
    for (unsigned i = 0, read_pos = 0; i != NOF_SUBCARRIERS_PER_RB; ++i) {
      int16_t re = q.sign_extend(unpack_bits(bits_reader, read_pos, params.data_width));
      int16_t im = q.sign_extend(unpack_bits(bits_reader, read_pos + params.data_width, params.data_width));
      read_pos += (params.data_width * 2);
      output[out_idx++] = {q.to_float(re), q.to_float(im)};
    }
  }
}

void iq_compression_none_impl::log_post_quantization_rms(span<const int16_t> samples)
{
  if (SRSRAN_UNLIKELY(logger.debug.enabled() && !samples.empty())) {
    // Calculate and print RMS of quantized samples.
    float sum_squares = srsvec::dot_prod(samples, samples, 0);
    float rms         = std::sqrt(sum_squares / samples.size());
    if (std::isnormal(rms)) {
      logger.debug("Quantized IQ samples RMS value of '{}'", rms);
    }
  }
}
