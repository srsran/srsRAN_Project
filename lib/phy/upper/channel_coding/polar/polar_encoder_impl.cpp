/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "polar_encoder_impl.h"

using namespace srsgnb;

polar_encoder_impl::polar_encoder_impl(unsigned code_size_log_) :
  max_code_size_log(static_cast<uint8_t>(code_size_log_)),
  max_code_size(1U << max_code_size_log),
  max_code_half_size(max_code_size / 2),
  i_even(max_code_half_size),
  i_odd(max_code_half_size),
  tmp(max_code_size)
{
  for (uint16_t i = 0; i != max_code_size / 2; ++i) {
    i_even[i] = 2 * i;
    i_odd[i]  = 2 * i + 1;
  }
}

void polar_encoder_impl::encode(span<const uint8_t> input, unsigned code_size_log, span<uint8_t> output)
{
  // first stage also initializes output vector
  uint16_t code_half_size = 1U << (code_size_log - 1U);

  // Make sure it does not exceed the maximum allocated size
  assert(code_half_size <= max_code_half_size);

  for (uint16_t j = 0; j != code_half_size; ++j) {
    tmp[j]                  = input[i_even[j]];
    tmp[j + code_half_size] = input[i_odd[j]];
  }

  for (uint16_t j = 0; j != code_half_size; ++j) {
    output[i_odd[j]]  = tmp[i_odd[j]];
    output[i_even[j]] = tmp[i_even[j]] ^ tmp[i_odd[j]]; // bitXor
  }

  // remaining stages
  for (uint16_t i = 1; i != code_size_log; ++i) {
    for (uint16_t j = 0; j != code_half_size; ++j) {
      tmp[j]                  = output[i_even[j]];
      tmp[j + code_half_size] = output[i_odd[j]];
    }

    for (uint16_t j = 0; j != code_half_size; ++j) {
      output[i_odd[j]]  = tmp[i_odd[j]];
      output[i_even[j]] = tmp[i_even[j]] ^ tmp[i_odd[j]]; // bitXor
    }
  }
}

std::unique_ptr<polar_encoder> srsgnb::create_polar_encoder_pipelined(unsigned code_size_log)
{
  return std::make_unique<polar_encoder_impl>(code_size_log);
}
