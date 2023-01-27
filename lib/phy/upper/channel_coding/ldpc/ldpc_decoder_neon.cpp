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
/// \brief LDPC decoder - definition of the NEON-optimized implementation.

#include "ldpc_decoder_neon.h"
#include "ldpc_graph_impl.h"
#include "srsgnb/srsvec/copy.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

// NEON vectors filled with useful constants.
static const int8x16_t LLR_MAX_s8          = vdupq_n_s8(LLR_MAX.to_value_type());
static const int8x16_t LLR_MIN_s8          = vdupq_n_s8(LLR_MIN.to_value_type());
static const int8x16_t LLR_INFINITY_s8     = vdupq_n_s8(LLR_INFINITY.to_value_type());
static const int8x16_t LLR_NEG_INFINITY_s8 = vdupq_n_s8(-LLR_INFINITY.to_value_type());

void ldpc_decoder_neon::select_strategy()
{
  // Each BG node contains LS bits, which are stored in node_size_neon NEON vectors.
  node_size_neon = divide_ceil(lifting_size, NEON_SIZE_BYTE);

  if (bg_N_high_rate == 26) {
    compute_var_to_check_msgs_hr = select_var_to_check_strategy_hr_bg1<MAX_NODE_SIZE_NEON>();
  } else {
    srsgnb_assert(bg_N_high_rate == 14,
                  "The number of nodes including the high-rate region should be 14 or 26, provided {}.",
                  bg_N_high_rate);
    compute_var_to_check_msgs_hr = select_var_to_check_strategy_hr_bg2<MAX_NODE_SIZE_NEON>();
  }

  compute_var_to_check_msgs_ext = select_var_to_check_strategy_ext<MAX_NODE_SIZE_NEON>();
  compute_soft_bits             = select_soft_bits_strategy<MAX_NODE_SIZE_NEON>();
  compute_check_to_var_msgs     = select_check_to_var_strategy<MAX_NODE_SIZE_NEON>();
  analyze_var_to_check_msgs     = select_analyze_var_to_check_strategy<MAX_NODE_SIZE_NEON>();
}

void ldpc_decoder_neon::load_soft_bits(span<const log_likelihood_ratio> llrs)
{
  // Erase registers.
  std::fill(is_check_to_var_initialised.begin(), is_check_to_var_initialised.end(), false);

  // Compute the number of data nodes occupied by the llrs.
  unsigned nof_useful_nodes = divide_ceil(llrs.size(), lifting_size) + 2;

  // Copy input llrs and organize them by nodes.
  const log_likelihood_ratio* llr_ptr = llrs.data();
  // Recall that the first 2 * lifting_size bits (2 nodes) are not transmitted.
  std::memset(soft_bits.data_at(0), 0, 2 * node_size_neon * NEON_SIZE_BYTE);
  for (unsigned i_node = 2 * node_size_neon, max_node = nof_useful_nodes * node_size_neon; i_node != max_node;
       i_node += node_size_neon) {
    std::memcpy(soft_bits.data_at(i_node), llr_ptr, lifting_size);
    llr_ptr += lifting_size;
  }

  // The length of llrs may not be an exact multiple of the lifting size.
  unsigned tail_positions = llrs.size() % lifting_size;
  if (tail_positions != 0) {
    std::memset(
        soft_bits.data_at((nof_useful_nodes - 1) * node_size_neon, tail_positions), 0, lifting_size - tail_positions);
  }
}

void ldpc_decoder_neon::update_variable_to_check_messages(unsigned check_node)
{
  unsigned bg_N_hr_lifted = bg_N_high_rate * node_size_neon;
  // First, update the messages corresponding to the high-rate region. All layers contribute.
  if (is_check_to_var_initialised[check_node]) {
    compute_var_to_check_msgs_hr(neon::neon_span(var_to_check, 0, bg_N_hr_lifted),
                                 neon::neon_span(soft_bits, 0, bg_N_hr_lifted),
                                 neon::neon_span(check_to_var[check_node], 0, bg_N_hr_lifted));
  } else {
    // If check_to_var is uninitialized, it is equivalent to copy soft_bits to var_to_check.
    std::memcpy(var_to_check.data_at(0), soft_bits.data_at(0), bg_N_hr_lifted * NEON_SIZE_BYTE);
  }

  // Next, update the messages corresponding to the extension region, if applicable.
  // From layer 4 onwards, each layer is connected to only one consecutive block of lifting_size bits.
  if (check_node >= 4) {
    if (is_check_to_var_initialised[check_node]) {
      compute_var_to_check_msgs_ext(
          neon::neon_span(var_to_check, bg_N_hr_lifted, node_size_neon),
          neon::neon_span(soft_bits, bg_N_hr_lifted + (check_node - 4) * node_size_neon, node_size_neon),
          neon::neon_span(check_to_var[check_node], bg_N_hr_lifted, node_size_neon));
    } else {
      // If check_to_var is uninitialized, it is equivalent to copy soft_bits to var_to_check.
      std::memcpy(var_to_check.data_at(bg_N_hr_lifted),
                  soft_bits.data_at(bg_N_hr_lifted + (check_node - 4) * node_size_neon),
                  node_size_neon * NEON_SIZE_BYTE);
    }
  }
}

template <unsigned NOF_NODES, unsigned NODE_SIZE_NEON>
void ldpc_decoder_neon::compute_var_to_check_msgs(neon::neon_span        v2c,
                                                  const neon::neon_span& soft,
                                                  const neon::neon_span& c2v)
{
  constexpr unsigned LENGTH = NOF_NODES * NODE_SIZE_NEON;
  srsgnb_assert((soft.size() == LENGTH) && (c2v.size() == LENGTH) && (v2c.size() == LENGTH),
                "Number of soft bits, variable-to-check messages and check-to-variable messages must coincide.");

  // By definition, the difference between two LLRs saturates at +/- LLR_MAX. Moreover, if either term is infinite, so
  // is the result, with proper sign.
  for (unsigned i_block = 0; i_block != LENGTH; ++i_block) {
    int8x16_t soft_s8 = soft.get_at(i_block);
    int8x16_t c2v_s8  = c2v.get_at(i_block);

    // v2c = (soft - c2v > LLR_MAX) ? LLR_MAX : (soft - c2v)
    int8x16_t help_sub_s8 = vsubq_s8(soft_s8, c2v_s8);
    int8x16_t v2c_s8      = vminq_s8(help_sub_s8, LLR_MAX_s8);

    // v2c = (v2c < MIN_LLR) ? MIN_LLR : v2c
    v2c_s8 = vmaxq_s8(v2c_s8, LLR_MIN_s8);

    // Ensure that soft = +/- infinity implies v2c = +/- infinity.
    // v2c = (soft < infinity) ? v2c : infinity
    uint8x16_t mask_u8 = vcgtq_s8(LLR_INFINITY_s8, soft_s8);
    v2c_s8             = vbslq_s8(mask_u8, v2c_s8, LLR_INFINITY_s8);

    // v2c = (soft > - infinity) ? v2c : - infinity
    mask_u8 = vcgtq_s8(soft_s8, LLR_NEG_INFINITY_s8);
    v2c_s8  = vbslq_s8(mask_u8, v2c_s8, LLR_NEG_INFINITY_s8);
    v2c.set_at(i_block, v2c_s8);
  }
}

void ldpc_decoder_neon::update_check_to_variable_messages(unsigned check_node)
{
  // Buffer to store the minimum (in absolute value) variable-to-check message.
  neon::neon_array<MAX_NODE_SIZE_NEON> min_var_to_check;
  // Buffer to store the second minimum (in absolute value) variable-to-check message for each base graph check node.
  neon::neon_array<MAX_NODE_SIZE_NEON> second_min_var_to_check;
  // Buffer to store the index of the minimum-valued variable-to-check message.
  neon::neon_array<MAX_NODE_SIZE_NEON> min_var_to_check_index;
  // Buffer to store the sign product of all variable-to-check messages.
  neon::neon_array<MAX_NODE_SIZE_NEON> sign_prod_var_to_check;

  // Reset temporal buffers.
  unsigned node_size_byte = node_size_neon * NEON_SIZE_BYTE;
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

    neon::neon_span this_rotated_node(rotated_var_to_check, i_loop * node_size_neon, node_size_neon);
    rotate_node_right(
        this_rotated_node.data_at(0, 0), var_to_check.data_at(v2c_base_index * node_size_neon, 0), shift, lifting_size);

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
    neon::neon_array<MAX_NODE_SIZE_NEON> this_check_to_var;

    const neon::neon_span this_rotated_node(rotated_var_to_check, i_loop * node_size_neon, node_size_neon);

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
    rotate_node_left(check_to_var[check_node].data_at(c2v_base_index * node_size_neon, 0),
                     this_check_to_var.data_at(0, 0),
                     shift,
                     lifting_size);
  }
  is_check_to_var_initialised[check_node] = true;
}

void ldpc_decoder_neon::update_soft_bits(unsigned check_node)
{
  const BG_adjacency_row_t& current_var_indices = current_graph->get_adjacency_row(check_node);
  const auto*               this_var_index      = current_var_indices.cbegin();
  const auto*               last_var_index      = current_var_indices.cend();

  // For all variable nodes connected to this check node...
  for (; (this_var_index != last_var_index) && (*this_var_index != NO_EDGE); ++this_var_index) {
    // Retrieve the corresponding messages and soft bits.
    unsigned              node_index = std::min(*this_var_index, bg_N_high_rate) * node_size_neon;
    const neon::neon_span this_check_to_var(check_to_var[check_node], node_index, node_size_neon);
    const neon::neon_span this_var_to_check(var_to_check, node_index, node_size_neon);
    neon::neon_span       this_soft_bits(soft_bits, *this_var_index * node_size_neon, node_size_neon);

    compute_soft_bits(this_soft_bits, this_check_to_var, this_var_to_check);
  }
}

void ldpc_decoder_neon::get_hard_bits(bit_buffer& out)
{
  // Buffer to hold the soft bits.
  std::array<log_likelihood_ratio, MAX_LIFTING_SIZE * MAX_BG_K> temp_llr;
  span<log_likelihood_ratio>                                    llr_write_buffer(temp_llr);

  // Copy the LLRs from the soft_bits AVX array into temp_llr without any padding. Recall that temp_llr is aligned to
  // the size of the AVX registers. This makes it possible to call the hard decision function only once, improving
  // efficiency.
  for (unsigned i_node_lifted = 0, max_node_lifted = bg_K * node_size_neon; i_node_lifted != max_node_lifted;
       i_node_lifted += node_size_neon) {
    // View over the LLR.
    span<const log_likelihood_ratio> current_soft(
        reinterpret_cast<log_likelihood_ratio*>(soft_bits.data_at(i_node_lifted, 0)), lifting_size);

    // Append LLRs to the buffer.
    srsvec::copy(llr_write_buffer.first(lifting_size), current_soft);

    // Advance buffer.
    llr_write_buffer = llr_write_buffer.last(llr_write_buffer.size() - lifting_size);
  }

  // View over the appended LLR.
  span<const log_likelihood_ratio> llr_read_buffer(temp_llr.begin(), llr_write_buffer.begin());

  // Convert to hard bits.
  srsgnb::hard_decision(out, llr_read_buffer);
}

template <unsigned NODE_SIZE_NEON_PH>
ldpc_decoder_neon::var_to_check_strategy ldpc_decoder_neon::select_var_to_check_strategy_hr_bg1()
{
  if (node_size_neon != NODE_SIZE_NEON_PH) {
    return select_var_to_check_strategy_hr_bg1<NODE_SIZE_NEON_PH - 1>();
  }
  return compute_var_to_check_msgs<26, NODE_SIZE_NEON_PH>;
}

template <>
ldpc_decoder_neon::var_to_check_strategy ldpc_decoder_neon::select_var_to_check_strategy_hr_bg1<1>()
{
  return compute_var_to_check_msgs<26, 1>;
}

template <unsigned NODE_SIZE_NEON_PH>
ldpc_decoder_neon::var_to_check_strategy ldpc_decoder_neon::select_var_to_check_strategy_hr_bg2()
{
  if (node_size_neon != NODE_SIZE_NEON_PH) {
    return select_var_to_check_strategy_hr_bg2<NODE_SIZE_NEON_PH - 1>();
  }
  return compute_var_to_check_msgs<14, NODE_SIZE_NEON_PH>;
}

template <>
ldpc_decoder_neon::var_to_check_strategy ldpc_decoder_neon::select_var_to_check_strategy_hr_bg2<1>()
{
  return compute_var_to_check_msgs<14, 1>;
}

template <unsigned NODE_SIZE_NEON_PH>
ldpc_decoder_neon::var_to_check_strategy ldpc_decoder_neon::select_var_to_check_strategy_ext()
{
  if (node_size_neon != NODE_SIZE_NEON_PH) {
    return select_var_to_check_strategy_ext<NODE_SIZE_NEON_PH - 1>();
  }
  return compute_var_to_check_msgs<1, NODE_SIZE_NEON_PH>;
}

template <>
ldpc_decoder_neon::var_to_check_strategy ldpc_decoder_neon::select_var_to_check_strategy_ext<1>()
{
  return compute_var_to_check_msgs<1, 1>;
}

template <unsigned NODE_SIZE_NEON>
static void inner_update_soft_bits(neon::neon_span        this_soft_bits,
                                   const neon::neon_span& this_check_to_var,
                                   const neon::neon_span& this_var_to_check)
{
  for (unsigned i_block = 0; i_block != NODE_SIZE_NEON; ++i_block) {
    // Get a mask of the infinite check-to-var messages.
    int8x16_t  check_to_var_s8              = this_check_to_var.get_at(i_block);
    uint8x16_t is_check_plus_infty_mask_u8  = vcgtq_s8(check_to_var_s8, LLR_MAX_s8);
    uint8x16_t is_check_minus_infty_mask_u8 = vcgtq_s8(LLR_MIN_s8, check_to_var_s8);
    // Get a mask of the infinite var-to_check messages.
    int8x16_t  var_to_check_s8            = this_var_to_check.get_at(i_block);
    uint8x16_t is_var_plus_infty_mask_u8  = vcgtq_s8(var_to_check_s8, LLR_MAX_s8);
    uint8x16_t is_var_minus_infty_mask_u8 = vcgtq_s8(LLR_MIN_s8, var_to_check_s8);
    // Add check-to-variable and variable-to-check messages (note: we use saturated addition).
    int8x16_t soft_s8 = vqaddq_s8(check_to_var_s8, var_to_check_s8);

    // Soft bits are +INFINITY if they are larger than LLR_MAX or if at least one of the addends is (but not if the two
    // addends are both infinity but with opposite signs).
    uint8x16_t mask_u8        = vcgtq_s8(soft_s8, LLR_MAX_s8);
    uint8x16_t mask_inputs_u8 = vandq_u8(vmvnq_u8(is_var_minus_infty_mask_u8), is_check_plus_infty_mask_u8);
    uint8x16_t mask_tmp_u8    = vandq_u8(vmvnq_u8(is_check_minus_infty_mask_u8), is_var_plus_infty_mask_u8);
    mask_inputs_u8            = vorrq_u8(mask_inputs_u8, mask_tmp_u8);
    mask_u8                   = vorrq_u8(mask_u8, mask_inputs_u8);
    soft_s8                   = vbslq_s8(mask_u8, LLR_INFINITY_s8, soft_s8);

    // Soft bits are -INFINITY if they are smaller than LLR_MIN or if at least one of the addends is (but not if the two
    // addends are both infinity but with opposite signs).
    mask_u8        = vcgtq_s8(LLR_MIN_s8, soft_s8);
    mask_inputs_u8 = vandq_u8(vmvnq_u8(is_var_plus_infty_mask_u8), is_check_minus_infty_mask_u8);
    mask_tmp_u8    = vandq_u8(vmvnq_u8(is_check_plus_infty_mask_u8), is_var_minus_infty_mask_u8);
    mask_inputs_u8 = vorrq_u8(mask_inputs_u8, mask_tmp_u8);
    mask_u8        = vorrq_u8(mask_u8, mask_inputs_u8);
    this_soft_bits.set_at(i_block, vbslq_s8(mask_u8, LLR_NEG_INFINITY_s8, soft_s8));
  }
}

template <unsigned NODE_SIZE_NEON_PH>
ldpc_decoder_neon::soft_bit_strategy ldpc_decoder_neon::select_soft_bits_strategy()
{
  if (node_size_neon != NODE_SIZE_NEON_PH) {
    return select_soft_bits_strategy<NODE_SIZE_NEON_PH - 1>();
  }
  return inner_update_soft_bits<NODE_SIZE_NEON_PH>;
}

template <>
ldpc_decoder_neon::soft_bit_strategy ldpc_decoder_neon::select_soft_bits_strategy<1>()
{
  return inner_update_soft_bits<1>;
}

template <unsigned NODE_SIZE_NEON>
static void inner_update_check_to_var_msgs(neon::neon_span        this_check_to_var,
                                           const neon::neon_span& min_var_to_check,
                                           const neon::neon_span& second_min_var_to_check,
                                           const neon::neon_span& min_var_to_check_index,
                                           const neon::neon_span& sign_prod_var_to_check,
                                           const neon::neon_span& this_rotated_node,
                                           float                  scaling_factor,
                                           unsigned               i_loop)
{
  int8x16_t this_var_index_s8 = vdupq_n_s8(static_cast<int8_t>(i_loop));
  for (unsigned i_block = 0; i_block != NODE_SIZE_NEON; ++i_block) {
    // The check-to-variable message is the minimum of all variable-to-check messages but the current one: that is, if
    // i_block corresponds to the minimum message, take the second-minimum.
    uint8x16_t mask_is_min_u8 = vceqq_s8(this_var_index_s8, min_var_to_check_index.get_at(i_block));
    int8x16_t  check_to_var_s8 =
        vbslq_s8(mask_is_min_u8, second_min_var_to_check.get_at(i_block), min_var_to_check.get_at(i_block));
    // Scale the message to compensate for approximations.
    check_to_var_s8 = neon::scale_s8(check_to_var_s8, scaling_factor, log_likelihood_ratio::max().to_value_type());

    // Sign of the cumulative product of all variable-to-check messages but the current one (same as multiplying the
    // sign of all messages by the sign of the current one).
    int8x16_t  final_sign_s8 = veorq_s8(this_rotated_node.get_at(i_block), sign_prod_var_to_check.get_at(i_block));
    uint8x16_t sign_mask     = vreinterpretq_u8_s8(vshrq_n_s8(final_sign_s8, 7));

    // Assign the sign to the message.
    int8x16_t check_to_var_neg_s8 = vnegq_s8(check_to_var_s8);
    this_check_to_var.set_at(i_block, vbslq_s8(sign_mask, check_to_var_neg_s8, check_to_var_s8));
  }
}

template <unsigned NODE_SIZE_NEON_PH>
ldpc_decoder_neon::check_to_var_strategy ldpc_decoder_neon::select_check_to_var_strategy()
{
  if (node_size_neon != NODE_SIZE_NEON_PH) {
    return select_check_to_var_strategy<NODE_SIZE_NEON_PH - 1>();
  }

  return inner_update_check_to_var_msgs<NODE_SIZE_NEON_PH>;
}

template <>
ldpc_decoder_neon::check_to_var_strategy ldpc_decoder_neon::select_check_to_var_strategy<1>()
{
  return inner_update_check_to_var_msgs<1>;
}

template <unsigned NODE_SIZE_NEON>
static void inner_analyze_var_to_check(neon::neon_span        min_var_to_check,
                                       neon::neon_span        second_min_var_to_check,
                                       neon::neon_span        min_var_to_check_index,
                                       neon::neon_span        sign_prod_var_to_check,
                                       const neon::neon_span& this_rotated_node,
                                       unsigned               i_loop)
{
  int8x16_t this_var_index_s8 = vdupq_n_s8(static_cast<int8_t>(i_loop));
  // For all NEON registers inside a node...
  for (unsigned i_block = 0; i_block != NODE_SIZE_NEON; ++i_block) {
    // Store the sign of the cumulative product of all variable nodes.
    int8x16_t sign_prod_s8 = sign_prod_var_to_check.get_at(i_block);
    sign_prod_s8           = veorq_s8(sign_prod_s8, this_rotated_node.get_at(i_block));
    sign_prod_var_to_check.set_at(i_block, sign_prod_s8);

    // Find the minimum variable-to-check message (in absolute value).
    int8x16_t  abs_var_to_check_s8 = vabsq_s8(this_rotated_node.get_at(i_block));
    int8x16_t  current_min_s8      = min_var_to_check.get_at(i_block);
    uint8x16_t mask_min_u8         = vcgtq_s8(current_min_s8, abs_var_to_check_s8);
    // The following quantity is needed to find the second-smallest message.
    int8x16_t help_min_s8 = vbslq_s8(mask_min_u8, current_min_s8, abs_var_to_check_s8);
    current_min_s8        = vbslq_s8(mask_min_u8, abs_var_to_check_s8, current_min_s8);
    min_var_to_check.set_at(i_block, current_min_s8);

    // Store the index corresponding to the minimum.
    int8x16_t min_var_to_check_index_s8 = min_var_to_check_index.get_at(i_block);
    min_var_to_check_index_s8           = vbslq_s8(mask_min_u8, this_var_index_s8, min_var_to_check_index_s8);
    min_var_to_check_index.set_at(i_block, min_var_to_check_index_s8);

    // Find the second-minimum value.
    int8x16_t current_second_min_s8 = second_min_var_to_check.get_at(i_block);
    mask_min_u8                     = vcgtq_s8(current_second_min_s8, abs_var_to_check_s8);
    current_second_min_s8           = vbslq_s8(mask_min_u8, help_min_s8, current_second_min_s8);
    second_min_var_to_check.set_at(i_block, current_second_min_s8);
  }
}

template <unsigned NODE_SIZE_NEON_PH>
ldpc_decoder_neon::analyze_var_to_check_strategy ldpc_decoder_neon::select_analyze_var_to_check_strategy()
{
  if (node_size_neon != NODE_SIZE_NEON_PH) {
    return select_analyze_var_to_check_strategy<NODE_SIZE_NEON_PH - 1>();
  }

  return inner_analyze_var_to_check<NODE_SIZE_NEON_PH>;
}

template <>
ldpc_decoder_neon::analyze_var_to_check_strategy ldpc_decoder_neon::select_analyze_var_to_check_strategy<1>()
{
  return inner_analyze_var_to_check<1>;
}