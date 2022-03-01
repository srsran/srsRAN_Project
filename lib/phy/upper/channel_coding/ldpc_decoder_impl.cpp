#include "ldpc_decoder_impl.h"
#include "ldpc_luts_impl.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

void ldpc_decoder_impl::init(const config_t& cfg)
{
  uint8_t  pos  = get_lifting_size_position(cfg.lifting_size);
  unsigned skip = static_cast<unsigned>(cfg.base_graph) * nof_lifting_sizes;
  current_graph = &graph_array[skip + pos];
  bg_N_full     = current_graph->get_nof_BG_var_nodes_full();
  bg_N_short    = current_graph->get_nof_BG_var_nodes_short();
  bg_M          = current_graph->get_nof_BG_check_nodes();
  bg_K          = current_graph->get_nof_BG_info_nodes();
  assert(bg_K == bg_N_full - bg_M);
  lifting_size = static_cast<uint16_t>(cfg.lifting_size);

  max_iterations = cfg.max_iterations;

  crc = cfg.crc;

  select_strategy();
}

unsigned ldpc_decoder_impl::decode(span<const int8_t> input, span<uint8_t> output, const config_t& cfg)
{
  init(cfg);

  uint16_t message_length   = bg_K * lifting_size;
  uint16_t max_input_length = bg_N_short * lifting_size;
  assert(output.size() == message_length);
  assert(input.size() <= max_input_length);

  // The minimum input length is message_length + two times the lifting size.
  uint16_t min_input_length = message_length + 2 * lifting_size;
  assert(input.size() >= min_input_length);

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
      extract_message(output);

      // Early stop
      if (crc->calculate_bit(output) == 0) {
        return i_iteration + 1;
      }
    }
  }

  // If a CRC calculator was passed with the configuration parameters and we hit this point, the codeblock wasn't decded
  // correctly.
  if (crc != nullptr) {
    return 0;
  }

  // We reach this point only if we don't have a CRC calculator for early stopping: we return whatever message we could
  // reconstruct after max_iterations (note that we don't know whether the message is correct or not).
  extract_message(output);
  return max_iterations;
}

std::unique_ptr<ldpc_decoder> srsgnb::create_ldpc_decoder(const std::string& dec_type)
{
  if (dec_type == "generic") {
    return std::make_unique<ldpc_decoder_generic>();
  }
  if (dec_type == "avx2") {
    return std::make_unique<ldpc_decoder_avx2>();
  }
  assert(false);
}
