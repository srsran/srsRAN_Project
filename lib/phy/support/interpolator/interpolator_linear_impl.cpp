/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Linear interpolator definition.

#include "interpolator_linear_impl.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/support/srsgnb_assert.h"

using namespace srsran;

void interpolator_linear_impl::interpolate(span<cf_t> output, span<const cf_t> input, const configuration& cfg)
{
  srsgnb_assert(output.size() >= (input.size() - 1) * cfg.stride + 1 + cfg.offset,
                "The output length is {}. With input length {}, offset {} and stride {} it should be at least {}.",
                output.size(),
                input.size(),
                cfg.offset,
                cfg.stride,
                (input.size() - 1) * cfg.stride + 1 + cfg.offset);

  srsgnb_assert(!input.empty(), "At least one known value should be provided.");

  srsgnb_assert(cfg.stride > 0, "The pattern stride {} cannot be less than 1.", cfg.stride);

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

  unsigned i_output = cfg.offset;
  unsigned i_input  = 0;

  // First, fill the offset with the first known value.
  std::fill_n(output.begin(), i_output + 1, input[i_input]);

  // Linear interpolation.
  for (unsigned next = i_output + cfg.stride, out_length = output.size(); next < out_length; next += cfg.stride) {
    cf_t       jump       = (input[i_input + 1] - input[i_input]) / static_cast<float>(cfg.stride);
    span<cf_t> output_tmp = output.subspan(i_output + 1, cfg.stride);
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
