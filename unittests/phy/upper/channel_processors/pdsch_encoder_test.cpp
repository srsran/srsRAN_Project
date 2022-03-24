#include "srsgnb/phy/upper/channel_processors/pdsch_encoder.h"
#include "srsgnb/srsvec/bit.h"
#include <iostream>
#include <random>

using namespace srsgnb;
using namespace srsgnb::ldpc;

static std::mt19937 rgen(0);

int main()
{
  std::uniform_int_distribution<> bin_pack_d(0, 7);
  constexpr unsigned              tbs = 3904;

  static_vector<uint8_t, tbs> transport_block(tbs / 8);
  std::generate(transport_block.begin(), transport_block.end(), [bin_pack_d]() mutable { return bin_pack_d(rgen); });

  segment_config pdsch_enc_cfg{};

  pdsch_enc_cfg.base_graph     = base_graph_t::BG1;
  pdsch_enc_cfg.mod            = modulation_scheme::QPSK;
  pdsch_enc_cfg.rv             = 0;
  pdsch_enc_cfg.nof_layers     = 1;
  pdsch_enc_cfg.nof_ch_symbols = 3000;

  static_vector<described_segment, MAX_NOF_SEGMENTS> d_segments;

  constexpr unsigned max_cw_length = 40000;
  unsigned           cw_length =
      pdsch_enc_cfg.nof_ch_symbols * static_cast<unsigned>(pdsch_enc_cfg.mod) * pdsch_enc_cfg.nof_layers;
  static_vector<uint8_t, max_cw_length> codeword(cw_length);

  std::unique_ptr<pdsch_encoder> pdsch_enc = create_pdsch_encoder();
  pdsch_enc->encode(codeword, transport_block, pdsch_enc_cfg);

  static_vector<uint8_t, max_cw_length> tr_block_unpacked(tbs);
  srsvec::bit_unpack(transport_block, tr_block_unpacked);

  // Not a very meaningful test, for now.
  srsran_assert(std::equal(codeword.begin(), codeword.begin() + 16, tr_block_unpacked.begin() + 384),
                "End-to-end encoding went wrong.");
}
