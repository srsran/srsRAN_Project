#include "ldpc_rate_matching_impl.h"
#include "ldpc_luts_impl.h"
#include "srsgnb/support/srsran_assert.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

void ldpc_rate_matching_impl::init(const config_t& cfg)
{
  srsran_assert((cfg.rv >= 0) && (cfg.rv <= 3), "RV should an integer between 0 and 3.");
  rv = cfg.rv;

  modulation_order = static_cast<uint8_t>(cfg.mod);
}

void ldpc_rate_matching_impl::rate_match(span<uint8_t> output, span<const uint8_t> input, const config_t& cfg)
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
  if (block_length % BG1_N_short == 0) {
    // input is a BG1 codeblock
    shift_factor = {0, 17, 33, 56};
    lifting_size = block_length / BG1_N_short;
  } else if (block_length % BG2_N_short == 0) {
    // input is a BG2 codeblock
    shift_factor = {0, 13, 25, 43};
    lifting_size = block_length / BG2_N_short;
  } else {
    srsran_assert(false, "LDPC rate matching: invalid input length.");
  }
  assert(get_lifting_index(static_cast<lifting_size_t>(lifting_size)) != VOID_LIFTSIZE);
  double tmp = (shift_factor[rv] * buffer_length) / block_length;
  shift_k0   = static_cast<uint16_t>(floor(tmp)) * lifting_size;

  buffer = input.subspan(0, buffer_length);

  if (modulation_order == 1) {
    select_bits(buffer, output);
  } else {
    span<uint8_t> aux = span<uint8_t>(auxiliary_buffer).subspan(0, output.size());
    select_bits(buffer, aux);
    interleave_bits(aux, output);
  }
}

void ldpc_rate_matching_impl::select_bits(span<const uint8_t> in, span<uint8_t> out) const
{
  unsigned in_index = shift_k0 % buffer_length;
  for (auto& this_out : out) {
    while (in[in_index] == filler_bit) {
      in_index = (in_index + 1) % buffer_length;
    }
    this_out = in[in_index];
    in_index = (in_index + 1) % buffer_length;
  }
}

void ldpc_rate_matching_impl::interleave_bits(span<const uint8_t> in, span<uint8_t> out) const
{
  unsigned E = in.size();
  unsigned out_index{0};

  for (auto& this_out : out) {
    unsigned help_index = out_index % modulation_order;
    unsigned in_index   = (out_index + help_index * (E - 1)) / modulation_order;
    this_out            = in[in_index];
    ++out_index;
  }
}

std::unique_ptr<srsgnb::ldpc_rate_matching> srsgnb::create_ldpc_rate_matching()
{
  return std::make_unique<ldpc_rate_matching_impl>();
}
