#include "short_block_encoder_impl.h"
#include "srsgnb/srsvec/binary.h"
#include "srsgnb/support/srsran_assert.h"

#include <iostream>

using namespace srsgnb;

static constexpr unsigned MAX_IN_BITS  = 11;
static constexpr unsigned MAX_OUT_BITS = 32;

// TODO(david): this should be constexpr, but then the rows are not accepted as inputs to binary_xor.
static std::array<std::array<uint8_t, MAX_OUT_BITS>, MAX_IN_BITS> basis_sequences = {
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

static void validate_spans(span<uint8_t> output, span<const uint8_t> input)
{
  unsigned in_size  = input.size();
  unsigned out_size = output.size();
  srsran_assert((in_size > 0) && (in_size <= MAX_IN_BITS), "The input length should be between 1 and 11 bits.");
  if (in_size > 2) {
    srsran_assert(out_size == MAX_OUT_BITS, "Invalid output length.");
  } else {
    std::array<unsigned, 5> out_lengths = {};
    if (in_size == 1) {
      // Output length must be equal to the number of bits per symbol of the block modulation.
      out_lengths = {1, 2, 4, 6, 8};
    } else if (in_size == 2) {
      // Output length must be equal to three times the number of bits per symbol of the block modulation.
      out_lengths = {3, 6, 12, 18, 24};
    }
    auto this_length = {out_size};
    srsran_assert(std::includes(out_lengths.cbegin(), out_lengths.cend(), this_length.begin(), this_length.end()),
                  "Invalid output length.");
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
      srsvec::binary_xor(output, span<uint8_t>(basis_sequences[c_i]), output);
    }
  }
}

void short_block_encoder_impl::encode(span<uint8_t> output, span<const uint8_t> input)
{
  validate_spans(output, input);
  switch (input.size()) {
    case 1:
      encode_1(output, input);
      break;
    case 2:
      encode_2(output, input);
      break;
    default:
      encode_3_11(output, input);
  }
}

void short_block_encoder_impl::rate_match(span<uint8_t> output, span<const uint8_t> input)
{
  unsigned output_size = output.size();
  unsigned input_size  = input.size();
  srsran_assert(output_size >= input_size, "Output size cannot be smaller than input size.");

  for (unsigned idx = 0; idx != output_size; ++idx) {
    output[idx] = input[idx % input_size];
  }
}

std::unique_ptr<short_block_encoder> srsgnb::create_short_block_encoder()
{
  return std::make_unique<short_block_encoder_impl>();
}
