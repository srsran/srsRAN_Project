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
#include "srsgnb/support/srsran_assert.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

void ldpc_rate_matcher_impl::init(const codeblock_metadata::tb_common_metadata& cfg)
{
  srsran_assert((cfg.rv >= 0) && (cfg.rv <= 3), "RV should an integer between 0 and 3.");
  rv = cfg.rv;

  modulation_order = static_cast<uint8_t>(cfg.mod);
}

void ldpc_rate_matcher_impl::rate_match(span<uint8_t>                                 output,
                                        span<const uint8_t>                           input,
                                        const codeblock_metadata::tb_common_metadata& cfg)
{
  init(cfg);

  unsigned block_length = input.size();

  srsran_assert(block_length >= cfg.Nref, "N_ref should be smaller than the input length.");
  if (cfg.Nref > 0) {
    buffer_length = cfg.Nref;
  } else {
    buffer_length = block_length;
  }

  // The output size must be a multiple of the modulation order.
  srsran_assert(output.size() % modulation_order == 0,
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
    srsran_assert(false, "LDPC rate matching: invalid input length.");
  }
  srsran_assert(get_lifting_index(static_cast<lifting_size_t>(lifting_size)) != VOID_LIFTSIZE,
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
  unsigned in_index = shift_k0 % buffer_length;
  for (auto& this_out : out) {
    while (in[in_index] == FILLER_BIT) {
      in_index = (in_index + 1) % buffer_length;
    }
    this_out = in[in_index];
    in_index = (in_index + 1) % buffer_length;
  }
}

void ldpc_rate_matcher_impl::interleave_bits(span<uint8_t> out, span<const uint8_t> in) const
{
  unsigned E = out.size();

  for (unsigned out_index = 0; out_index != E; ++out_index) {
    unsigned help_index = out_index % modulation_order;
    unsigned in_index   = (out_index + help_index * (E - 1)) / modulation_order;
    out[out_index]      = in[in_index];
  }
}

std::unique_ptr<srsgnb::ldpc_rate_matcher> srsgnb::create_ldpc_rate_matcher()
{
  return std::make_unique<ldpc_rate_matcher_impl>();
}
