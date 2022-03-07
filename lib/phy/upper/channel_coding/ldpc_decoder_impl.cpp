#include "ldpc_decoder_impl.h"
#include "ldpc_luts_impl.h"
#include "srsgnb/support/srsran_assert.h"
#include <cmath>

using namespace srsgnb;
using namespace srsgnb::ldpc;

// We use this value to represent infinity, that is perfectly known bit.
static constexpr int8_t LOCAL_INF = INT8_MAX;
// Messages depending on uncertain bits are quantized over 7 bits (from -63 to 63).
constexpr int8_t LOCAL_MAX_RANGE = 63;

void ldpc_decoder_impl::init(const config_t& cfg)
{
  uint8_t  pos   = get_lifting_size_position(cfg.lifting_size);
  unsigned skip  = static_cast<unsigned>(cfg.base_graph) * nof_lifting_sizes;
  current_graph  = &graph_array[skip + pos];
  bg_N_full      = current_graph->get_nof_BG_var_nodes_full();
  bg_N_short     = current_graph->get_nof_BG_var_nodes_short();
  bg_M           = current_graph->get_nof_BG_check_nodes();
  bg_K           = current_graph->get_nof_BG_info_nodes();
  bg_N_high_rate = bg_K + 4;
  assert(bg_K == bg_N_full - bg_M);
  lifting_size = static_cast<uint16_t>(cfg.lifting_size);

  max_iterations = cfg.max_iterations;
  assert(max_iterations > 0);

  scaling_factor = cfg.scaling_factor;
  assert((scaling_factor > 0) && (scaling_factor < 2));

  crc = cfg.crc;

  select_strategy();
}

unsigned ldpc_decoder_impl::decode(span<uint8_t> output, span<const int8_t> input, const config_t& cfg)
{
  init(cfg);

  uint16_t message_length   = bg_K * lifting_size;
  uint16_t max_input_length = bg_N_short * lifting_size;
  assert(output.size() == message_length);
  assert(input.size() <= max_input_length);

  // The minimum input length is message_length + two times the lifting size.
  uint16_t min_input_length = message_length + 2 * lifting_size;
  srsran_assert(input.size() >= min_input_length,
                "The input length (%d) does not reach miminum (%d)",
                input.size(),
                max_input_length);

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
      if (crc->calculate_bit(output) == 0) {
        return i_iteration + 1;
      }
    }
  }

  // If a CRC calculator was passed with the configuration parameters and we hit this point, the codeblock wasn't
  // decoded correctly.
  if (crc != nullptr) {
    return 0;
  }

  // We reach this point only if we don't have a CRC calculator for early stopping: we return whatever message we could
  // reconstruct after max_iterations (note that we don't know whether the message is correct or not).
  get_hard_bits(output);
  return max_iterations;
}

void ldpc_decoder_generic::load_soft_bits(span<const int8_t> llrs)
{
  // Erase registers.
  std::fill(soft_bits.begin(), soft_bits.end(), 0);
  std::fill(var_to_check.begin(), var_to_check.end(), 0);
  for (auto& tmp : check_to_var) {
    std::fill(tmp.begin(), tmp.end(), 0);
  }

  // Recall that the first 2 * lifting_size bits are not transmitted.
  unsigned nof_shortened_bits = 2 * lifting_size;
  std::copy(llrs.begin(), llrs.end(), soft_bits.begin() + nof_shortened_bits);
}

void ldpc_decoder_generic::update_variable_to_check_messages(unsigned i_layer)
{
  // First, update the messages corresponding to the high-rate region. All layers contribute.
  span<const int8_t> this_soft_bits(soft_bits);
  span<const int8_t> this_check_to_var(check_to_var[i_layer]);
  span<int8_t>       this_var_to_check(var_to_check);

  compute_var_to_check_msgs(this_soft_bits.subspan(0, nof_hrr_nodes),
                            this_check_to_var.subspan(0, nof_hrr_nodes),
                            this_var_to_check.subspan(0, nof_hrr_nodes));

  // Next, update the messages corresponding to the extension region, if applicable.
  // From layer 4 onwards, each layer is connected to only one consecutive block of lifting_size bits.
  if (i_layer >= 4) {
    unsigned skip_soft_bits = nof_hrr_nodes + (i_layer - 4) * lifting_size;
    compute_var_to_check_msgs(this_soft_bits.subspan(skip_soft_bits, lifting_size),
                              this_check_to_var.subspan(nof_hrr_nodes, lifting_size),
                              this_var_to_check.subspan(nof_hrr_nodes, lifting_size));
  }
}

void ldpc_decoder_generic::compute_var_to_check_msgs(span<const int8_t> soft, span<const int8_t> c2v, span<int8_t> v2c)
{
  unsigned nof_messages = v2c.size();
  assert((soft.size() == nof_messages) && (c2v.size() == nof_messages));

  for (unsigned i = 0; i != nof_messages; ++i) {
    if (abs(soft[i]) >= LOCAL_INF) {
      v2c[i] = (soft[i] > 0) ? LOCAL_INF : -LOCAL_INF;
      continue;
    }
    int tmp = static_cast<int>(soft[i]) - c2v[i];
    if (abs(tmp) > LOCAL_MAX_RANGE) {
      tmp = (tmp > 0) ? LOCAL_MAX_RANGE : -LOCAL_MAX_RANGE;
    }
    v2c[i] = static_cast<int8_t>(tmp);
  }
}

void ldpc_decoder_generic::update_check_to_variable_messages(unsigned i_layer)
{
  // Prepare helper registers
  std::fill(sign_prod_var_to_check.begin(), sign_prod_var_to_check.end(), 1);
  std::fill(min_var_to_check.begin(), min_var_to_check.end(), LOCAL_INF);
  std::fill(second_min_var_to_check.begin(), second_min_var_to_check.end(), LOCAL_INF);

  const BG_adjacency_row_t& current_var_indices = current_graph->get_adjacency_row(i_layer);
  const auto*               this_var_index      = current_var_indices.cbegin();
  const auto*               last_var_index      = current_var_indices.cend();
  for (; (this_var_index != last_var_index) && (*this_var_index != NO_EDGE); ++this_var_index) {
    unsigned shift          = current_graph->get_lifted_node(i_layer, *this_var_index);
    unsigned v2c_base_index = *this_var_index * lifting_size;
    v2c_base_index          = (v2c_base_index <= nof_hrr_nodes) ? v2c_base_index : nof_hrr_nodes;

    // Look for the two var_to_check messages with minimum absolute value and compute the sign product of all
    // var_to_check messages.
    for (unsigned j = 0; j != lifting_size; ++j) {
      unsigned v2c_index                 = v2c_base_index + j;
      int8_t   this_var_to_check         = static_cast<int8_t>(abs(var_to_check[v2c_index]));
      unsigned tmp_index                 = (j + lifting_size - shift) % lifting_size;
      bool     is_min                    = (this_var_to_check < min_var_to_check[tmp_index]);
      int8_t   new_second_min            = is_min ? min_var_to_check[tmp_index] : this_var_to_check;
      bool     is_best_two               = (this_var_to_check < second_min_var_to_check[tmp_index]);
      second_min_var_to_check[tmp_index] = is_best_two ? new_second_min : second_min_var_to_check[tmp_index];
      min_var_to_check[tmp_index]        = is_min ? this_var_to_check : min_var_to_check[tmp_index];
      min_var_to_check_index[tmp_index]  = is_min ? v2c_index : min_var_to_check_index[tmp_index];

      sign_prod_var_to_check[tmp_index] *= (var_to_check[v2c_index] >= 0) ? 1 : -1;
    }
  }

  // Recall: check_to_var is an array of arrays of int8_t.
  auto& this_check_to_var = check_to_var[i_layer];

  for (this_var_index = current_var_indices.cbegin();
       (this_var_index != last_var_index) && (*this_var_index != NO_EDGE);
       ++this_var_index) {
    unsigned shift          = current_graph->get_lifted_node(i_layer, *this_var_index);
    unsigned c2v_base_index = *this_var_index * lifting_size;
    c2v_base_index          = (c2v_base_index <= nof_hrr_nodes) ? c2v_base_index : nof_hrr_nodes;
    for (unsigned j = 0; j != lifting_size; ++j) {
      unsigned c2v_index = c2v_base_index + j;
      unsigned tmp_index = (j + lifting_size - shift) % lifting_size;

      this_check_to_var[c2v_index] = (c2v_index != min_var_to_check_index[tmp_index])
                                         ? min_var_to_check[tmp_index]
                                         : second_min_var_to_check[tmp_index];

      this_check_to_var[c2v_index] =
          static_cast<int8_t>(std::round(static_cast<float>(this_check_to_var[c2v_index]) * scaling_factor));

      this_check_to_var[c2v_index] = static_cast<int8_t>(
          this_check_to_var[c2v_index] * sign_prod_var_to_check[tmp_index] * ((var_to_check[c2v_index] >= 0) ? 1 : -1));
    }
  }
}

void ldpc_decoder_generic::update_soft_bits(unsigned i_layer)
{
  // Recall: check_to_var is an array of arrays of int8_t.
  auto& this_check_to_var = check_to_var[i_layer];

  const BG_adjacency_row_t& current_var_indices = current_graph->get_adjacency_row(i_layer);
  const auto*               this_var_index      = current_var_indices.cbegin();
  const auto*               last_var_index      = current_var_indices.cend();
  for (; (this_var_index != last_var_index) && (*this_var_index != NO_EDGE); ++this_var_index) {
    unsigned var_index_lifted = *this_var_index * lifting_size;
    for (int j = 0; j != lifting_size; ++j) {
      unsigned bit_index     = var_index_lifted + j;
      unsigned bit_index_tmp = (var_index_lifted <= nof_hrr_nodes) ? bit_index : nof_hrr_nodes + j;
      int      tmp           = this_check_to_var[bit_index_tmp] + var_to_check[bit_index_tmp];

      // Soft bits absolutely larger than LOCAL_MAX_RANGE are set to infinity (LOCAL_INF). As a result, they become
      // fixed bits, that is they won't change their value from now on.
      if (abs(tmp) > LOCAL_MAX_RANGE) {
        tmp = (tmp > 0) ? LOCAL_INF : -LOCAL_INF;
      }

      soft_bits[bit_index] = static_cast<int8_t>(tmp);
    }
  }
}

void ldpc_decoder_generic::get_hard_bits(span<uint8_t> out)
{
  unsigned out_length = out.size();
  std::transform(soft_bits.cbegin(), soft_bits.cbegin() + out_length, out.begin(), [](int8_t sb) { return (sb < 0); });
}

std::unique_ptr<ldpc_decoder> srsgnb::create_ldpc_decoder(const std::string& dec_type)
{
  if (dec_type == "generic") {
    return std::make_unique<ldpc_decoder_generic>();
  }
  if (dec_type == "avx2") {
    return std::make_unique<ldpc_decoder_avx2>();
  }
  srsran_assert(false, "Invalid decoder type '%s'. Valid types are: generic and avx2", dec_type.c_str());
  return {};
}
