/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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

#include "pusch_decoder_notifier_spy.h"
#include "pusch_decoder_test_data.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/test_utils.h"
#ifdef HWACC_PUSCH_ENABLED
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"
#endif // HWACC_PUSCH_ENABLED
#include <getopt.h>

/// \cond
using namespace srsran;
using namespace units::literals;

static bool     use_early_stop      = false;
static unsigned nof_ldpc_iterations = 6;

static std::string decoder_type = "generic";

#ifdef HWACC_PUSCH_ENABLED
static bool        test_harq      = true;
static bool        ext_softbuffer = true;
static bool        std_out_sink   = true;
static std::string hal_log_level  = "error";
static std::string eal_arguments  = "";
#endif // HWACC_PUSCH_ENABLED

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-T X] [-e] [-i X] [-x] [-y] [-z error|warning|info|debug] [-h] [eal_args ...]\n", prog);
  fmt::print("\t-T       PUSCH decoder type [generic,acc100][Default {}]\n", decoder_type);
  fmt::print("\t-e       Use LDPC decoder early stop [Default {}]\n", use_early_stop);
  fmt::print("\t-i       Number of LDPC iterations [Default {}]\n", nof_ldpc_iterations);
#ifdef HWACC_PUSCH_ENABLED
  fmt::print("\t-x       Use the host's memory for the soft-buffer [Default {}]\n", !ext_softbuffer);
  fmt::print("\t-y       Force logging output written to a file [Default {}]\n", std_out_sink ? "std_out" : "file");
  fmt::print("\t-z       Set logging level for the HAL [Default {}]\n", hal_log_level);
  fmt::print("\teal_args EAL arguments\n");
#endif // HWACC_PUSCH_ENABLED
  fmt::print("\t-h       This help\n");
}

#ifdef HWACC_PUSCH_ENABLED
// Separates EAL and non-EAL arguments.
// The function assumes that 'eal_arg' flags the start of the EAL arguments and that no more non-EAL arguments follow.
static std::string capture_eal_args(int* argc, char*** argv)
{
  // Searchs for the EAL args (if any), flagged by 'eal_args', while removing all the rest (except argv[0]).
  bool        eal_found = false;
  char**      mod_argv  = *argv;
  std::string eal_argv  = {mod_argv[0]};
  int         opt_ind   = *argc;
  for (int j = 1; j < opt_ind; ++j) {
    // Search for the 'eal_args' flag (if any).
    if (!eal_found) {
      if (strcmp(mod_argv[j], "eal_args") == 0) {
        // 'eal_args' flag found.
        eal_found = true;
        // Remove all main app arguments starting from that point, while copying them to the EAL argument string.
        mod_argv[j] = NULL;
        for (int k = j + 1; k < opt_ind; ++k) {
          eal_argv += " ";
          eal_argv += mod_argv[k];
          mod_argv[k] = NULL;
        }
        *argc = j;
      }
    }
  }
  *argv = mod_argv;

  return eal_argv;
}
#endif // HWACC_PUSCH_ENABLED

static void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "T:e:i:xyz:h")) != -1) {
    switch (opt) {
      case 'T':
        decoder_type = std::string(optarg);
        break;
      case 'e':
        use_early_stop = (strtol(optarg, nullptr, 10) != 0);
        break;
      case 'i':
        nof_ldpc_iterations = strtol(optarg, nullptr, 10);
        break;
#ifdef HWACC_PUSCH_ENABLED
      case 'x':
        ext_softbuffer = false;
        break;
      case 'y':
        std_out_sink = false;
        break;
      case 'z':
        hal_log_level = std::string(optarg);
        break;
#endif // HWACC_PUSCH_ENABLED
      case 'h':
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

static std::shared_ptr<pusch_decoder_factory> create_generic_pusch_decoder_factory()
{
  std::shared_ptr<crc_calculator_factory> crc_calculator_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calculator_factory);

  std::shared_ptr<ldpc_decoder_factory> ldpc_decoder_factory = create_ldpc_decoder_factory_sw("auto");
  TESTASSERT(ldpc_decoder_factory);

  std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rate_dematcher_factory =
      create_ldpc_rate_dematcher_factory_sw("auto");
  TESTASSERT(ldpc_rate_dematcher_factory);

  std::shared_ptr<ldpc_segmenter_rx_factory> segmenter_rx_factory = create_ldpc_segmenter_rx_factory_sw();
  TESTASSERT(segmenter_rx_factory);

  pusch_decoder_factory_sw_configuration pusch_decoder_factory_sw_config;
  pusch_decoder_factory_sw_config.crc_factory       = crc_calculator_factory;
  pusch_decoder_factory_sw_config.decoder_factory   = ldpc_decoder_factory;
  pusch_decoder_factory_sw_config.dematcher_factory = ldpc_rate_dematcher_factory;
  pusch_decoder_factory_sw_config.segmenter_factory = segmenter_rx_factory;
  pusch_decoder_factory_sw_config.nof_prb           = MAX_RB;
  pusch_decoder_factory_sw_config.nof_layers        = pusch_constants::MAX_NOF_LAYERS;
  return create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_config);
}

static std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> create_hw_accelerator_pusch_dec_factory()
{
#ifdef HWACC_PUSCH_ENABLED
  srslog::sink* log_sink =
      std_out_sink ? srslog::create_stdout_sink() : srslog::create_file_sink("pusch_decoder_vectortest.log");
  srslog::set_default_sink(*log_sink);
  srslog::init();
  srslog::basic_logger& logger = srslog::fetch_basic_logger("HAL", false);
  logger.set_level(srslog::str_to_basic_level(hal_log_level));

  // Pointer to a dpdk-based hardware-accelerator interface.
  static std::unique_ptr<dpdk::dpdk_eal> dpdk_interface = nullptr;
  if (!dpdk_interface) {
    dpdk_interface = dpdk::create_dpdk_eal(eal_arguments, logger);
    TESTASSERT(dpdk_interface, "Failed to open DPDK EAL with arguments.");
  }

  // Intefacing to the bbdev-based hardware-accelerator.
  dpdk::bbdev_acc_configuration bbdev_config;
  bbdev_config.id                                    = 0;
  bbdev_config.nof_ldpc_enc_lcores                   = 0;
  bbdev_config.nof_ldpc_dec_lcores                   = 1;
  bbdev_config.nof_fft_lcores                        = 0;
  bbdev_config.nof_mbuf                              = static_cast<unsigned>(pow2(log2_ceil(MAX_NOF_SEGMENTS)));
  std::shared_ptr<dpdk::bbdev_acc> bbdev_accelerator = create_bbdev_acc(bbdev_config, logger);
  TESTASSERT(bbdev_accelerator);

  // Interfacing to a shared external HARQ buffer context repository.
  unsigned nof_cbs                   = MAX_NOF_SEGMENTS;
  uint64_t acc100_ext_harq_buff_size = bbdev_accelerator->get_harq_buff_size_bytes();
  std::shared_ptr<ext_harq_buffer_context_repository> harq_buffer_context =
      create_ext_harq_buffer_context_repository(nof_cbs, acc100_ext_harq_buff_size, test_harq);
  TESTASSERT(harq_buffer_context);

  // Set the hardware-accelerator configuration.
  hw_accelerator_pusch_dec_configuration hw_decoder_config;
  hw_decoder_config.acc_type            = "acc100";
  hw_decoder_config.bbdev_accelerator   = bbdev_accelerator;
  hw_decoder_config.ext_softbuffer      = ext_softbuffer;
  hw_decoder_config.harq_buffer_context = harq_buffer_context;

  // ACC100 hardware-accelerator implementation.
  return create_hw_accelerator_pusch_dec_factory(hw_decoder_config);
#else  // HWACC_PUSCH_ENABLED
  return nullptr;
#endif // HWACC_PUSCH_ENABLED
}

static std::shared_ptr<pusch_decoder_factory> create_acc100_pusch_decoder_factory()
{
  std::shared_ptr<crc_calculator_factory> crc_calculator_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calculator_factory);

  std::shared_ptr<ldpc_segmenter_rx_factory> segmenter_rx_factory = create_ldpc_segmenter_rx_factory_sw();
  TESTASSERT(segmenter_rx_factory);

  std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> hw_decoder_factory = create_hw_accelerator_pusch_dec_factory();
  TESTASSERT(hw_decoder_factory, "Failed to create a HW acceleration decoder factory.");

  // Set the hardware-accelerated PUSCH decoder configuration.
  pusch_decoder_factory_hw_configuration decoder_hw_factory_config;
  decoder_hw_factory_config.segmenter_factory  = segmenter_rx_factory;
  decoder_hw_factory_config.crc_factory        = crc_calculator_factory;
  decoder_hw_factory_config.hw_decoder_factory = hw_decoder_factory;
  return create_pusch_decoder_factory_hw(decoder_hw_factory_config);
}

static std::shared_ptr<pusch_decoder_factory> create_pusch_decoder_factory()
{
  if (decoder_type == "generic") {
    return create_generic_pusch_decoder_factory();
  }

  if (decoder_type == "acc100") {
    return create_acc100_pusch_decoder_factory();
  }

  return nullptr;
}

int main(int argc, char** argv)
{
#ifdef HWACC_PUSCH_ENABLED
  // Separate EAL and non-EAL arguments.
  eal_arguments = capture_eal_args(&argc, &argv);
#endif // HWACC_PUSCH_ENABLED

  // Parse generic arguments.
  parse_args(argc, argv);

  std::shared_ptr<pusch_decoder_factory> pusch_dec_factory = create_pusch_decoder_factory();
  TESTASSERT(pusch_dec_factory, "Failed to create PUSCH decoder factory of type {}.", decoder_type);

  std::unique_ptr<pusch_decoder> decoder = pusch_dec_factory->create();
  TESTASSERT(decoder);

  rx_buffer_pool_config pool_config = {};

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

    units::bits tbs = units::bytes(ref_tb.size()).to_bits();

    // Recall that llrs_all contains all retransmissions concatenated.
    unsigned cws = llrs_all.size() / nof_retx;

    unsigned nof_codeblocks = ldpc::compute_nof_codeblocks(tbs, cfg.base_graph);

    // The codeword is the concatenation of codeblocks. However, since codeblock sizes can vary slightly, we add some
    // extra margin.
    pool_config.max_codeblock_size   = static_cast<unsigned>(std::round(cws * 5 / nof_codeblocks));
    pool_config.nof_buffers          = 1;
    pool_config.nof_codeblocks       = nof_codeblocks;
    pool_config.expire_timeout_slots = 10;
    pool_config.external_soft_bits   = false;
#ifdef HWACC_PUSCH_ENABLED
    if (decoder_type == "acc100") {
      pool_config.external_soft_bits = ext_softbuffer;
    }
#endif // HWACC_PUSCH_ENABLED

    // Create Rx buffer pool.
    std::unique_ptr<rx_buffer_pool_controller> pool = create_rx_buffer_pool(pool_config);
    TESTASSERT(pool);

    pusch_decoder::configuration dec_cfg = {};

    std::size_t cw_offset       = 0;
    dec_cfg.new_data            = true;
    dec_cfg.nof_ldpc_iterations = nof_ldpc_iterations;
    dec_cfg.use_early_stop      = use_early_stop;

    // List of bit indexes within the codeword at which point the PUSCH decoder will be informed of the total number of
    // UL-SCH CW bits. This enables the PUSCH decoder to start decoding codeblocks.
    std::vector<units::bits> i_cw_decoding_start_list = {0_bits, units::bits((cws / 2) + 1), units::bits(cws - 1)};
    unsigned                 cw_dec_start_idx         = 0;

    for (auto rv : rv_sequence) {
      cfg.rv = rv;
      std::vector<uint8_t> rx_tb(tbs.truncate_to_bytes().value());

      // Prepare decoder configuration.
      dec_cfg.base_graph = cfg.base_graph;
      dec_cfg.rv         = cfg.rv;
      dec_cfg.mod        = cfg.mod;
      dec_cfg.Nref       = cfg.Nref;
      dec_cfg.nof_layers = cfg.nof_layers;

      // Reserve buffer.
      unique_rx_buffer buffer = pool->get_pool().reserve({}, trx_buffer_identifier(0, 0), nof_codeblocks, true);
      TESTASSERT(buffer.is_valid());

      // Reset code blocks CRCs.
      buffer.get().reset_codeblocks_crc();

      // Setup decoder for new data.
      pusch_decoder_notifier_spy decoder_notifier_spy;
      pusch_decoder_buffer& decoder_buffer = decoder->new_data(rx_tb, std::move(buffer), decoder_notifier_spy, dec_cfg);

      // Feed codeword.
      span<const log_likelihood_ratio> llrs_cw = span<const log_likelihood_ratio>(llrs_all).subspan(cw_offset, cws);

      // Bit index within the CW at which point the set_nof_softbits decoder function will be called.
      units::bits i_cw_dec_start = i_cw_decoding_start_list[cw_dec_start_idx];

      if (i_cw_dec_start > 0_bits) {
        // Feed bits before passing the number of UL-SCH CW bits to the decoder.
        span<const log_likelihood_ratio> llrs_before_nof_softbits_call = llrs_cw.first(i_cw_dec_start.value());
        decoder_buffer.on_new_softbits(llrs_before_nof_softbits_call);
      }

      if (i_cw_dec_start != units::bits(cws - 1)) {
        // Pass the number of soft bits to the decoder.
        decoder->set_nof_softbits(units::bits(cws));
      }

      // Feed the remaining softbits.
      span<const log_likelihood_ratio> llrs_after_nof_softbits_call = llrs_cw.last(cws - i_cw_dec_start.value());
      decoder_buffer.on_new_softbits(llrs_after_nof_softbits_call);
      decoder_buffer.on_end_softbits();

      // Extract decoding results.
      TESTASSERT(!decoder_notifier_spy.get_entries().empty());
      const pusch_decoder_result& dec_stats = decoder_notifier_spy.get_entries().back();

      cw_offset += cws;
      dec_cfg.new_data = false;

      TESTASSERT(dec_stats.tb_crc_ok, "TB CRC checksum failed for rv={}.", rv);
      TESTASSERT_EQ(span<uint8_t>(rx_tb), span<uint8_t>(ref_tb), "TB not decoded correctly.");
      TESTASSERT_EQ(dec_stats.ldpc_decoder_stats.get_nof_observations(),
                    dec_stats.nof_codeblocks_total,
                    "Error reporting decoded codeblocks (use_early_stop={} rv={}).",
                    use_early_stop,
                    rv);
      if (use_early_stop) {
        TESTASSERT(dec_stats.ldpc_decoder_stats.get_max() <= 2, "Too many decoder iterations.");
      } else {
        TESTASSERT_EQ(dec_cfg.nof_ldpc_iterations,
                      dec_stats.ldpc_decoder_stats.get_min(),
                      "Something wrong with iteration counting (no early stop).");
      }

      ++cw_dec_start_idx;
      cw_dec_start_idx %= i_cw_decoding_start_list.size();
    }
  }
}
/// \endcond
