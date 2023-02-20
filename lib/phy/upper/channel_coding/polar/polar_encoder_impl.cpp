/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "polar_encoder_impl.h"
#include "srsran/phy/upper/channel_coding/polar/polar_code.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

namespace {

// Templated recursive stage function.
template <unsigned CODE_SIZE_LOG>
void stage_function(uint8_t* output, const uint8_t* input)
{
  unsigned code_size      = (1U << (CODE_SIZE_LOG));
  unsigned code_half_size = code_size / 2;

  stage_function<CODE_SIZE_LOG - 1>(output, input);
  stage_function<CODE_SIZE_LOG - 1>(output + code_half_size, input + code_half_size);

  for (unsigned i_bit = 0; i_bit != code_half_size; ++i_bit) {
    output[i_bit] = output[i_bit] ^ output[i_bit + code_half_size];
  }
}

// Recursive stage function for 2 input bits.
template <>
void stage_function<1>(uint8_t* output, const uint8_t* input)
{
  output[0] = input[0] ^ input[1];
  output[1] = input[1];
}

// Recursive polar encoder.
template <unsigned MAX_CODE_SIZE_LOG>
void polar_encode(uint8_t* output, const uint8_t* input, unsigned code_size_log)
{
  // If the code size match, run the stage function of the size.
  if (MAX_CODE_SIZE_LOG == code_size_log) {
    stage_function<MAX_CODE_SIZE_LOG>(output, input);
    return;
  }

  // Otherwise, check the next smaller size.
  polar_encode<MAX_CODE_SIZE_LOG - 1>(output, input, code_size_log);
}

// Recursive polar encoder of size 2.
template <>
void polar_encode<1>(uint8_t* output, const uint8_t* input, unsigned /**/)
{
  stage_function<1>(output, input);
}

} // namespace

void polar_encoder_impl::encode(span<uint8_t> output, span<const uint8_t> input, unsigned code_size_log)
{
  srsgnb_assert(code_size_log > 0 && code_size_log <= polar_code::eMAX,
                "The logarithmic code size {} is invalid. It must be greater than zero and less than or equal to {}.",
                code_size_log,
                static_cast<unsigned>(polar_code::eMAX));

  // Start with the largest encoder.
  polar_encode<polar_code::eMAX>(output.data(), input.data(), code_size_log);
}
