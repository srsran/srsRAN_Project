/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ldpc_decoder_impl.h"
#include "ldpc_luts_impl.h"
#include "srsgnb/srsvec/zero.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <cmath>

using namespace srsgnb;
using namespace srsgnb::ldpc;

void ldpc_decoder_impl::init(const configuration& cfg)
{
  uint8_t  pos   = get_lifting_size_position(cfg.block_conf.tb_common.lifting_size);
  unsigned skip  = (cfg.block_conf.tb_common.base_graph == ldpc_base_graph_type::BG2) ? NOF_LIFTING_SIZES : 0;
  current_graph  = &graph_array[skip + pos];
  bg_N_full      = current_graph->get_nof_BG_var_nodes_full();
  bg_N_short     = current_graph->get_nof_BG_var_nodes_short();
  bg_M           = current_graph->get_nof_BG_check_nodes();
  bg_K           = current_graph->get_nof_BG_info_nodes();
  bg_N_high_rate = bg_K + 4;
  assert(bg_K == bg_N_full - bg_M);
  lifting_size = static_cast<uint16_t>(cfg.block_conf.tb_common.lifting_size);

  max_iterations = cfg.algorithm_conf.max_iterations;
  assert(max_iterations > 0);

  scaling_factor = cfg.algorithm_conf.scaling_factor;
  assert((scaling_factor > 0) && (scaling_factor < 1));

  unsigned nof_crc_bits = cfg.block_conf.cb_specific.nof_crc_bits;
  srsgnb_assert((nof_crc_bits == 16) || (nof_crc_bits == 24), "Invalid number of CRC bits.");

  nof_significant_bits = bg_K * lifting_size - cfg.block_conf.cb_specific.nof_filler_bits;

  select_strategy();
}

optional<unsigned> ldpc_decoder_impl::decode(span<uint8_t>                    output,
                                             span<const log_likelihood_ratio> input,
                                             crc_calculator*                  crc,
                                             const configuration&             cfg)
{
  init(cfg);

  uint16_t message_length   = bg_K * lifting_size;
  uint16_t max_input_length = bg_N_short * lifting_size;
  srsgnb_assert(output.size() == message_length,
                "The output size {} is not equal to the message length {}.",
                output.size(),
                message_length);
  srsgnb_assert(input.size() <= max_input_length,
                "The input size {} exceeds the maximum message length {}.",
                input.size(),
                max_input_length);

  // The minimum input length is message_length + two times the lifting size.
  uint16_t min_input_length = message_length + 2 * lifting_size;
  srsgnb_assert(input.size() >= min_input_length,
                "The input length {} does not reach minimum {}",
                input.size(),
                min_input_length);

  load_soft_bits(input);

  // The minimum codeblock length is message_length + four times the lifting size
  // (that is, the length of the high-rate region).
  uint16_t min_codeblock_length = message_length + 4 * lifting_size;
  // The decoder works with at least min_codeblock_length bits. Recall that the encoder also shortens
  // the codeblock by 2 * lifting size before returning it as output.
  codeblock_length = std::max(input.size() + 2UL * lifting_size, static_cast<size_t>(min_codeblock_length));
  // The decoder works with a codeblock length that is a multiple of the lifting size.
  if (codeblock_length % lifting_size != 0) {
    codeblock_length = (codeblock_length / lifting_size + 1) * lifting_size;
  }

  unsigned nof_layers = codeblock_length / lifting_size - bg_K;

  for (unsigned i_iteration = 0; i_iteration != max_iterations; ++i_iteration) {
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      update_variable_to_check_messages(i_layer);

      update_check_to_variable_messages(i_layer);

      update_soft_bits(i_layer);
    }

    // If a CRC calculator was passed with the configuration parameters
    if (crc != nullptr) {
      get_hard_bits(output);

      // Early stop
      if (crc->calculate_bit(output.first(nof_significant_bits)) == 0) {
        return i_iteration + 1;
      }
    }
  }

  // If a CRC calculator was passed with the configuration parameters and we hit this point, the codeblock wasn't
  // decoded correctly.
  if (crc != nullptr) {
    return {};
  }

  // We reach this point only if we don't have a CRC calculator for early stopping: we return whatever message we could
  // reconstruct after max_iterations (note that we don't know whether the message is correct or not).
  get_hard_bits(output);
  return {};
}

void ldpc_decoder_generic::load_soft_bits(span<const log_likelihood_ratio> llrs)
{
  // Erase registers.
  for (auto& tmp : check_to_var) {
    std::fill(tmp.begin(), tmp.end(), 0);
  }

  // Recall that the first 2 * lifting_size bits are not transmitted.
  unsigned nof_shortened_bits = 2 * lifting_size;
  srsvec::zero(span<log_likelihood_ratio>(soft_bits).first(nof_shortened_bits));
  std::copy(llrs.begin(), llrs.end(), soft_bits.begin() + nof_shortened_bits);
}

void ldpc_decoder_generic::update_variable_to_check_messages(unsigned check_node)
{
  // First, update the messages corresponding to the high-rate region. All layers contribute.
  span<const log_likelihood_ratio> this_soft_bits(soft_bits);
  span<const log_likelihood_ratio> this_check_to_var(check_to_var[check_node]);
  span<log_likelihood_ratio>       this_var_to_check(var_to_check);

  compute_var_to_check_msgs(this_var_to_check.first(nof_hrr_nodes),
                            this_soft_bits.first(nof_hrr_nodes),
                            this_check_to_var.first(nof_hrr_nodes));

  // Next, update the messages corresponding to the extension region, if applicable.
  // From layer 4 onwards, each layer is connected to only one consecutive block of lifting_size bits.
  if (check_node >= 4) {
    unsigned skip_soft_bits = nof_hrr_nodes + (check_node - 4) * lifting_size;
    compute_var_to_check_msgs(this_var_to_check.subspan(nof_hrr_nodes, lifting_size),
                              this_soft_bits.subspan(skip_soft_bits, lifting_size),
                              this_check_to_var.subspan(nof_hrr_nodes, lifting_size));
  }
}

void ldpc_decoder_generic::compute_var_to_check_msgs(span<log_likelihood_ratio>       v2c,
                                                     span<const log_likelihood_ratio> soft,
                                                     span<const log_likelihood_ratio> c2v)
{
  assert((soft.size() == v2c.size()) && (c2v.size() == v2c.size()));

  // By definition, the difference between two LLRs saturates at +/- LLR_MAX. Moreover, if either term is infinite, so
  // is the result, with proper sign.
  std::transform(
      soft.begin(), soft.end(), c2v.begin(), v2c.begin(), [](log_likelihood_ratio a, log_likelihood_ratio b) {
        return a - b;
      });
}

static log_likelihood_ratio scale_llr(log_likelihood_ratio llr, float scaling_factor)
{
  srsgnb_assert((scaling_factor > 0) && (scaling_factor < 1), "Scaling factor should be in the interval (0, 1).");
  if (log_likelihood_ratio::isinf(llr)) {
    return llr;
  }
  // Since scaling_factor belongs to (0, 1), there is no risk of overflow.
  return static_cast<log_likelihood_ratio::value_type>(
      std::round(static_cast<float>(llr.to_value_type()) * scaling_factor));
}

void ldpc_decoder_generic::update_check_to_variable_messages(unsigned check_node)
{
  // Prepare helper registers
  std::fill(sign_prod_var_to_check.begin(), sign_prod_var_to_check.end(), 1);
  std::fill(min_var_to_check.begin(), min_var_to_check.end(), LLR_MAX);
  std::fill(second_min_var_to_check.begin(), second_min_var_to_check.end(), LLR_MAX);

  const BG_adjacency_row_t& current_var_indices = current_graph->get_adjacency_row(check_node);
  const auto*               this_var_index      = current_var_indices.cbegin();
  const auto*               last_var_index      = current_var_indices.cend();
  for (; (this_var_index != last_var_index) && (*this_var_index != NO_EDGE); ++this_var_index) {
    unsigned shift          = current_graph->get_lifted_node(check_node, *this_var_index);
    unsigned v2c_base_index = *this_var_index * lifting_size;
    v2c_base_index          = (v2c_base_index <= nof_hrr_nodes) ? v2c_base_index : nof_hrr_nodes;

    // Look for the two var_to_check messages with minimum absolute value and compute the sign product of all
    // var_to_check messages.
    for (unsigned j = 0; j != lifting_size; ++j) {
      unsigned             v2c_index         = v2c_base_index + j;
      log_likelihood_ratio this_var_to_check = log_likelihood_ratio::abs(var_to_check[v2c_index]);
      unsigned             tmp_index         = (j + lifting_size - shift) % lifting_size;
      bool                 is_min            = (this_var_to_check < min_var_to_check[tmp_index]);
      log_likelihood_ratio new_second_min    = is_min ? min_var_to_check[tmp_index] : this_var_to_check;
      bool                 is_best_two       = (this_var_to_check < second_min_var_to_check[tmp_index]);
      second_min_var_to_check[tmp_index]     = is_best_two ? new_second_min : second_min_var_to_check[tmp_index];
      min_var_to_check[tmp_index]            = is_min ? this_var_to_check : min_var_to_check[tmp_index];
      min_var_to_check_index[tmp_index]      = is_min ? v2c_index : min_var_to_check_index[tmp_index];

      sign_prod_var_to_check[tmp_index] *= (var_to_check[v2c_index] >= 0) ? 1 : -1;
    }
  }

  // Recall: check_to_var is an array of arrays of log_likelihood_ratio.
  auto& this_check_to_var = check_to_var[check_node];

  for (this_var_index = current_var_indices.cbegin();
       (this_var_index != last_var_index) && (*this_var_index != NO_EDGE);
       ++this_var_index) {
    unsigned shift          = current_graph->get_lifted_node(check_node, *this_var_index);
    unsigned c2v_base_index = *this_var_index * lifting_size;
    c2v_base_index          = (c2v_base_index <= nof_hrr_nodes) ? c2v_base_index : nof_hrr_nodes;
    for (unsigned j = 0; j != lifting_size; ++j) {
      unsigned c2v_index = c2v_base_index + j;
      unsigned tmp_index = (j + lifting_size - shift) % lifting_size;

      this_check_to_var[c2v_index] = (c2v_index != min_var_to_check_index[tmp_index])
                                         ? min_var_to_check[tmp_index]
                                         : second_min_var_to_check[tmp_index];

      this_check_to_var[c2v_index] = scale_llr(this_check_to_var[c2v_index], scaling_factor);

      int final_sign               = sign_prod_var_to_check[tmp_index] * ((var_to_check[c2v_index] >= 0) ? 1 : -1);
      this_check_to_var[c2v_index] = log_likelihood_ratio::copysign(this_check_to_var[c2v_index], final_sign);
    }
  }
}

void ldpc_decoder_generic::update_soft_bits(unsigned check_node)
{
  // Recall: check_to_var is an array of arrays of log_likelihood_ratio.
  auto& this_check_to_var = check_to_var[check_node];

  const BG_adjacency_row_t& current_var_indices = current_graph->get_adjacency_row(check_node);
  const auto*               this_var_index      = current_var_indices.cbegin();
  const auto*               last_var_index      = current_var_indices.cend();
  for (; (this_var_index != last_var_index) && (*this_var_index != NO_EDGE); ++this_var_index) {
    unsigned var_index_lifted = *this_var_index * lifting_size;
    for (int j = 0; j != lifting_size; ++j) {
      unsigned bit_index     = var_index_lifted + j;
      unsigned bit_index_tmp = (var_index_lifted <= nof_hrr_nodes) ? bit_index : nof_hrr_nodes + j;

      // Soft bits absolutely larger than LOCAL_MAX_RANGE are set to infinity (LOCAL_INF). As a result, they become
      // fixed bits, that is they won't change their value from now on.
      soft_bits[bit_index] =
          log_likelihood_ratio::promotion_sum(this_check_to_var[bit_index_tmp], var_to_check[bit_index_tmp]);
    }
  }
}

void ldpc_decoder_generic::get_hard_bits(span<uint8_t> out)
{
  unsigned out_length = out.size();
  std::transform(soft_bits.cbegin(), soft_bits.cbegin() + out_length, out.begin(), [](log_likelihood_ratio sb) {
    return sb.to_hard_bit();
  });
}
