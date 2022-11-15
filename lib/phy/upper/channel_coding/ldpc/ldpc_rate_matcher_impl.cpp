/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ldpc_rate_matcher_impl.h"
#include "ldpc_luts_impl.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/support/srsgnb_assert.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

void ldpc_rate_matcher_impl::init(const codeblock_metadata::tb_common_metadata& cfg)
{
  srsgnb_assert((cfg.rv >= 0) && (cfg.rv <= 3), "RV should an integer between 0 and 3.");
  rv = cfg.rv;

  modulation_order = static_cast<uint8_t>(cfg.mod);
}

void ldpc_rate_matcher_impl::rate_match(span<uint8_t>                                 output,
                                        span<const uint8_t>                           input,
                                        const codeblock_metadata::tb_common_metadata& cfg)
{
  init(cfg);

  unsigned block_length = input.size();

  if (cfg.Nref > 0) {
    buffer_length = std::min(cfg.Nref, block_length);
  } else {
    buffer_length = block_length;
  }

  // The output size cannot be larger than the maximum rate-matched codeblock length.
  srsgnb_assert(output.size() <= MAX_CODEBLOCK_RM_SIZE,
                "The length of the rate-matched codeblock is {} but it shouldn't be more than {}.",
                output.size(),
                MAX_CODEBLOCK_RM_SIZE);

  // The output size must be a multiple of the modulation order.
  srsgnb_assert(output.size() % modulation_order == 0,
                "The output length should be a multiple of the modulation order.");

  // Compute shift_k0 according to TS38.212 Table 5.4.2.1-2
  std::array<double, 4> shift_factor{};
  uint16_t              lifting_size{};
  if (block_length % BG1_N_SHORT == 0) {
    // input is a BG1 codeblock
    shift_factor = {0, 17, 33, 56};
    lifting_size = block_length / BG1_N_SHORT;
  } else if (block_length % BG2_N_SHORT == 0) {
    // input is a BG2 codeblock
    shift_factor = {0, 13, 25, 43};
    lifting_size = block_length / BG2_N_SHORT;
  } else {
    srsgnb_assert(false, "LDPC rate matching: invalid input length.");
  }
  srsgnb_assert(get_lifting_index(static_cast<lifting_size_t>(lifting_size)) != VOID_LIFTSIZE,
                "LDPC rate matching: invalid input length.");
  double tmp = (shift_factor[rv] * buffer_length) / block_length;
  shift_k0   = static_cast<uint16_t>(floor(tmp)) * lifting_size;

  buffer = input.first(buffer_length);

  if (modulation_order == 1) {
    select_bits(output, buffer);
  } else {
    span<uint8_t> aux = span<uint8_t>(auxiliary_buffer).first(output.size());
    select_bits(aux, buffer);
    interleave_bits(output, aux);
  }
}

void ldpc_rate_matcher_impl::select_bits(span<uint8_t> out, span<const uint8_t> in) const
{
  unsigned output_length = out.size();
  unsigned out_index     = 0;
  unsigned in_index      = shift_k0;

  // Equivalent code:
  // unsigned in_index = shift_k0 % buffer_length;
  // for (auto& this_out : out) {
  //   while (in[in_index] == FILLER_BIT) {
  //     in_index = (in_index + 1) % buffer_length;
  //   }
  //   this_out = in[in_index];
  //   in_index = (in_index + 1) % buffer_length;
  // }

  // as long as the ouput index does not reach the output length...
  while (out_index < output_length) {
    // Skip filler bits.
    while (in[in_index] == FILLER_BIT) {
      in_index = (in_index + 1) % buffer_length;
    }

    // Select remainder input.
    span<const uint8_t> input_chunk = in.subspan(in_index, in.size() - in_index);

    // Find the first filler bit, or end of the input buffer if no filler bit is found. This indicates the end of
    // contiguous data to read from the input buffer.
    const uint8_t* filler_bit_it = srsvec::find(input_chunk, FILLER_BIT);

    // Calculate number of bits to copy from in_index to the end of the input contiguous data.
    unsigned count = static_cast<unsigned>(filler_bit_it - input_chunk.begin());

    // Trim the count to the remainder bits.
    count = std::min(count, output_length - out_index);

    // Append the consecutive number of bits.
    srsvec::copy(out.subspan(out_index, count), in.subspan(in_index, count));
    out_index += count;

    // Advance in_index the amount of written bits.
    in_index = (in_index + count) % buffer_length;
  }
}

namespace {
template <unsigned Qm>
void interleave_bits_Qm(span<uint8_t> out, span<const uint8_t> in)
{
  unsigned E = out.size();
  unsigned K = E / Qm;
  for (unsigned out_index = 0, i = 0; i != K; ++i) {
    for (unsigned j = 0; j != Qm; ++j, ++out_index) {
      out[out_index] = in[K * j + i];
    }
  }
}
} // namespace

void ldpc_rate_matcher_impl::interleave_bits(span<uint8_t> out, span<const uint8_t> in) const
{
  switch (modulation_order) {
    case 2:
      interleave_bits_Qm<2>(out, in);
      break;
    case 4:
      interleave_bits_Qm<4>(out, in);
      break;
    case 6:
      interleave_bits_Qm<6>(out, in);
      break;
    case 8:
    default:
      interleave_bits_Qm<8>(out, in);
      break;
  }
}
