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

#include "short_block_encoder_impl.h"
#include "srsran/adt/static_vector.h"
#include "srsran/srsvec/binary.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

// Maximum message length.
static constexpr unsigned MAX_IN_BITS = 11;
// Maximum codeblock length.
static constexpr unsigned MAX_BLOCK_BITS = 32;

static constexpr std::array<std::array<uint8_t, MAX_BLOCK_BITS>, MAX_IN_BITS> BASIS_SEQUENCES = {
    {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
     {1, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0},
     {0, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0},
     {0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0},
     {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0},
     {0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0},
     {0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 0},
     {0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0},
     {0, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 0},
     {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0}}};

static void validate_spans(span<uint8_t> output, span<const uint8_t> input, unsigned bits_per_symbol)
{
  unsigned in_size  = input.size();
  unsigned out_size = output.size();
  srsran_assert((in_size > 0) && (in_size <= MAX_IN_BITS), "The input length should be between 1 and 11 bits.");
  if (in_size > 2) {
    // Output length should be no less than MAX_OUT_BITS.
    srsran_assert(out_size >= MAX_BLOCK_BITS, "Invalid output length.");
  } else {
    unsigned min_out_length = 0;
    if (in_size == 1) {
      // Output length must be equal to the number of bits per symbol of the block modulation.
      min_out_length = bits_per_symbol;
    } else if (in_size == 2) {
      // Output length must be equal to three times the number of bits per symbol of the block modulation.
      min_out_length = 3 * bits_per_symbol;
    }
    srsran_assert(out_size >= min_out_length, "Invalid output length.");
  }
}

// Encoder function for a single bit.
static void encode_1(span<uint8_t> output, span<const uint8_t> input)
{
  std::fill(output.begin(), output.end(), PLACEHOLDER_ONE);
  output[0] = input[0];
  if (output.size() > 1) {
    output[1] = PLACEHOLDER_REPEAT;
  }
}

// Encoder function for sequences of 2 bits.
static void encode_2(span<uint8_t> output, span<const uint8_t> input)
{
  std::fill(output.begin(), output.end(), PLACEHOLDER_ONE);

  uint8_t c0 = input[0];
  uint8_t c1 = input[1];
  uint8_t c2 = c0 ^ c1;

  output[0] = c0;
  output[1] = c1;
  if (output.size() == 3) {
    output[2] = c2;
  } else {
    // validate_spans ensures that output.size() is a multiple of 3.
    unsigned step = output.size() / 3;

    output[step]           = c2;
    output[step + 1]       = c0;
    output[2UL * step]     = c1;
    output[2UL * step + 1] = c2;
  }
}

// Encoder function for sequences of length between 3 and 11 bits.
static void encode_3_11(span<uint8_t> output, span<const uint8_t> input)
{
  // Ensure output is initialized to zero.
  std::fill(output.begin(), output.end(), 0);

  for (unsigned c_i = 0, length = input.size(); c_i != length; ++c_i) {
    if (input[c_i] == 1) {
      srsvec::binary_xor(output, BASIS_SEQUENCES[c_i], output);
    }
  }
}

// Matches the rate of a short block according to TS38.212 Section 5.4.3.
static void rate_match(span<uint8_t> output, span<const uint8_t> input)
{
  unsigned output_size = output.size();
  unsigned input_size  = input.size();
  srsran_assert(output_size >= input_size, "Output size cannot be smaller than input size.");

  for (unsigned idx = 0; idx != output_size; ++idx) {
    output[idx] = input[idx % input_size];
  }
}

void short_block_encoder_impl::encode(span<uint8_t> output, span<const uint8_t> input, modulation_scheme mod)
{
  unsigned bits_per_symbol = get_bits_per_symbol(mod);
  validate_spans(output, input, bits_per_symbol);

  static_vector<uint8_t, MAX_BLOCK_BITS> tmp(0);

  switch (input.size()) {
    case 1:
      tmp.resize(bits_per_symbol);
      encode_1(tmp, input);
      break;
    case 2:
      tmp.resize(3UL * bits_per_symbol);
      encode_2(tmp, input);
      break;
    default:
      tmp.resize(MAX_BLOCK_BITS);
      encode_3_11(tmp, input);
  }
  rate_match(output, tmp);
}

std::unique_ptr<short_block_encoder> srsran::create_short_block_encoder()
{
  return std::make_unique<short_block_encoder_impl>();
}
