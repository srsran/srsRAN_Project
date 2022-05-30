/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "polar_decoder_impl.h"
#include "srsgnb/srsvec/binary.h"
#include "srsgnb/srsvec/zero.h"

using namespace srsgnb;

void vec_function_f(log_likelihood_ratio* z, const log_likelihood_ratio* x, const log_likelihood_ratio* y, unsigned len)
{
  for (unsigned i = 0; i != len; ++i) {
    z[i] = log_likelihood_ratio::soft_xor(x[i], y[i]);
  }
}

// Combines two log-likelihood ratio: constructively (sum) if b = 0, destructively (difference) if b = 1.
log_likelihood_ratio switch_combine(log_likelihood_ratio x, log_likelihood_ratio y, uint8_t b)
{
  return ((b == 0) ? x + y : x - y);
}

void vec_function_g(log_likelihood_ratio*       z,
                    const log_likelihood_ratio* x,
                    const log_likelihood_ratio* y,
                    const uint8_t*              b,
                    const uint16_t              len)
{
  for (int i = 0; i != len; ++i) {
    z[i] = switch_combine(y[i], x[i], b[i]);
  }
}

static inline void vec_hard_bit(span<log_likelihood_ratio> x, span<uint8_t> z)
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
  uint8_t s = 0; // stage

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
  std::fill(is_not_rate_0.begin(), is_not_rate_0.begin() + code_size, 1);
  std::fill(is_rate_1.begin(), is_rate_1.begin() + code_size, 1);
  for (const uint16_t fs : frozen_set) {
    is_not_rate_0[fs] = 0;
    is_rate_1[fs]     = 0;
  }

  s = 0;
  for (uint16_t j = 0; j != code_size; ++j) {
    node_type[s][j] = static_cast<node_rate>(3 * is_not_rate_0[j]); //  0 if rate-0; 2 if rate-r; 3 if rate 1
  }
  for (s = 1; s != (uint8_t)(code_size_log + 1); ++s) {
    uint16_t code_size_log_s = code_size_log - s;
    uint16_t code_stage_size = (1U << code_size_log_s);
    for (uint16_t j = 0; j != code_stage_size; ++j) {
      is_not_rate_0[j] = is_not_rate_0[i_even[j]] | is_not_rate_0[i_odd[j]]; // bitor
      is_rate_1[j]     = is_rate_1[i_even[j]] & is_rate_1[i_odd[j]];         // bitand
      node_type[s][j] =
          static_cast<node_rate>(2 * is_not_rate_0[j] + is_rate_1[j]); //  0 if rate-0; 2 if rate-r; 3 if rate 1
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

  state.active_node_per_stage.resize(nMax + 1);

  // allocates memory for estimated bits per stage
  uint16_t est_bits_size = param.code_stage_size[nMax];

  est_bit.resize(est_bits_size);

  // allocate memory for LLR pointers.
  llr0.resize(nMax + 1);
  llr1.resize(nMax + 1);

  // There are LLR buffers for n = 0 to n = code_size_log. Each with size 2^n. Thus,
  // the total memory needed is 2^(n+1)-1.
  // Only the stages starting at multiples of SRSRAN_AVX2_B_SIZE are aligned.

  // Let n_simd_llr be the exponent of the SIMD size in nummer of LLRs.
  // i.e. in a SIMD instruction we can load 2^(n_simd_llr) LLR values
  // then the memory for stages s >= n_simd_llr - 1 is aligned.
  // but only the operations at stages s > n_simd_llr have all the inputs aligned.
  uint8_t  n_llr_all_stages = nMax + 1; // there are 2^(n_llr_all_stages) - 1 LLR values summing up all stages.
  uint16_t llr_all_stages   = 1U << n_llr_all_stages;

  llr_alloc.resize(llr_all_stages);

  llr0[0] = llr_alloc.data();

  // initialize all LLR pointers
  llr1[0] = llr0[0] + 1;
  for (uint8_t s = 1; s != nMax + 1; ++s) {
    llr0[s] = llr0[0] + param.code_stage_size[s];
    llr1[s] = llr0[0] + param.code_stage_size[s] + param.code_stage_size[s - 1];
  }

  param.node_type_alloc.resize(llr_all_stages);
  param.node_type.resize(nMax + 1);
  param.node_type[0] = param.node_type_alloc.data();

  // initialize all node type pointers. (stage 0 is the first, opposite to LLRs)
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

  // Initializes the data_decoded_vector to all zeros
  srsvec::zero(data_decoded.first(code_size));

  // Initialize est_bit vector to all zeros
  srsvec::zero(est_bit.first(code_size));

  // Initializes LLR buffer for the last stage/level with the input LLRs values
  for (uint16_t i = 0; i != code_half_size; ++i) {
    llr0[code_size_log][i] = input_llr[i];
    llr1[code_size_log][i] = input_llr[i + code_half_size];
  }

  // Initializes the state of the decoding tree
  state.stage = code_size_log + 1; // start from the only one node at the last stage + 1.
  srsvec::zero(state.active_node_per_stage.first(code_size_log + 1));
  state.flag_finished = false;

  // computes the node types for the decoding tree
  tmp_node_type.compute(param.node_type, frozen_set, code_size_log);
}

void polar_decoder_impl::rate_0_node()
{
  uint8_t  code_size_log = param.code_size_log; // code_size_log.
  uint16_t code_size     = param.code_stage_size[code_size_log];
  uint16_t bit_pos       = state.active_node_per_stage[0];
  uint8_t  stage         = state.stage;

  if (bit_pos == code_size - 1) {
    state.flag_finished = true;
  } else {
    // update active node at all the stages
    for (uint8_t i = 0; i <= stage; ++i) {
      state.active_node_per_stage[i] += param.code_stage_size[stage - i];
    }
  }
}

void polar_decoder_impl::rate_1_node(span<uint8_t> message)
{
  uint8_t stage = state.stage; // for SSC decoder rate 1 nodes are always at stage 0.

  uint16_t bit_pos         = state.active_node_per_stage[0];
  uint16_t code_size       = param.code_stage_size[param.code_size_log];
  uint16_t code_stage_size = param.code_stage_size[stage];

  span<uint8_t>              codeword = est_bit.subspan(bit_pos, code_stage_size);
  span<log_likelihood_ratio> LLR      = span<log_likelihood_ratio>(llr0[stage], code_stage_size);

  vec_hard_bit(LLR, codeword);

  if (stage != 0) {
    span<uint8_t> message_stage = message.subspan(bit_pos, code_stage_size);
    enc->encode(message_stage, codeword, stage);
  } else {
    message[bit_pos] = codeword[0];
  }

  // update active node at all the stages
  for (uint8_t i = 0; i <= stage; ++i) {
    state.active_node_per_stage[i] += param.code_stage_size[stage - i];
  }

  // check if this is the last bit
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

  vec_function_f(llr0[stage - 1], llr0[stage], llr1[stage], stage_half_size);

  // move to the child node to the left (up) of the tree.
  simplified_node(message);
  if (state.flag_finished) { // (just in case). However for 5G frozen sets, the code can never end here.
    return;
  }

  bit_pos  = state.active_node_per_stage[0];
  offset0  = bit_pos - stage_half_size;
  estbits0 = est_bit.data() + offset0;

  vec_function_g(llr0[stage - 1], llr0[stage], llr1[stage], estbits0, stage_half_size);

  // move to the child node to the right (down) of the tree.
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

  // update this node index
  ++state.active_node_per_stage[stage]; // return to the father node
}

void polar_decoder_impl::simplified_node(span<uint8_t> message)
{
  --state.stage; // to child node.

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

  ++state.stage; // to parent node.
}

void polar_decoder_impl::decode(span<uint8_t>                    data_decoded,
                                span<const log_likelihood_ratio> input_llr,
                                const polar_code&                code)
{
  span<const uint16_t> frozen_set = code.get_F_set();

  init(data_decoded, input_llr, code.get_n(), frozen_set);

  simplified_node(data_decoded);
}

std::unique_ptr<polar_decoder> srsgnb::create_polar_decoder_ssc(std::unique_ptr<polar_encoder> enc,
                                                                unsigned                       code_size_log)
{
  return std::make_unique<polar_decoder_impl>(std::move(enc), code_size_log);
}
