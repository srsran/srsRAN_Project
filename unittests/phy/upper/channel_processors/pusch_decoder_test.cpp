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
/// \brief Unit test for PUSCH decoder.
///
/// Vector-based unit test. Each test vector contains the UL-SCH transport block and the LLRs corresponding to multiple
/// retransmissions with different redundancy versions. For each retransmission, the test combines the received codeword
/// with the LLRs of previous transmissions (if any) and decodes the resulting sequence of LLRs. The test vectors have
/// been generated with very little noise, so that the decoding should be successful after each retransmission with only
/// few LDPC iterations.

#include "pusch_decoder_test_data.h"
#include "srsgnb/phy/upper/channel_processors/pusch_decoder.h"
#include "srsgnb/phy/upper/rx_softbuffer_pool.h"
#include "srsgnb/support/test_utils.h"

/// \cond
namespace srsgnb {
struct rx_softbuffer_pool_description {
  unsigned max_codeblock_size;
  unsigned max_softbuffers;
  unsigned max_nof_codeblocks;
  unsigned expire_timeout_slots;
};

std::unique_ptr<rx_softbuffer_pool> create_rx_softbuffer_pool(const rx_softbuffer_pool_description& config);

} // namespace srsgnb

using namespace srsgnb;

int main()
{
  std::unique_ptr<pusch_decoder> decoder = create_pusch_decoder();

  rx_softbuffer_pool_description pool_config = {};

  for (const auto& test_data : pusch_decoder_test_data) {
    segment_config        cfg         = test_data.config;
    std::vector<unsigned> rv_sequence = test_data.rv_sequence;
    std::vector<int8_t>   llrs_all    = test_data.llrs.read();
    TESTASSERT(!llrs_all.empty());
    std::vector<uint8_t> ref_tb = test_data.transport_block.read();
    TESTASSERT(!ref_tb.empty());

    unsigned bits_per_symbol = get_bits_per_symbol(cfg.mod);
    unsigned nof_retx        = rv_sequence.size();
    TESTASSERT_EQ(cfg.nof_ch_symbols * bits_per_symbol * nof_retx, llrs_all.size(), "Wrong number of LLRs.");

    constexpr unsigned BITS_PER_BYTE = 8;
    unsigned           tbs           = ref_tb.size() * BITS_PER_BYTE;

    // Recall that llrs_all contains all retransmissions concatenated.
    unsigned cws = llrs_all.size() / nof_retx;

    unsigned nof_codeblocks = compute_nof_codeblocks(tbs, cfg.base_graph);

    // The codeword is the concatenation of codeblocks. However, since codeblock sizes can vary slightly, we add some
    // extra margin.
    pool_config.max_codeblock_size   = static_cast<unsigned>(std::round(cws * 5 / nof_codeblocks));
    pool_config.max_softbuffers      = 1;
    pool_config.max_nof_codeblocks   = nof_codeblocks;
    pool_config.expire_timeout_slots = 10;

    // Create Rx softbuffer pool.
    std::unique_ptr<rx_softbuffer_pool> pool = create_rx_softbuffer_pool(pool_config);
    TESTASSERT(pool);

    // Reserve softbuffer.
    rx_softbuffer* softbuffer = pool->reserve_softbuffer({}, {}, nof_codeblocks);
    TESTASSERT(softbuffer);

    pusch_decoder::statistics dec_stats = {};

    std::size_t cw_offset = 0;
    bool        new_data  = true;
    for (auto rv : rv_sequence) {
      cfg.rv = rv;
      std::vector<uint8_t> rx_tb(tbs / BITS_PER_BYTE);

      decoder->decode(
          rx_tb, dec_stats, softbuffer, span<const int8_t>(llrs_all).subspan(cw_offset, cws), new_data, cfg, {});
      cw_offset += cws;
      new_data = false;

      TESTASSERT(dec_stats.tb_crc_ok, "TB CRC checksum failed.");
      TESTASSERT(std::equal(rx_tb.cbegin(), rx_tb.cend(), ref_tb.cbegin()), "TB not decoded correctly.");
      TESTASSERT(dec_stats.nof_ldpc_iterations < 2.2, "Too many decoder iterations.");

      // Force all CRCs to false to test LLR combining.
      softbuffer->reset_codeblocks_crc();
    }
  }
}
/// \endcond