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
/// \brief LDPC decoder - definition of the AVX2-optimized implementation.

#include "ldpc_decoder_avx2.h"
#include "ldpc_graph_impl.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

// AVX2 vectors filled with useful constants.
static const __m256i LLR_MAX_epi8          = _mm256_set1_epi8(LLR_MAX.to_value_type());
static const __m256i LLR_MIN_epi8          = _mm256_set1_epi8(LLR_MIN.to_value_type());
static const __m256i LLR_INFINITY_epi8     = _mm256_set1_epi8(LLR_INFINITY.to_value_type());
static const __m256i LLR_NEG_INFINITY_epi8 = _mm256_set1_epi8(-LLR_INFINITY.to_value_type());

void ldpc_decoder_avx2::select_strategy()
{
  // Each BG node contains LS bits, which are stored in node_size_avx2 AVX2 vectors.
  node_size_avx2 = divide_ceil(lifting_size, AVX2_SIZE_BYTE);

  if (bg_N_high_rate == 26) {
    compute_var_to_check_msgs_hr = select_var_to_check_strategy_hr_bg1<MAX_NODE_SIZE_AVX2>();
  } else {
    srsgnb_assert(bg_N_high_rate == 14,
                  "The number of nodes including the high-rate region should be 14 or 26, provided {}.",
                  bg_N_high_rate);
    compute_var_to_check_msgs_hr = select_var_to_check_strategy_hr_bg2<MAX_NODE_SIZE_AVX2>();
  }

  compute_var_to_check_msgs_ext = select_var_to_check_strategy_ext<MAX_NODE_SIZE_AVX2>();
  compute_soft_bits             = select_soft_bits_strategy<MAX_NODE_SIZE_AVX2>();
  compute_check_to_var_msgs     = select_check_to_var_strategy<MAX_NODE_SIZE_AVX2>();
  analyze_var_to_check_msgs     = select_analyze_var_to_check_strategy<MAX_NODE_SIZE_AVX2>();
}

void ldpc_decoder_avx2::load_soft_bits(span<const log_likelihood_ratio> llrs)
{
  // Erase registers.
  std::fill(is_check_to_var_initialised.begin(), is_check_to_var_initialised.end(), false);

  // Compute the number of data nodes occupied by the llrs.
  unsigned nof_useful_nodes = divide_ceil(llrs.size(), lifting_size) + 2;

  // Copy input llrs and organize them by nodes.
  const log_likelihood_ratio* llr_ptr = llrs.data();
  // Recall that the first 2 * lifting_size bits (2 nodes) are not transmitted.
  std::memset(soft_bits.data_at(0), 0, 2 * node_size_avx2 * AVX2_SIZE_BYTE);
  for (unsigned i_node = 2 * node_size_avx2, max_node = nof_useful_nodes * node_size_avx2; i_node != max_node;
       i_node += node_size_avx2) {
    std::memcpy(soft_bits.data_at(i_node), llr_ptr, lifting_size);
    llr_ptr += lifting_size;
  }

  // The length of llrs may not be an exact multiple of the lifting size.
  unsigned tail_positions = llrs.size() % lifting_size;
  if (tail_positions != 0) {
    std::memset(
        soft_bits.data_at((nof_useful_nodes - 1) * node_size_avx2, tail_positions), 0, lifting_size - tail_positions);
  }
}

void ldpc_decoder_avx2::update_variable_to_check_messages(unsigned check_node)
{
  unsigned bg_N_hr_lifted = bg_N_high_rate * node_size_avx2;
  // First, update the messages corresponding to the high-rate region. All layers contribute.
  if (is_check_to_var_initialised[check_node]) {
    compute_var_to_check_msgs_hr(mm256::avx2_span(var_to_check, 0, bg_N_hr_lifted),
                                 mm256::avx2_span(soft_bits, 0, bg_N_hr_lifted),
                                 mm256::avx2_span(check_to_var[check_node], 0, bg_N_hr_lifted));
  } else {
    // If check_to_var is uninitialized, it is equivalent to copy soft_bits to var_to_check.
    std::memcpy(var_to_check.data_at(0), soft_bits.data_at(0), bg_N_hr_lifted * AVX2_SIZE_BYTE);
  }

  // Next, update the messages corresponding to the extension region, if applicable.
  // From layer 4 onwards, each layer is connected to only one consecutive block of lifting_size bits.
  if (check_node >= 4) {
    if (is_check_to_var_initialised[check_node]) {
      compute_var_to_check_msgs_ext(
          mm256::avx2_span(var_to_check, bg_N_hr_lifted, node_size_avx2),
          mm256::avx2_span(soft_bits, bg_N_hr_lifted + (check_node - 4) * node_size_avx2, node_size_avx2),
          mm256::avx2_span(check_to_var[check_node], bg_N_hr_lifted, node_size_avx2));
    } else {
      // If check_to_var is uninitialized, it is equivalent to copy soft_bits to var_to_check.
      std::memcpy(var_to_check.data_at(bg_N_hr_lifted),
                  soft_bits.data_at(bg_N_hr_lifted + (check_node - 4) * node_size_avx2),
                  node_size_avx2 * AVX2_SIZE_BYTE);
    }
  }
}

template <unsigned NOF_NODES, unsigned NODE_SIZE_AVX2>
void ldpc_decoder_avx2::compute_var_to_check_msgs(mm256::avx2_span        v2c,
                                                  const mm256::avx2_span& soft,
                                                  const mm256::avx2_span& c2v)
{
  constexpr unsigned LENGTH = NOF_NODES * NODE_SIZE_AVX2;
  srsgnb_assert((soft.size() == LENGTH) && (c2v.size() == LENGTH) && (v2c.size() == LENGTH),
                "Number of soft bits, variable-to-check messages and check-to-variable messages must coincide.");

  // By definition, the difference between two LLRs saturates at +/- LLR_MAX. Moreover, if either term is infinite, so
  // is the result, with proper sign.
  for (unsigned i_block = 0; i_block != LENGTH; ++i_block) {
    __m256i soft_epi8 = soft.get_at(i_block);
    __m256i c2v_epi8  = c2v.get_at(i_block);

    // v2c = (soft - c2v > LLR_MAX) ? LLR_MAX : (soft - c2v)
    __m256i help_sub_epi8 = _mm256_subs_epi8(soft_epi8, c2v_epi8);
    __m256i v2c_epi8      = _mm256_min_epi8(help_sub_epi8, LLR_MAX_epi8);

    // v2c = (v2c < MIN_LLR) ? MIN_LLR : v2c
    v2c_epi8 = _mm256_max_epi8(v2c_epi8, LLR_MIN_epi8);

    // Ensure that soft = +/- infinity implies v2c = +/- infinity.
    // v2c = (soft < infinity) ? v2c : infinity
    __m256i mask_epi8 = _mm256_cmpgt_epi8(LLR_INFINITY_epi8, soft_epi8);
    v2c_epi8          = _mm256_blendv_epi8(LLR_INFINITY_epi8, v2c_epi8, mask_epi8);

    // v2c = (soft > - infinity) ? v2c : - infinity
    mask_epi8 = _mm256_cmpgt_epi8(soft_epi8, LLR_NEG_INFINITY_epi8);
    v2c_epi8  = _mm256_blendv_epi8(LLR_NEG_INFINITY_epi8, v2c_epi8, mask_epi8);
    v2c.set_at(i_block, v2c_epi8);
  }
}

void ldpc_decoder_avx2::update_check_to_variable_messages(unsigned check_node)
{
  // Buffer to store the minimum (in absolute value) variable-to-check message.
  mm256::avx2_array<MAX_NODE_SIZE_AVX2> min_var_to_check;
  // Buffer to store the second minimum (in absolute value) variable-to-check message for each base graph check node.
  mm256::avx2_array<MAX_NODE_SIZE_AVX2> second_min_var_to_check;
  // Buffer to store the index of the minimum-valued variable-to-check message.
  mm256::avx2_array<MAX_NODE_SIZE_AVX2> min_var_to_check_index;
  // Buffer to store the sign product of all variable-to-check messages.
  mm256::avx2_array<MAX_NODE_SIZE_AVX2> sign_prod_var_to_check;

  // Reset temporal buffers.
  unsigned node_size_byte = node_size_avx2 * AVX2_SIZE_BYTE;
  std::memset(min_var_to_check.data_at(0), LLR_MAX.to_value_type(), node_size_byte);
  std::memset(second_min_var_to_check.data_at(0), LLR_MAX.to_value_type(), node_size_byte);
  std::memset(min_var_to_check_index.data_at(0), 0, node_size_byte);
  std::memset(sign_prod_var_to_check.data_at(0), 0, node_size_byte);

  // Retrieve list of variable nodes connected to this check node.
  const BG_adjacency_row_t& current_var_indices = current_graph->get_adjacency_row(check_node);
  const auto*               last_var_index_itr  = current_var_indices.cend();
  // For all variable nodes connected to this check node.
  unsigned i_loop = 0;
  for (const auto* this_var_index_itr = current_var_indices.cbegin();
       (this_var_index_itr != last_var_index_itr) && (*this_var_index_itr != NO_EDGE);
       ++this_var_index_itr, ++i_loop) {
    // Rotate the variable node as specified by the base graph.
    unsigned shift          = current_graph->get_lifted_node(check_node, *this_var_index_itr);
    unsigned v2c_base_index = std::min(*this_var_index_itr, bg_N_high_rate);

    mm256::avx2_span this_rotated_node(rotated_var_to_check, i_loop * node_size_avx2, node_size_avx2);
    rotate_node_right(
        this_rotated_node.data_at(0, 0), var_to_check.data_at(v2c_base_index * node_size_avx2, 0), shift, lifting_size);

    analyze_var_to_check_msgs(min_var_to_check,
                              second_min_var_to_check,
                              min_var_to_check_index,
                              sign_prod_var_to_check,
                              this_rotated_node,
                              i_loop);
  }

  // For all variable nodes connected to this check node.
  i_loop = 0;
  for (const auto* this_var_index_itr = current_var_indices.cbegin();
       (this_var_index_itr != last_var_index_itr) && (*this_var_index_itr != NO_EDGE);
       ++this_var_index_itr, ++i_loop) {
    mm256::avx2_array<MAX_NODE_SIZE_AVX2> this_check_to_var;

    const mm256::avx2_span this_rotated_node(rotated_var_to_check, i_loop * node_size_avx2, node_size_avx2);

    compute_check_to_var_msgs(this_check_to_var,
                              min_var_to_check,
                              second_min_var_to_check,
                              min_var_to_check_index,
                              sign_prod_var_to_check,
                              this_rotated_node,
                              scaling_factor,
                              i_loop);

    // Rotate the message back before storing it.
    unsigned shift          = current_graph->get_lifted_node(check_node, *this_var_index_itr);
    unsigned c2v_base_index = std::min(*this_var_index_itr, bg_N_high_rate);
    rotate_node_left(check_to_var[check_node].data_at(c2v_base_index * node_size_avx2, 0),
                     this_check_to_var.data_at(0, 0),
                     shift,
                     lifting_size);
  }
  is_check_to_var_initialised[check_node] = true;
}

void ldpc_decoder_avx2::update_soft_bits(unsigned check_node)
{
  const BG_adjacency_row_t& current_var_indices = current_graph->get_adjacency_row(check_node);
  const auto*               this_var_index      = current_var_indices.cbegin();
  const auto*               last_var_index      = current_var_indices.cend();

  // For all variable nodes connected to this check node...
  for (; (this_var_index != last_var_index) && (*this_var_index != NO_EDGE); ++this_var_index) {
    // Retrieve the corresponding messages and soft bits.
    unsigned               node_index = std::min(*this_var_index, bg_N_high_rate) * node_size_avx2;
    const mm256::avx2_span this_check_to_var(check_to_var[check_node], node_index, node_size_avx2);
    const mm256::avx2_span this_var_to_check(var_to_check, node_index, node_size_avx2);
    mm256::avx2_span       this_soft_bits(soft_bits, *this_var_index * node_size_avx2, node_size_avx2);

    compute_soft_bits(this_soft_bits, this_check_to_var, this_var_to_check);
  }
}

void ldpc_decoder_avx2::get_hard_bits(span<uint8_t> out)
{
  for (unsigned i_node_lifted = 0, max_node_lifted = bg_K * node_size_avx2, i_out = 0; i_node_lifted != max_node_lifted;
       i_node_lifted += node_size_avx2, i_out += lifting_size) {
    span<const log_likelihood_ratio> current_soft(
        reinterpret_cast<log_likelihood_ratio*>(soft_bits.data_at(i_node_lifted, 0)), lifting_size);
    span<uint8_t> current_out = out.subspan(i_out, lifting_size);
    std::transform(current_soft.begin(), current_soft.end(), current_out.begin(), [](log_likelihood_ratio sb) {
      return sb.to_hard_bit();
    });
  }
}

template <unsigned NODE_SIZE_AVX2_PH>
ldpc_decoder_avx2::var_to_check_strategy ldpc_decoder_avx2::select_var_to_check_strategy_hr_bg1()
{
  if (node_size_avx2 != NODE_SIZE_AVX2_PH) {
    return select_var_to_check_strategy_hr_bg1<NODE_SIZE_AVX2_PH - 1>();
  }
  return compute_var_to_check_msgs<26, NODE_SIZE_AVX2_PH>;
}

template <>
ldpc_decoder_avx2::var_to_check_strategy ldpc_decoder_avx2::select_var_to_check_strategy_hr_bg1<1>()
{
  return compute_var_to_check_msgs<26, 1>;
}

template <unsigned NODE_SIZE_AVX2_PH>
ldpc_decoder_avx2::var_to_check_strategy ldpc_decoder_avx2::select_var_to_check_strategy_hr_bg2()
{
  if (node_size_avx2 != NODE_SIZE_AVX2_PH) {
    return select_var_to_check_strategy_hr_bg2<NODE_SIZE_AVX2_PH - 1>();
  }
  return compute_var_to_check_msgs<14, NODE_SIZE_AVX2_PH>;
}

template <>
ldpc_decoder_avx2::var_to_check_strategy ldpc_decoder_avx2::select_var_to_check_strategy_hr_bg2<1>()
{
  return compute_var_to_check_msgs<14, 1>;
}

template <unsigned NODE_SIZE_AVX2_PH>
ldpc_decoder_avx2::var_to_check_strategy ldpc_decoder_avx2::select_var_to_check_strategy_ext()
{
  if (node_size_avx2 != NODE_SIZE_AVX2_PH) {
    return select_var_to_check_strategy_ext<NODE_SIZE_AVX2_PH - 1>();
  }
  return compute_var_to_check_msgs<1, NODE_SIZE_AVX2_PH>;
}

template <>
ldpc_decoder_avx2::var_to_check_strategy ldpc_decoder_avx2::select_var_to_check_strategy_ext<1>()
{
  return compute_var_to_check_msgs<1, 1>;
}

template <unsigned NODE_SIZE_AVX2>
static void inner_update_soft_bits(mm256::avx2_span        this_soft_bits,
                                   const mm256::avx2_span& this_check_to_var,
                                   const mm256::avx2_span& this_var_to_check)
{
  for (unsigned i_block = 0; i_block != NODE_SIZE_AVX2; ++i_block) {
    // Add check-to-variable and variable-to-check messages.
    __m256i soft_epi8 = _mm256_adds_epi8(this_check_to_var.get_at(i_block), this_var_to_check.get_at(i_block));
    // If they are above LLR_MAX (resp., below LLR_MIN), promote to LLR_INFINITY (resp., -LLR_INFINITY).
    __m256i mask_epi8 = _mm256_cmpgt_epi8(soft_epi8, LLR_MAX_epi8);
    soft_epi8         = _mm256_blendv_epi8(soft_epi8, LLR_INFINITY_epi8, mask_epi8);
    mask_epi8         = _mm256_cmpgt_epi8(LLR_MIN_epi8, soft_epi8);
    this_soft_bits.set_at(i_block, _mm256_blendv_epi8(soft_epi8, LLR_NEG_INFINITY_epi8, mask_epi8));
  }
}

template <unsigned NODE_SIZE_AVX2_PH>
ldpc_decoder_avx2::soft_bit_strategy ldpc_decoder_avx2::select_soft_bits_strategy()
{
  if (node_size_avx2 != NODE_SIZE_AVX2_PH) {
    return select_soft_bits_strategy<NODE_SIZE_AVX2_PH - 1>();
  }
  return inner_update_soft_bits<NODE_SIZE_AVX2_PH>;
}

template <>
ldpc_decoder_avx2::soft_bit_strategy ldpc_decoder_avx2::select_soft_bits_strategy<1>()
{
  return inner_update_soft_bits<1>;
}

template <unsigned NODE_SIZE_AVX2>
static void inner_update_check_to_var_msgs(mm256::avx2_span        this_check_to_var,
                                           const mm256::avx2_span& min_var_to_check,
                                           const mm256::avx2_span& second_min_var_to_check,
                                           const mm256::avx2_span& min_var_to_check_index,
                                           const mm256::avx2_span& sign_prod_var_to_check,
                                           const mm256::avx2_span& this_rotated_node,
                                           float                   scaling_factor,
                                           unsigned                i_loop)
{
  __m256i this_var_index_epi8 = _mm256_set1_epi8(static_cast<int8_t>(i_loop));
  for (unsigned i_block = 0; i_block != NODE_SIZE_AVX2; ++i_block) {
    // The check-to-variable message is the minimum of all variable-to-check messages but the current one: that is, if
    // i_block corresponds to the minimum message, take the second-minimum.
    __m256i mask_is_min_epi8 = _mm256_cmpeq_epi8(this_var_index_epi8, min_var_to_check_index.get_at(i_block));
    __m256i check_to_var_epi8 =
        _mm256_blendv_epi8(min_var_to_check.get_at(i_block), second_min_var_to_check.get_at(i_block), mask_is_min_epi8);
    // Scale the message to compensate for approximations.
    check_to_var_epi8 = mm256::scale_epi8(check_to_var_epi8, scaling_factor);

    // Sign of the cumulative product of all variable-to-check messages but the current one (same as multiplying the
    // sign of all messages by the sign of the current one).
    __m256i final_sign_epi8 =
        _mm256_xor_si256(this_rotated_node.get_at(i_block), sign_prod_var_to_check.get_at(i_block));

    // Assign the sign to the message.
    __m256i help_epi8 = _mm256_sign_epi8(check_to_var_epi8, final_sign_epi8);
    this_check_to_var.set_at(i_block, _mm256_blendv_epi8(check_to_var_epi8, help_epi8, final_sign_epi8));
  }
}

template <unsigned NODE_SIZE_AVX2_PH>
ldpc_decoder_avx2::check_to_var_strategy ldpc_decoder_avx2::select_check_to_var_strategy()
{
  if (node_size_avx2 != NODE_SIZE_AVX2_PH) {
    return select_check_to_var_strategy<NODE_SIZE_AVX2_PH - 1>();
  }

  return inner_update_check_to_var_msgs<NODE_SIZE_AVX2_PH>;
}

template <>
ldpc_decoder_avx2::check_to_var_strategy ldpc_decoder_avx2::select_check_to_var_strategy<1>()
{
  return inner_update_check_to_var_msgs<1>;
}

template <unsigned NODE_SIZE_AVX2>
static void inner_analyze_var_to_check(mm256::avx2_span        min_var_to_check,
                                       mm256::avx2_span        second_min_var_to_check,
                                       mm256::avx2_span        min_var_to_check_index,
                                       mm256::avx2_span        sign_prod_var_to_check,
                                       const mm256::avx2_span& this_rotated_node,
                                       unsigned                i_loop)
{
  __m256i this_var_index_epi8 = _mm256_set1_epi8(static_cast<int8_t>(i_loop));
  // For all AVX2 registers inside a node...
  for (unsigned i_block = 0; i_block != NODE_SIZE_AVX2; ++i_block) {
    // Store the sign of the cumulative product of all variable nodes.
    __m256i sign_prod_epi8 = sign_prod_var_to_check.get_at(i_block);
    sign_prod_epi8         = _mm256_xor_si256(sign_prod_epi8, this_rotated_node.get_at(i_block));
    sign_prod_var_to_check.set_at(i_block, sign_prod_epi8);

    // Find the minimum variable-to-check message (in absolute value).
    __m256i abs_var_to_check_epi8 = _mm256_abs_epi8(this_rotated_node.get_at(i_block));
    __m256i current_min_epi8      = min_var_to_check.get_at(i_block);
    __m256i mask_min_epi8         = _mm256_cmpgt_epi8(current_min_epi8, abs_var_to_check_epi8);
    // The following quantity is needed to find the second-smallest message.
    __m256i help_min_epi8 = _mm256_blendv_epi8(abs_var_to_check_epi8, current_min_epi8, mask_min_epi8);
    current_min_epi8      = _mm256_blendv_epi8(current_min_epi8, abs_var_to_check_epi8, mask_min_epi8);
    min_var_to_check.set_at(i_block, current_min_epi8);

    // Store the index corresponding to the minimum.
    __m256i min_var_to_check_index_epi8 = min_var_to_check_index.get_at(i_block);
    min_var_to_check_index_epi8 = _mm256_blendv_epi8(min_var_to_check_index_epi8, this_var_index_epi8, mask_min_epi8);
    min_var_to_check_index.set_at(i_block, min_var_to_check_index_epi8);

    // Find the second-minimum value.
    __m256i current_second_min_epi8 = second_min_var_to_check.get_at(i_block);
    mask_min_epi8                   = _mm256_cmpgt_epi8(current_second_min_epi8, abs_var_to_check_epi8);
    current_second_min_epi8         = _mm256_blendv_epi8(current_second_min_epi8, help_min_epi8, mask_min_epi8);
    second_min_var_to_check.set_at(i_block, current_second_min_epi8);
  }
}

template <unsigned NODE_SIZE_AVX2_PH>
ldpc_decoder_avx2::analyze_var_to_check_strategy ldpc_decoder_avx2::select_analyze_var_to_check_strategy()
{
  if (node_size_avx2 != NODE_SIZE_AVX2_PH) {
    return select_analyze_var_to_check_strategy<NODE_SIZE_AVX2_PH - 1>();
  }

  return inner_analyze_var_to_check<NODE_SIZE_AVX2_PH>;
}

template <>
ldpc_decoder_avx2::analyze_var_to_check_strategy ldpc_decoder_avx2::select_analyze_var_to_check_strategy<1>()
{
  return inner_analyze_var_to_check<1>;
}
