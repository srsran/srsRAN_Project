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

/// \file
/// \brief Linear interpolator definition.

#include "interpolator_linear_impl.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/simd.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

static void simd_interpolate(span<cf_t> output, span<const cf_t> input, unsigned offset, unsigned stride)
{
  unsigned i_output = offset;
  unsigned i_input  = 0;

  // First, fill the offset with the first known value.
  std::fill_n(output.begin(), i_output + 1, input[i_input]);

#if SRSRAN_SIMD_CF_SIZE
  // Maximum interpolation factor.
  static constexpr unsigned max_simd_stride = 3;
  if (stride <= max_simd_stride) {
    // Linear interpolation filter, it is constant across all the SIMD batches.
    static_vector<float, max_simd_stride * SRSRAN_SIMD_CF_SIZE> interpolate_stairs(stride * SRSRAN_SIMD_CF_SIZE);
    for (unsigned i = 0; i != stride * SRSRAN_SIMD_CF_SIZE; ++i) {
      interpolate_stairs[i] = static_cast<float>(i % stride) / static_cast<float>(stride);
    }

    // Temporary up-sampled input. Separated in real and imaginary to avoid interleaving and deinterleaving. Also, It
    // leaves extra space for one more SIMD register.
    static constexpr unsigned max_temp_input_size =
        max_simd_stride * (SRSRAN_SIMD_CF_SIZE + 1) + (SRSRAN_SIMD_CF_SIZE - max_simd_stride);
    std::array<float, max_temp_input_size> interpolate_input_re;
    std::array<float, max_temp_input_size> interpolate_input_im;

    // Process SIMD batch in chunks of SIMD register size.
    for (unsigned i_input_end = ((input.size() - 1) / SRSRAN_SIMD_CF_SIZE) * SRSRAN_SIMD_CF_SIZE;
         i_input != i_input_end;
         i_input += SRSRAN_SIMD_CF_SIZE) {
      // Load input and repeat values to match the up sampling factor. It assumes the maximum stride is smaller than or
      // equal to the register size.
      static_assert(max_simd_stride <= SRSRAN_SIMD_CF_SIZE);
      for (unsigned i = 0; i != SRSRAN_SIMD_CF_SIZE + 1; ++i) {
        // Prepare real part.
        simd_f_t simd_in_re = srsran_simd_f_set1(input[i_input + i].real());
        srsran_simd_f_storeu(&interpolate_input_re[i * stride], simd_in_re);

        // Prepare imaginary part.
        simd_f_t simd_in_im = srsran_simd_f_set1(input[i_input + i].imag());
        srsran_simd_f_storeu(&interpolate_input_im[i * stride], simd_in_im);
      }

      // Process each of the SIMD registers.
      for (unsigned i_simd = 0; i_simd != stride; ++i_simd) {
        // Load the temporary data into SIMD registers.
        simd_f_t  simd_interpolate_stairs = srsran_simd_f_loadu(&interpolate_stairs[i_simd * SRSRAN_SIMD_CF_SIZE]);
        simd_cf_t simd_interpolate_input0 = srsran_simd_cf_loadu(&interpolate_input_re[i_simd * SRSRAN_SIMD_CF_SIZE],
                                                                 &interpolate_input_im[i_simd * SRSRAN_SIMD_CF_SIZE]);
        simd_cf_t simd_interpolate_input1 =
            srsran_simd_cf_loadu(&interpolate_input_re[i_simd * SRSRAN_SIMD_CF_SIZE + stride],
                                 &interpolate_input_im[i_simd * SRSRAN_SIMD_CF_SIZE + stride]);

        // Calculate results.
        simd_cf_t simd_result =
            (simd_interpolate_input1 - simd_interpolate_input0) * simd_interpolate_stairs + simd_interpolate_input0;

        // Store and advance output index.
        srsran_simd_cfi_storeu(&output[i_output], simd_result);
        i_output += SRSRAN_SIMD_CF_SIZE;
      }
    }
    output[i_output] = input[i_input];
  }
#endif // SRSRAN_SIMD_CF_SIZE

  // Linear interpolation.
  for (unsigned next = i_output + stride, out_length = output.size(); next < out_length; next += stride) {
    cf_t       jump       = (input[i_input + 1] - input[i_input]) / static_cast<float>(stride);
    span<cf_t> output_tmp = output.subspan(i_output + 1, stride);
    std::generate(output_tmp.begin(), output_tmp.end(), [jump, val = output[i_output]]() mutable {
      val += jump;
      return val;
    });
    i_output = next;
    ++i_input;
  }

  // At this point, "current" points at the last known observation. Repeat its value for all the remaining missing
  // observations.
  std::fill(output.begin() + i_output + 1, output.end(), input[i_input]);
}

void interpolator_linear_impl::interpolate(span<cf_t> output, span<const cf_t> input, const configuration& cfg)
{
  srsran_assert(output.size() >= (input.size() - 1) * cfg.stride + 1 + cfg.offset,
                "The output length is {}. With input length {}, offset {} and stride {} it should be at least {}.",
                output.size(),
                input.size(),
                cfg.offset,
                cfg.stride,
                (input.size() - 1) * cfg.stride + 1 + cfg.offset);

  srsran_assert(!input.empty(), "At least one known value should be provided.");

  srsran_assert(cfg.stride > 0, "The pattern stride {} cannot be less than 1.", cfg.stride);

  // Trivial case - no missing values, nothing to do but copying.
  if (output.size() == input.size()) {
    srsvec::copy(output, input);
    return;
  }

  // Trivial case - only one value given, repeat it throughout the output.
  if (input.size() == 1) {
    std::fill(output.begin(), output.end(), input[0]);
    return;
  }

  simd_interpolate(output, input, cfg.offset, cfg.stride);
}
