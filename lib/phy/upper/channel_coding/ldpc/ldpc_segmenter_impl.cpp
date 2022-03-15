#include "ldpc_segmenter_impl.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_codeblock_description.h"
#include "srsgnb/support/srsran_assert.h"
#include <cmath>

using namespace srsgnb;
using namespace srsgnb::ldpc;

static constexpr unsigned crc_length = 24;

void ldpc_segmenter_impl::compute_nof_segments()
{
  if (nof_tb_bits_in <= max_segment_length) {
    nof_segments    = 1;
    nof_tb_bits_out = nof_tb_bits_in;
  } else {
    double tmp      = (1.0 * nof_tb_bits_in) / (max_segment_length - crc_length);
    nof_segments    = static_cast<unsigned>(std::ceil(tmp));
    nof_tb_bits_out = nof_tb_bits_in + nof_segments * crc_length;
  }
}

void ldpc_segmenter_impl::compute_lifting_size()
{
  unsigned ref_length{22};
  if (base_graph == base_graph_t::BG2) {
    if (nof_tb_bits_in > 640) {
      ref_length = 10;
    } else if (nof_tb_bits_in > 560) {
      ref_length = 9;
    } else if (nof_tb_bits_in > 192) {
      ref_length = 8;
    } else {
      ref_length = 6;
    }
  }

  unsigned total_ref_length = nof_segments * ref_length;

  lifting_size = 0;
  for (auto ls : all_lifting_sizes) {
    if (ls * total_ref_length > nof_tb_bits_out) {
      lifting_size = ls;
      break;
    }
  }
  assert(lifting_size != 0);
}

void ldpc_segmenter_impl::compute_segment_length()
{
  constexpr unsigned base_length_BG1 = BG1_N_full - BG1_M;
  constexpr unsigned base_length_BG2 = BG2_N_full - BG2_M;
  unsigned           base_length     = (base_graph == base_graph_t::BG1) ? base_length_BG1 : base_length_BG2;

  segment_length = base_length * lifting_size;
}

unsigned ldpc_segmenter_impl::compute_rm_length(unsigned i_seg, modulation_scheme mod, unsigned nof_layers) const
{
  double tmp = (1.0 * symbols_per_layer) / nof_segments;
  if (i_seg < nof_short_segments) {
    tmp = std::floor(tmp);
  } else {
    tmp = std::ceil(tmp);
  }
  return static_cast<unsigned>(tmp) * nof_layers * static_cast<unsigned>(mod);
}

static void
fill_segment(span<uint8_t> segment, span<const uint8_t> tr_block, unsigned nof_crc_bits, unsigned nof_filler)
{
  assert(segment.size() == tr_block.size() + nof_crc_bits + nof_filler);

  std::copy(tr_block.begin(), tr_block.end(), segment.begin());

  unsigned nof_used_bits = tr_block.size();
  if (nof_crc_bits > 0) {
    std::fill_n(segment.begin() + nof_used_bits, nof_crc_bits, 0);
    nof_used_bits += nof_crc_bits;
  }

  std::fill_n(segment.begin() + nof_used_bits, nof_filler, filler_bit);
}

static void check_inputs(const ldpc_segmenter::segmented_codeblocks& segments,
                         const tb_segment_description&               segment_descriptions,
                         span<const uint8_t>                         transport_block,
                         const ldpc_segmenter::config_t&             cfg)
{
  srsran_assert(segments.empty(), "Argument segments should be empty.");
  srsran_assert(segment_descriptions.empty(), "Argument segment_descriptions should be empty.");
  srsran_assert(!transport_block.empty(), "Argument transport_block should not be empty.");

  srsran_assert((cfg.rv >= 0) && (cfg.rv <= 3), "Invalid redundancy version.");

  srsran_assert((cfg.nof_layers >= 1) && (cfg.nof_layers <= 4), "Invalid number of layers.");

  srsran_assert(cfg.nof_ch_symbols % (cfg.nof_layers) == 0,
                "The number of channel symbols should be a multiple of the product between the number of layers.");
}

void ldpc_segmenter_impl::segment(segmented_codeblocks&   segments,
                                  tb_segment_description& segment_descriptions,
                                  span<const uint8_t>     transport_block,
                                  const config_t&         cfg)
{
  check_inputs(segments, segment_descriptions, transport_block, cfg);

  base_graph               = cfg.base_graph;
  max_segment_length       = (base_graph == base_graph_t::BG1) ? max_BG1_block_length : max_BG2_block_length;
  nof_tb_bits_in           = transport_block.size();
  nof_available_coded_bits = cfg.nof_ch_symbols * static_cast<unsigned>(cfg.mod);

  compute_nof_segments();
  compute_lifting_size();
  compute_segment_length();

  unsigned crc_bits{0};
  if (nof_segments > 1) {
    crc_bits = crc_length;
  }
  double   tmp_info_bits = (1.0 * nof_tb_bits_out) / nof_segments;
  unsigned max_info_bits = static_cast<unsigned>(std::ceil(tmp_info_bits)) - crc_bits;

  symbols_per_layer  = cfg.nof_ch_symbols / cfg.nof_layers;
  nof_short_segments = nof_segments - (symbols_per_layer % nof_segments);

  unsigned input_idx{0};
  for (unsigned i_segment = 0; i_segment != nof_segments; ++i_segment) {
    std::vector<uint8_t> tmp_segment(segment_length);
    unsigned             nof_info_bits   = std::min(max_info_bits, nof_tb_bits_in - input_idx);
    unsigned             nof_filler_bits = segment_length - nof_info_bits - crc_bits;
    fill_segment(tmp_segment, transport_block.subspan(input_idx, nof_info_bits), crc_bits, nof_filler_bits);
    segments.push_back(tmp_segment);

    codeblock_description tmp_description{};

    tmp_description.tb_common.base_graph   = base_graph;
    tmp_description.tb_common.lifting_size = static_cast<lifting_size_t>(lifting_size);
    tmp_description.tb_common.rv           = cfg.rv;
    tmp_description.tb_common.mod          = cfg.mod;
    tmp_description.tb_common.Nref         = cfg.Nref;

    constexpr unsigned inverse_BG1_rate     = 3;
    constexpr unsigned inverse_BG2_rate     = 5;
    unsigned           inverse_rate         = (base_graph == base_graph_t::BG1) ? inverse_BG1_rate : inverse_BG2_rate;
    tmp_description.cb_specific.full_length = segment_length * inverse_rate;
    tmp_description.cb_specific.nof_filler_bits = nof_filler_bits;
    tmp_description.cb_specific.rm_length       = compute_rm_length(i_segment, cfg.mod, cfg.nof_layers);

    segment_descriptions.push_back(tmp_description);
  }
}

std::unique_ptr<ldpc_segmenter> srsgnb::create_ldpc_segmenter()
{
  return std::make_unique<ldpc_segmenter_impl>();
}