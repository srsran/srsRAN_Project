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
/// \brief Benchmark for hardware-accelerated PUSCH decoder implementations.
///
/// The benchmark compares the latency of a hardware-accelerated PUSCH decoder implementation to that of the generic
/// one.

#include "../../../../../unittests/phy/upper/channel_processors/pusch/pusch_decoder_notifier_spy.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/support/test_utils.h"
#ifdef DPDK_FOUND
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"
#include <rte_cycles.h>
#endif // DPDK_FOUND
#include <getopt.h>
#include <random>

/// \cond
using namespace srsran;

// A test case consists of a LDPC segmenter configuration, a Transport Block size, a number of LLRs and a PRB size.
using test_case_type = std::tuple<segmenter_config, unsigned, unsigned, unsigned>;

static std::string                        hwacc_decoder_type          = "acc100";
static bool                               ext_softbuffer              = true;
static bool                               use_early_stop              = true;
static unsigned                           nof_ldpc_iterations         = 2;
static dmrs_type                          dmrs                        = dmrs_type::TYPE1;
static unsigned                           nof_cdm_groups_without_data = 2;
static bounded_bitset<MAX_NSYMB_PER_SLOT> dmrs_symbol_mask =
    {false, false, true, false, false, false, false, false, false, false, false, false, false, false};

#ifdef DPDK_FOUND
static bool        test_harq     = false;
static std::string hal_log_level = "ERROR";
static bool        std_out_sink  = true;
static std::string eal_arguments = "";
#endif // DPDK_FOUND

// Test profile structure, initialized with default profile values.
struct test_profile {
  std::string                      name          = "default";
  std::string                      description   = "Runs all combinations.";
  subcarrier_spacing               scs           = subcarrier_spacing::kHz15;
  cyclic_prefix                    cp            = cyclic_prefix::NORMAL;
  unsigned                         nof_tx_layers = 1;
  unsigned                         nof_symbols   = get_nsymb_per_slot(cyclic_prefix::NORMAL);
  std::vector<unsigned>            nof_prb_set   = {25, 52, 106, 270};
  std::vector<sch_mcs_description> mcs_set       = {{modulation_scheme::QPSK, 120.0F},
                                                    {modulation_scheme::QAM16, 658.0F},
                                                    {modulation_scheme::QAM64, 873.0F},
                                                    {modulation_scheme::QAM256, 948.0F}};
};

// Profile selected during test execution.
static test_profile selected_profile = {};

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-T X] [-e] [-i X] [-x] [-y] [-z error|warning|info|debug] [-h] [eal_args ...]\n", prog);
  fmt::print("\t-T Hardware-accelerated PUSCH decoder type [acc100][Default {}]\n", hwacc_decoder_type);
  fmt::print("\t-e Use LDPC decoder early stop [Default {}]\n", use_early_stop);
  fmt::print("\t-i X Number of LDPC iterations [Default X = {}]\n", nof_ldpc_iterations);
#ifdef DPDK_FOUND
  fmt::print("\t-x Use the host's memory for the soft-buffer [Default {}]\n", !ext_softbuffer);
  fmt::print("\t-y Force logging output written to a file [Default {}]\n", std_out_sink ? "std_out" : "file");
  fmt::print("\t-z Force DEBUG logging level for the hal [Default {}]\n", hal_log_level);
#endif // DPDK_FOUND
  fmt::print("\t-h This help\n");
}

#ifdef DPDK_FOUND
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
#endif // DPDK_FOUND

static int parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "T:ei:xyz:h")) != -1) {
    switch (opt) {
      case 'T':
        hwacc_decoder_type = std::string(optarg);
        break;
      case 'e':
        use_early_stop = true;
        break;
      case 'i':
        nof_ldpc_iterations = strtol(optarg, nullptr, 10);
        break;
#ifdef DPDK_FOUND
      case 'x':
        ext_softbuffer = false;
        break;
      case 'y':
        std_out_sink = false;
        break;
      case 'z':
        hal_log_level = std::string(optarg);
        break;
#endif // DPDK_FOUND
      case 'h':
      default:
        usage(argv[0]);
        exit(0);
    }
  }
  return 0;
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
  return create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_config);
}

static std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> create_hw_accelerator_pusch_dec_factory()
{
#ifdef DPDK_FOUND
  srslog::sink* log_sink =
      std_out_sink ? srslog::create_stdout_sink() : srslog::create_file_sink("hwacc_decoderacc_benchmark.log");
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
#else  // DPDK_FOUND
  return nullptr;
#endif // DPDK_FOUND
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

static std::shared_ptr<pusch_decoder_factory> create_pusch_decoder_factory(std::string decoder_type)
{
  if (decoder_type == "generic") {
    return create_generic_pusch_decoder_factory();
  }

  if (decoder_type == "acc100") {
    return create_acc100_pusch_decoder_factory();
  }

  return nullptr;
}

// Generates a meaningful set of test cases.
static std::vector<test_case_type> generate_test_cases(const test_profile& profile)
{
  std::vector<test_case_type> test_case_set;

  for (sch_mcs_description mcs : profile.mcs_set) {
    for (unsigned nof_prb : profile.nof_prb_set) {
      // Determine the Transport Block Size.
      tbs_calculator_configuration tbs_config = {};
      tbs_config.mcs_descr                    = mcs;
      tbs_config.n_prb                        = nof_prb;
      tbs_config.nof_layers                   = profile.nof_tx_layers;
      tbs_config.nof_symb_sh                  = profile.nof_symbols;
      tbs_config.nof_dmrs_prb = dmrs.nof_dmrs_per_rb() * dmrs_symbol_mask.count() * nof_cdm_groups_without_data;
      unsigned tbs            = tbs_calculator_calculate(tbs_config);

      // Build the LDPC segmenter configuration.
      segmenter_config config = {};
      config.Nref             = 0;
      config.base_graph       = get_ldpc_base_graph(mcs.get_normalised_target_code_rate(), units::bits(tbs));
      config.mod              = mcs.modulation;
      config.nof_ch_symbols   = profile.nof_symbols * nof_prb * NRE;
      config.nof_layers       = profile.nof_tx_layers;
      config.rv               = 0;

      // Number of input LLRs to the decoder.
      unsigned bits_per_symbol = get_bits_per_symbol(mcs.modulation);
      unsigned nof_llr         = config.nof_ch_symbols * bits_per_symbol;

      test_case_set.emplace_back(
          std::tuple<segmenter_config, unsigned, unsigned, unsigned>(config, tbs, nof_llr, nof_prb));
    }
  }
  return test_case_set;
}

static uint64_t get_current_time()
{
#ifdef DPDK_FOUND
  return rte_rdtsc_precise();
#else  // DPDK_FOUND
  return 0;
#endif // DPDK_FOUND
}

// Returns a latency value in us.
static double conv_time_to_latency(uint64_t time)
{
#ifdef DPDK_FOUND
  uint64_t cpu_freq = static_cast<double>(rte_get_tsc_hz());
  return static_cast<double>(time * 1000000) / static_cast<double>(cpu_freq);
#else  // DPDK_FOUND
  return 0;
#endif // DPDK_FOUND
}

int main(int argc, char** argv)
{
#ifdef DPDK_FOUND
  // Separate EAL and non-EAL arguments.
  eal_arguments = capture_eal_args(&argc, &argv);
#endif // DPDK_FOUND

  // Parse main app arguments.
  int ret = parse_args(argc, argv);
  if (ret < 0) {
    return ret;
  }

  // Pseudo-random generator.
  std::mt19937 rgen(0);

  // Create the generic PUSCH decoder against which to benchmark the hardware-accelerated PUSCH decoder.
  std::shared_ptr<pusch_decoder_factory> gen_pusch_dec_factory = create_pusch_decoder_factory("generic");
  TESTASSERT(gen_pusch_dec_factory, "Failed to create PUSCH decoder factory of type {}.", "generic");

  std::unique_ptr<pusch_decoder> gen_decoder = gen_pusch_dec_factory->create();
  TESTASSERT(gen_decoder);

  // Create the hardware-accelerated PUSCH decoder.
  std::shared_ptr<pusch_decoder_factory> hwacc_pusch_dec_factory = create_pusch_decoder_factory(hwacc_decoder_type);
  TESTASSERT(hwacc_pusch_dec_factory,
             "Failed to create hardware-accelerated PUSCH decoder factory of type {}.",
             hwacc_decoder_type);

  std::unique_ptr<pusch_decoder> hwacc_decoder = hwacc_pusch_dec_factory->create();
  TESTASSERT(hwacc_decoder);

  rx_buffer_pool_config pool_config = {};

  // Create a vector to hold the randomly generated LLRs.
  unsigned                          max_nof_ch_symbols = 14 * 270 * NRE;
  unsigned                          max_nof_llrs       = max_nof_ch_symbols * 8;
  std::vector<log_likelihood_ratio> random_llrs(max_nof_llrs);

  // Generate random LLRs.
  std::generate(random_llrs.begin(), random_llrs.end(), [&]() { return static_cast<int8_t>((rgen() & 1) * 20 - 10); });

  // Generate the test cases.
  std::vector<test_case_type> test_case_set = generate_test_cases(selected_profile);

  fmt::print("Launching benchmark comparing generic and {} PUSCH decoder implementations\n\n", hwacc_decoder_type);

  for (const test_case_type& test_case : test_case_set) {
    // Get the PUSCH decoder configuration.
    const segmenter_config cfg = std::get<0>(test_case);
    // Get the TBS in bits.
    unsigned tbs = std::get<1>(test_case);
    // Get the number of input LLR.
    unsigned nof_llr = std::get<2>(test_case);
    // Get the number of PRB.
    unsigned nof_prb = std::get<3>(test_case);

    unsigned nof_codeblocks = ldpc::compute_nof_codeblocks(units::bits(tbs), cfg.base_graph);

    // Prepare receive data buffer.
    std::vector<uint8_t> data(tbs / 8);

    // The codeword is the concatenation of codeblocks. However, since codeblock sizes can vary slightly, we add some
    // extra margin.
    pool_config.max_codeblock_size   = ldpc::MAX_CODEBLOCK_SIZE;
    pool_config.nof_buffers          = 1;
    pool_config.nof_codeblocks       = nof_codeblocks;
    pool_config.expire_timeout_slots = 10;
    pool_config.external_soft_bits   = ext_softbuffer;

    // Call the hardware-accelerator PUSCH decoder function.
    uint64_t total_hwacc_time = 0;

    // Create Rx softbuffer pool.
    std::unique_ptr<rx_buffer_pool_controller> pool_hwacc = create_rx_buffer_pool(pool_config);
    TESTASSERT(pool_hwacc);

    pusch_decoder::configuration dec_cfg = {};

    // Prepare decoder configuration.
    dec_cfg.new_data            = true;
    dec_cfg.nof_ldpc_iterations = nof_ldpc_iterations;
    dec_cfg.use_early_stop      = use_early_stop;
    dec_cfg.base_graph          = cfg.base_graph;
    dec_cfg.rv                  = cfg.rv;
    dec_cfg.mod                 = cfg.mod;
    dec_cfg.Nref                = cfg.Nref;
    dec_cfg.nof_layers          = cfg.nof_layers;

    // Reserve softbuffer.
    unique_rx_buffer softbuffer_hwacc =
        pool_hwacc->get_pool().reserve({}, trx_buffer_identifier(0, 0), nof_codeblocks, true);
    TESTASSERT(softbuffer_hwacc.is_valid());

    // Force all CRCs to false to test LLR combining.
    softbuffer_hwacc.get().reset_codeblocks_crc();

    // Setup decoder for new data.
    pusch_decoder_notifier_spy decoder_notifier_spy_hwacc;
    uint64_t                   hwacc_start_time = get_current_time();
    pusch_decoder_buffer&      decoder_buffer_hwacc =
        hwacc_decoder->new_data(data, std::move(softbuffer_hwacc), decoder_notifier_spy_hwacc, dec_cfg);

    // Feed codeword.
    decoder_buffer_hwacc.on_new_softbits(span<const log_likelihood_ratio>(random_llrs).subspan(0, nof_llr));
    decoder_buffer_hwacc.on_end_softbits();
    uint64_t hwacc_op_time = get_current_time() - hwacc_start_time;
    total_hwacc_time += hwacc_op_time;

    double hwacc_lat = conv_time_to_latency(total_hwacc_time);

    // Call the software PUSCH decoder function.
    uint64_t total_gen_time = 0;

    // Create Rx softbuffer pool.
    pool_config.external_soft_bits                      = false;
    std::unique_ptr<rx_buffer_pool_controller> pool_gen = create_rx_buffer_pool(pool_config);
    TESTASSERT(pool_gen);

    // Prepare decoder configuration.
    dec_cfg                     = {};
    dec_cfg.new_data            = true;
    dec_cfg.nof_ldpc_iterations = nof_ldpc_iterations;
    dec_cfg.use_early_stop      = use_early_stop;
    dec_cfg.base_graph          = cfg.base_graph;
    dec_cfg.rv                  = cfg.rv;
    dec_cfg.mod                 = cfg.mod;
    dec_cfg.Nref                = cfg.Nref;
    dec_cfg.nof_layers          = cfg.nof_layers;

    // Reserve softbuffer.
    unique_rx_buffer softbuffer_gen =
        pool_gen->get_pool().reserve({}, trx_buffer_identifier(0, 0), nof_codeblocks, true);
    TESTASSERT(softbuffer_gen.is_valid());

    // Force all CRCs to false to test LLR combining.
    softbuffer_gen.get().reset_codeblocks_crc();

    // Setup decoder for new data.
    pusch_decoder_notifier_spy decoder_notifier_spy_gen;
    uint64_t                   gen_start_time = get_current_time();
    pusch_decoder_buffer&      decoder_buffer_gen =
        gen_decoder->new_data(data, std::move(softbuffer_gen), decoder_notifier_spy_gen, dec_cfg);

    // Feed codeword.
    decoder_buffer_gen.on_new_softbits(span<const log_likelihood_ratio>(random_llrs).subspan(0, nof_llr));
    decoder_buffer_gen.on_end_softbits();
    uint64_t gen_op_time = get_current_time() - gen_start_time;
    total_gen_time += gen_op_time;

    double gen_lat = conv_time_to_latency(total_gen_time);

    float perf_gain = 100.0 - (static_cast<float>(hwacc_lat) * 100.0 / static_cast<float>(gen_lat));
    fmt::print(
        "PUSCH RB={:<3} Mod={:<2} tbs={:<8}: latency gain {:<3.2f}%% (generic {:<10.2f} us, {:<5} {:<10.2f} us)\n",
        nof_prb,
        cfg.mod,
        tbs,
        perf_gain,
        gen_lat,
        hwacc_decoder_type,
        hwacc_lat);
  }
}
/// \endcond
