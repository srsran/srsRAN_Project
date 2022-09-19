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
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/phy/upper/rx_softbuffer_pool.h"
#include "srsgnb/support/test_utils.h"
#include <getopt.h>

/// \cond
using namespace srsgnb;

static bool     use_early_stop      = false;
static unsigned nof_ldpc_iterations = 6;

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-eX] [-iX]\n", prog);
  fmt::print("\t-e Use LDPC decoder early stop [Default {}]\n", use_early_stop);
  fmt::print("\t-i Number of LDPC iterations [Default {}]\n", nof_ldpc_iterations);
}

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "e:i:")) != -1) {
    //  printf("opt : %d\n", opt);
    switch (opt) {
      case 'e':
        use_early_stop = (strtol(optarg, nullptr, 10) != 0);
        break;
      case 'i':
        nof_ldpc_iterations = strtol(optarg, nullptr, 10);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

int main(int argc, char** argv)
{
  parse_args(argc, argv);

  std::shared_ptr<crc_calculator_factory> crc_calculator_factory = create_crc_calculator_factory_sw();
  TESTASSERT(crc_calculator_factory);

  std::shared_ptr<ldpc_decoder_factory> ldpc_decoder_factory = create_ldpc_decoder_factory_sw("generic");
  TESTASSERT(ldpc_decoder_factory);

  std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rate_dematcher_factory = create_ldpc_rate_dematcher_factory_sw();
  TESTASSERT(ldpc_rate_dematcher_factory);

  std::shared_ptr<ldpc_segmenter_rx_factory> segmenter_rx_factory = create_ldpc_segmenter_rx_factory_sw();
  TESTASSERT(segmenter_rx_factory);

  pusch_decoder_factory_sw_configuration pusch_decoder_factory_sw_config;
  pusch_decoder_factory_sw_config.crc_factory       = crc_calculator_factory;
  pusch_decoder_factory_sw_config.decoder_factory   = ldpc_decoder_factory;
  pusch_decoder_factory_sw_config.dematcher_factory = ldpc_rate_dematcher_factory;
  pusch_decoder_factory_sw_config.segmenter_factory = segmenter_rx_factory;
  std::shared_ptr<pusch_decoder_factory> pusch_decoder_factory =
      create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_config);
  TESTASSERT(pusch_decoder_factory);

  std::unique_ptr<pusch_decoder> decoder = pusch_decoder_factory->create();
  TESTASSERT(decoder);

  pusch_decoder_result dec_stats = {};

  rx_softbuffer_pool_description pool_config = {};

  for (const auto& test_data : pusch_decoder_test_data) {
    segmenter_config                  cfg         = test_data.config;
    std::vector<unsigned>             rv_sequence = test_data.rv_sequence;
    std::vector<log_likelihood_ratio> llrs_all    = test_data.llrs.read();
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

    unsigned nof_codeblocks = ldpc::compute_nof_codeblocks(tbs, cfg.base_graph);

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

    pusch_decoder::configuration dec_cfg = {};

    std::size_t cw_offset       = 0;
    dec_cfg.new_data            = true;
    dec_cfg.nof_ldpc_iterations = nof_ldpc_iterations;
    dec_cfg.use_early_stop      = use_early_stop;
    for (auto rv : rv_sequence) {
      cfg.rv = rv;
      std::vector<uint8_t> rx_tb(tbs / BITS_PER_BYTE);

      dec_cfg.segmenter_cfg = cfg;

      decoder->decode(
          rx_tb, dec_stats, softbuffer, span<const log_likelihood_ratio>(llrs_all).subspan(cw_offset, cws), dec_cfg);
      cw_offset += cws;
      dec_cfg.new_data = false;

      TESTASSERT(dec_stats.tb_crc_ok, "TB CRC checksum failed.");
      TESTASSERT_EQ(span<uint8_t>(rx_tb), span<uint8_t>(ref_tb), "TB not decoded correctly.");
      TESTASSERT_EQ(dec_stats.ldpc_decoder_stats.get_nof_observations(),
                    dec_stats.nof_codeblocks_total,
                    "Error reporting decoded codeblocks (use_early_stop={}).",
                    use_early_stop);
      if (use_early_stop) {
        TESTASSERT(dec_stats.ldpc_decoder_stats.get_max() <= 2, "Too many decoder iterations.");
      } else {
        TESTASSERT_EQ(dec_cfg.nof_ldpc_iterations,
                      dec_stats.ldpc_decoder_stats.get_min(),
                      "Something wrong with iteration counting (no early stop).");
      }

      // Force all CRCs to false to test LLR combining.
      softbuffer->reset_codeblocks_crc();
    }
  }
}
/// \endcond
