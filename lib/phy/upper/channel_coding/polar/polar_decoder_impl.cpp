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

#include "polar_decoder_impl.h"
#include "srsran/srsvec/binary.h"
#include "srsran/srsvec/zero.h"

using namespace srsran;

/// \brief Polar decoder function \f$f\f$.
///
/// Vectorial form of the soft xor operation.
static void
vec_function_f(span<log_likelihood_ratio> z, span<const log_likelihood_ratio> x, span<const log_likelihood_ratio> y)
{
  srsran_assert(y.size() == x.size(), "Input spans must have the same size.");
  srsran_assert(z.size() == x.size(), "Input and output spans must have the same size.");

  std::transform(x.begin(), x.end(), y.begin(), z.begin(), log_likelihood_ratio::soft_xor);
}

/// Combines two log-likelihood ratio: constructively (sum) if b = 0, destructively (difference) if b = 1.
static log_likelihood_ratio switch_combine(log_likelihood_ratio x, log_likelihood_ratio y, uint8_t b)
{
  return ((b == 0) ? (x + y) : (x - y));
}

/// \brief Polar decoder function \f$g\f$.
///
/// Vectorial form of switch_combine.
static void vec_function_g(span<log_likelihood_ratio>       z,
                           span<const log_likelihood_ratio> x,
                           span<const log_likelihood_ratio> y,
                           span<const uint8_t>              b)
{
  srsran_assert((y.size() == x.size()) && (b.size() == x.size()), "Input spans must have the same size.");
  srsran_assert(z.size() == x.size(), "Input and output spans must have the same size.");

  for (unsigned i = 0, len = x.size(); i != len; ++i) {
    z[i] = switch_combine(y[i], x[i], b[i]);
  }
}

/// Vectorial form of soft-to-hard bit conversion.
static void vec_hard_bit(span<log_likelihood_ratio> x, span<uint8_t> z)
{
  assert(x.size() == z.size());

  std::transform(x.begin(), x.end(), z.begin(), [](log_likelihood_ratio a) { return a.to_hard_bit(); });
}

polar_decoder_impl::tmp_node_s::tmp_node_s(uint8_t nMax)
{
  unsigned max_code_size      = (1U << nMax);
  unsigned nMax_1             = nMax - 1;
  unsigned max_code_half_size = (1U << nMax_1);

  is_not_rate_0.resize(2UL * max_code_size);

  is_rate_1 = span<uint8_t>(&is_not_rate_0[max_code_size], max_code_size);

  i_odd.resize(max_code_half_size);
  i_even.resize(max_code_half_size);
}

void polar_decoder_impl::tmp_node_s::compute(std::vector<node_rate*>& node_type,
                                             span<const uint16_t>     frozen_set,
                                             const uint16_t           code_size_log)
{
  // Stage index.
  uint8_t s = 0;

  uint16_t code_size       = (1U << code_size_log);
  uint8_t  code_size_log_1 = code_size_log - 1;
  uint16_t code_half_size  = (1U << code_size_log_1);

  srsvec::zero(i_even.first(code_half_size));
  srsvec::zero(i_odd.first(code_half_size));
  for (uint16_t i = 0; i != code_half_size; ++i) {
    i_even[i] = 2 * i;
    i_odd[i]  = 2 * i + 1;
  }

  // node_type = is_not_rate_0_node: 0 if rate 0, 1 if not rate 0.
  std::fill_n(is_not_rate_0.begin(), code_size, 1);
  std::fill_n(is_rate_1.begin(), code_size, 1);
  for (const uint16_t fs : frozen_set) {
    is_not_rate_0[fs] = 0;
    is_rate_1[fs]     = 0;
  }

  s = 0;
  for (uint16_t j = 0; j != code_size; ++j) {
    // Set to: 0 if rate-0; 2 if rate-r; 3 if rate 1.
    node_type[s][j] = static_cast<node_rate>(3 * is_not_rate_0[j]);
  }
  for (s = 1; s != (uint8_t)(code_size_log + 1); ++s) {
    uint16_t code_size_log_s = code_size_log - s;
    uint16_t code_stage_size = (1U << code_size_log_s);
    for (uint16_t j = 0; j != code_stage_size; ++j) {
      is_not_rate_0[j] = is_not_rate_0[i_even[j]] | is_not_rate_0[i_odd[j]]; // bitor
      is_rate_1[j]     = is_rate_1[i_even[j]] & is_rate_1[i_odd[j]];         // bitand
      // Set to: 0 if rate-0; 2 if rate-r; 3 if rate 1
      node_type[s][j] = static_cast<node_rate>(2 * is_not_rate_0[j] + is_rate_1[j]);
    }
  }
}

polar_decoder_impl::polar_decoder_impl(std::unique_ptr<polar_encoder> enc_, uint8_t nMax) :
  tmp_node_type(nMax), enc(std::move(enc_))
{
  param.code_stage_size.resize(nMax + 1);

  param.code_stage_size[0] = 1;
  for (uint8_t i = 1; i != nMax + 1; ++i) {
    param.code_stage_size[i] = 2 * param.code_stage_size[i - 1];
  }

  state.stage         = 0;
  state.flag_finished = false;
  state.active_node_per_stage.resize(nMax + 1);

  // Allocates memory for estimated bits per stage.
  uint16_t est_bits_size = param.code_stage_size[nMax];

  est_bit.resize(est_bits_size);

  // Allocate memory for LLR pointers.
  llr0.resize(nMax + 1);
  llr1.resize(nMax + 1);

  // For each n, n = 0 to n = nMax, we need an LLR buffer of size 2^n. Thus,
  // the total memory needed is 2^(nMax+1)-1.
  uint8_t  n_llr_all_stages = nMax + 1;
  uint16_t llr_all_stages   = (1U << n_llr_all_stages) - 1;

  // Allocate memory.
  llr_alloc.resize(llr_all_stages);

  // Assign a valid view for the first stage llr0 and an invalid view for llr1.
  llr0[0] = llr_alloc.first(1);
  llr1[0] = span<log_likelihood_ratio>();

  // Assign a valid view for the rest of stages llr0 and llr1.
  for (uint8_t s = 1; s != n_llr_all_stages; ++s) {
    llr0[s] = llr_alloc.subspan(param.code_stage_size[s] - 1, param.code_stage_size[s]);
    llr1[s] = llr0[s].last(param.code_stage_size[s - 1]);
  }

  param.node_type_alloc.resize(llr_all_stages);
  param.node_type.resize(nMax + 1);
  param.node_type[0] = param.node_type_alloc.data();

  // Initialize all node type pointers (stage 0 is the first, opposite to LLRs).
  for (uint8_t s = 1; s != nMax + 1; ++s) {
    param.node_type[s] = param.node_type[s - 1] + param.code_stage_size[nMax - s + 1];
  }
}

void polar_decoder_impl::init(span<uint8_t>                    data_decoded,
                              span<const log_likelihood_ratio> input_llr,
                              const uint8_t                    code_size_log,
                              span<const uint16_t>             frozen_set)
{
  param.code_size_log     = code_size_log;
  uint16_t code_size      = param.code_stage_size[code_size_log];
  uint16_t code_half_size = param.code_stage_size[code_size_log - 1];

  // Initializes the data_decoded_vector to all zeros.
  srsvec::zero(data_decoded.first(code_size));

  // Initialize est_bit vector to all zeros.
  srsvec::zero(est_bit.first(code_size));

  // Initializes LLR buffer for the last stage/level with the input LLRs values.
  for (uint16_t i = 0; i != code_half_size; ++i) {
    llr0[code_size_log][i] = input_llr[i];
    llr1[code_size_log][i] = input_llr[i + code_half_size];
  }

  // Initializes the state of the decoding tree: start from the only one node at the last stage + 1.
  state.stage = code_size_log + 1;
  srsvec::zero(state.active_node_per_stage.first(code_size_log + 1));
  state.flag_finished = false;

  // Computes the node types for the decoding tree.
  tmp_node_type.compute(param.node_type, frozen_set, code_size_log);
}

void polar_decoder_impl::rate_0_node()
{
  // Binary log of the code size.
  uint8_t  code_size_log = param.code_size_log;
  uint16_t code_size     = param.code_stage_size[code_size_log];
  uint16_t bit_pos       = state.active_node_per_stage[0];
  uint8_t  stage         = state.stage;

  if (bit_pos == code_size - 1) {
    state.flag_finished = true;
  } else {
    // Update active node at all the stages.
    for (uint8_t i = 0; i <= stage; ++i) {
      state.active_node_per_stage[i] += param.code_stage_size[stage - i];
    }
  }
}

void polar_decoder_impl::rate_1_node(span<uint8_t> message)
{
  // For the SSC decoder, rate 1 nodes are always at stage 0.
  uint8_t stage = state.stage;

  uint16_t bit_pos         = state.active_node_per_stage[0];
  uint16_t code_size       = param.code_stage_size[param.code_size_log];
  uint16_t code_stage_size = param.code_stage_size[stage];

  span<uint8_t> codeword = est_bit.subspan(bit_pos, code_stage_size);
  srsran_assert(llr0[stage].size() == code_stage_size, "Invalid size ({} != {})", llr0[stage].size(), code_stage_size);

  vec_hard_bit(llr0[stage], codeword);

  if (stage != 0) {
    span<uint8_t> message_stage = message.subspan(bit_pos, code_stage_size);
    enc->encode(message_stage, codeword, stage);
  } else {
    message[bit_pos] = codeword[0];
  }

  // Update active node at all the stages.
  for (uint8_t i = 0; i <= stage; ++i) {
    state.active_node_per_stage[i] += param.code_stage_size[stage - i];
  }

  // Check if this is the last bit.
  if (state.active_node_per_stage[0] == code_size) {
    state.flag_finished = true;
  }
}

void polar_decoder_impl::rate_r_node(span<uint8_t> message)
{
  uint8_t* estbits0        = nullptr;
  uint8_t* estbits1        = nullptr;
  uint16_t bit_pos         = 0;
  int16_t  offset0         = 0;
  int16_t  offset1         = 0;
  uint8_t  stage           = state.stage;
  uint16_t stage_size      = param.code_stage_size[stage];
  uint16_t stage_half_size = param.code_stage_size[stage - 1];

  vec_function_f(
      llr0[stage - 1].first(stage_half_size), llr0[stage].first(stage_half_size), llr1[stage].first(stage_half_size));

  // Move to the child node to the left (up) of the tree.
  simplified_node(message);
  if (state.flag_finished) {
    // Just in case: for 5G frozen sets, the code should never end here.
    return;
  }

  bit_pos  = state.active_node_per_stage[0];
  offset0  = bit_pos - stage_half_size;
  estbits0 = est_bit.data() + offset0;

  vec_function_g(llr0[stage - 1].first(stage_half_size),
                 llr0[stage].first(stage_half_size),
                 llr1[stage].first(stage_half_size),
                 {estbits0, stage_half_size});

  // Move to the child node to the right (down) of the tree.
  simplified_node(message);
  if (state.flag_finished) {
    return;
  }

  bit_pos = state.active_node_per_stage[0];

  offset0  = bit_pos - stage_size;
  offset1  = offset0 + stage_half_size;
  estbits0 = est_bit.data() + offset0;
  estbits1 = est_bit.data() + offset1;

  srsvec::binary_xor(span<uint8_t>(estbits0, stage_half_size),
                     span<uint8_t>(estbits1, stage_half_size),
                     span<uint8_t>(estbits0, stage_half_size));

  // Update this node index and return to the father node.
  ++state.active_node_per_stage[stage];
}

void polar_decoder_impl::simplified_node(span<uint8_t> message)
{
  // Move to the child node.
  --state.stage;

  uint8_t  stage   = state.stage;
  uint16_t bit_pos = state.active_node_per_stage[stage];

  switch (param.node_type[stage][bit_pos]) {
    case RATE_1:
      rate_1_node(message);
      break;
    case RATE_0:
      rate_0_node();
      break;
    case RATE_R:
      rate_r_node(message);
      break;
    default:
      srsran_assertion_failure("ERROR: wrong node type {}.", param.node_type[stage][bit_pos]);
  }
  // Go back to the parent node.
  ++state.stage;
}

void polar_decoder_impl::decode(span<uint8_t>                    data_decoded,
                                span<const log_likelihood_ratio> input_llr,
                                const polar_code&                code)
{
  const bounded_bitset<polar_code::NMAX>& frozen_set_mask = code.get_F_set();

  std::array<uint16_t, polar_code::NMAX> temp_frozen_set;
  span<uint16_t>                         frozen_set = span<uint16_t>(temp_frozen_set).first(frozen_set_mask.count());

  frozen_set_mask.for_each(
      0, frozen_set_mask.size(), [&frozen_set, n = 0](uint16_t index) mutable { frozen_set[n++] = index; });

  init(data_decoded, input_llr, code.get_n(), frozen_set);

  simplified_node(data_decoded);
}
