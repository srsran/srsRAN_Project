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
/// \brief Benchmark for hardware-accelerated PDSCH encoder implementations.
///
/// The benchmark compares the latency of a hardware-accelerated PDSCH encoder implementation to that of the generic
/// one.

#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/support/srsran_test.h"
#include "srsran/support/test_utils.h"
#ifdef DPDK_FOUND
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc_factory.h"
#include <rte_cycles.h>
#endif // DPDK_FOUND
#include <getopt.h>
#include <random>

/// \cond
using namespace srsran;
using namespace srsran::ldpc;

// A test case consists of a PDSCH encoder configuration, a Transport Block size, a codword size and a PRB size.
using test_case_type = std::tuple<pdsch_encoder::configuration, unsigned, unsigned, unsigned>;

static std::string                        hwacc_encoder_type          = "acc100";
static dmrs_type                          dmrs                        = dmrs_type::TYPE1;
static unsigned                           nof_cdm_groups_without_data = 2;
static bounded_bitset<MAX_NSYMB_PER_SLOT> dmrs_symbol_mask =
    {false, false, true, false, false, false, false, false, false, false, false, false, false, false};

#ifdef DPDK_FOUND
static bool        cb_mode       = false;
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
  fmt::print("\t-T       PDSCH encoder type [generic,acc100][Default {}]\n", hwacc_encoder_type);
#ifdef DPDK_FOUND
  fmt::print("\t-x       Force TB mode [Default {}]\n", cb_mode ? "cb_mode" : "tb_mode");
  fmt::print("\t-y       Force logging output written to a file [Default {}]\n", std_out_sink ? "std_out" : "file");
  fmt::print("\t-z       Set logging level for the HAL [Default {}]\n", hal_log_level);
  fmt::print("\teal_args EAL arguments\n");
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
  while ((opt = getopt(argc, argv, "T:xyz:h")) != -1) {
    switch (opt) {
      case 'T':
        hwacc_encoder_type = std::string(optarg);
        break;
#ifdef DPDK_FOUND
      case 'x':
        cb_mode = true;
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

static std::shared_ptr<pdsch_encoder_factory> create_generic_pdsch_encoder_factory()
{
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calc_factory);

  std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw("generic");
  TESTASSERT(ldpc_encoder_factory);

  std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
  TESTASSERT(ldpc_rate_matcher_factory);

  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory = create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
  TESTASSERT(segmenter_factory);

  pdsch_encoder_factory_sw_configuration encoder_factory_config;
  encoder_factory_config.encoder_factory      = ldpc_encoder_factory;
  encoder_factory_config.rate_matcher_factory = ldpc_rate_matcher_factory;
  encoder_factory_config.segmenter_factory    = segmenter_factory;
  return create_pdsch_encoder_factory_sw(encoder_factory_config);
}

static std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> create_hw_accelerator_pdsch_enc_factory()
{
#ifdef DPDK_FOUND
  srslog::sink* log_sink =
      std_out_sink ? srslog::create_stdout_sink() : srslog::create_file_sink("pdsch_encoder_vectortest.log");
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
  bbdev_config.nof_ldpc_enc_lcores                   = 1;
  bbdev_config.nof_ldpc_dec_lcores                   = 0;
  bbdev_config.nof_fft_lcores                        = 0;
  bbdev_config.nof_mbuf                              = static_cast<unsigned>(pow2(log2_ceil(MAX_NOF_SEGMENTS)));
  std::shared_ptr<dpdk::bbdev_acc> bbdev_accelerator = create_bbdev_acc(bbdev_config, logger);
  TESTASSERT(bbdev_accelerator);

  // Set the hardware-accelerator configuration.
  hw_accelerator_pdsch_enc_configuration hw_encoder_config;
  hw_encoder_config.acc_type          = "acc100";
  hw_encoder_config.bbdev_accelerator = bbdev_accelerator;

  // ACC100 hardware-accelerator implementation.
  return create_hw_accelerator_pdsch_enc_factory(hw_encoder_config);
#else  // DPDK_FOUND
  return nullptr;
#endif // DPDK_FOUND
}

static std::shared_ptr<pdsch_encoder_factory> create_acc100_pdsch_encoder_factory()
{
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calc_factory);

  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory = create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
  TESTASSERT(segmenter_factory);

  std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hw_encoder_factory = create_hw_accelerator_pdsch_enc_factory();
  TESTASSERT(hw_encoder_factory, "Failed to create a HW acceleration encoder factory.");

  // Set the hardware-accelerated PDSCH encoder configuration.
  pdsch_encoder_factory_hw_configuration encoder_hw_factory_config;
#ifdef DPDK_FOUND
  encoder_hw_factory_config.cb_mode     = cb_mode;
  encoder_hw_factory_config.max_tb_size = RTE_BBDEV_LDPC_E_MAX_MBUF;
#endif // DPDK_FOUND
  encoder_hw_factory_config.crc_factory        = crc_calc_factory;
  encoder_hw_factory_config.segmenter_factory  = segmenter_factory;
  encoder_hw_factory_config.hw_encoder_factory = hw_encoder_factory;
  return create_pdsch_encoder_factory_hw(encoder_hw_factory_config);
}

static std::shared_ptr<pdsch_encoder_factory> create_pdsch_encoder_factory(std::string encoder_type)
{
  if (encoder_type == "generic") {
    return create_generic_pdsch_encoder_factory();
  }

  if (encoder_type == "acc100") {
    return create_acc100_pdsch_encoder_factory();
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

      // Build the PDSCH encoder configuration.
      pdsch_encoder::configuration config;
      config.base_graph     = get_ldpc_base_graph(mcs.get_normalised_target_code_rate(), units::bits(tbs));
      config.rv             = 0;
      config.mod            = mcs.modulation;
      config.Nref           = 0;
      config.nof_layers     = profile.nof_tx_layers;
      config.nof_ch_symbols = profile.nof_symbols * nof_prb * NRE;

      // Codeword size.
      unsigned bits_per_symbol = get_bits_per_symbol(mcs.modulation);
      unsigned cw_length       = config.nof_ch_symbols * bits_per_symbol * profile.nof_tx_layers;

      test_case_set.emplace_back(
          std::tuple<pdsch_encoder::configuration, unsigned, unsigned, unsigned>(config, tbs, cw_length, nof_prb));
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

  // Create the generic PDSCH encoder against which to benchmark the hardware-accelerated PDSCH encoder.
  std::shared_ptr<pdsch_encoder_factory> gen_pdsch_enc_factory = create_pdsch_encoder_factory("generic");
  TESTASSERT(gen_pdsch_enc_factory, "Failed to create PDSCH encoder factory of type {}.", "generic");

  std::unique_ptr<pdsch_encoder> gen_encoder = gen_pdsch_enc_factory->create();
  TESTASSERT(gen_encoder);

  // Create the hardware-accelerated PDSCH encoder.
  std::shared_ptr<pdsch_encoder_factory> hwacc_pdsch_enc_factory = create_pdsch_encoder_factory(hwacc_encoder_type);
  TESTASSERT(hwacc_pdsch_enc_factory,
             "Failed to create hardware-accelerated PDSCH encoder factory of type {}.",
             hwacc_encoder_type);

  std::unique_ptr<pdsch_encoder> hwacc_encoder = hwacc_pdsch_enc_factory->create();
  TESTASSERT(hwacc_encoder);

  // Generate the test cases.
  std::vector<test_case_type> test_case_set = generate_test_cases(selected_profile);

  fmt::print("Launching benchmark comparing generic and {} PDSCH encoder implementations\n\n", hwacc_encoder_type);

  for (const test_case_type& test_case : test_case_set) {
    // Get the PDSCH encoder configuration.
    const pdsch_encoder::configuration cfg = std::get<0>(test_case);
    // Get the TBS in bits.
    unsigned tbs = std::get<1>(test_case);
    // Get the codeword length.
    unsigned cw_length = std::get<2>(test_case);
    // Get the number of PRB.
    unsigned nof_prb = std::get<3>(test_case);

    // Create transport block.
    std::vector<uint8_t> data(tbs / 8);
    std::generate(data.begin(), data.end(), [&rgen]() { return static_cast<uint8_t>(rgen() & 0xff); });

    // Prepare codeword buffer.
    std::vector<uint8_t> codeword(cw_length);

    // Call the hardware-accelerator PDSCH encoder function.
    uint64_t total_hwacc_time = 0;

    // Encode the TB.
    uint64_t hwacc_start_time = get_current_time();
    hwacc_encoder->encode(codeword, data, cfg);
    uint64_t hwacc_op_time = get_current_time() - hwacc_start_time;
    total_hwacc_time += hwacc_op_time;

    double hwacc_lat = conv_time_to_latency(total_hwacc_time);

    // Call the software PDSCH encoder function.
    uint64_t total_gen_time = 0;

    // Encode the TB.
    uint64_t gen_start_time = get_current_time();
    gen_encoder->encode(codeword, data, cfg);
    uint64_t gen_op_time = get_current_time() - gen_start_time;
    total_gen_time += gen_op_time;

    double gen_lat = conv_time_to_latency(total_gen_time);

    float perf_gain = 100.0 - (static_cast<float>(hwacc_lat) * 100.0 / static_cast<float>(gen_lat));
    fmt::print(
        "PDSCH RB={:<3} Mod={:<2} tbs={:<8}: latency gain {:<3.2f}%% (generic {:<10.2f} us, {:<5} {:<10.2f} us)\n",
        nof_prb,
        cfg.mod,
        tbs,
        perf_gain,
        gen_lat,
        hwacc_encoder_type,
        hwacc_lat);
  }
}
/// \endcond
