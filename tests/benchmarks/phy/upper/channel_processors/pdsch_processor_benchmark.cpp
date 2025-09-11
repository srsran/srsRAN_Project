/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../../../../unittests/phy/upper/channel_processors/pdsch/pdsch_processor_test_doubles.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/pdsch/factories.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/executors/task_worker_pool.h"
#include "srsran/support/executors/unique_thread.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"
#include "srsran/support/rtsan.h"
#include "srsran/support/srsran_test.h"
#include "srsran/support/tracing/event_tracing.h"
#ifdef HWACC_PDSCH_ENABLED
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_pdsch_enc_factory.h"
#endif // HWACC_PDSCH_ENABLED
#include <condition_variable>
#include <getopt.h>
#include <mutex>
#include <random>

using namespace srsran;

// A test case consists of a PDSCH PDU configuration and a Transport Block Size.
using test_case_type = std::tuple<pdsch_processor::pdu_t, unsigned>;

enum class benchmark_modes : unsigned { silent, latency, throughput_total, throughput_thread, all, invalid };

const char* to_string(benchmark_modes mode)
{
  switch (mode) {
    case benchmark_modes::silent:
      return "silent";
    case benchmark_modes::latency:
      return "latency";
    case benchmark_modes::throughput_total:
      return "throughput_total";
    case benchmark_modes::throughput_thread:
      return "throughput_thread";
    case benchmark_modes::all:
      return "all";
    case benchmark_modes::invalid:
    default:
      return "invalid";
  }
}

benchmark_modes to_benchmark_mode(const char* string)
{
  for (unsigned mode_i = static_cast<unsigned>(benchmark_modes::silent);
       mode_i != static_cast<unsigned>(benchmark_modes::invalid);
       ++mode_i) {
    benchmark_modes mode = static_cast<benchmark_modes>(mode_i);
    if (strcmp(to_string(mode), string) == 0) {
      return mode;
    }
  }
  return benchmark_modes::invalid;
}

// Maximum number of threads given the CPU hardware.
static const unsigned max_nof_threads = std::thread::hardware_concurrency();

// Executor queue type.
static constexpr concurrent_queue_policy queue_policy = concurrent_queue_policy::locking_mpmc;

// General sleeping time.
static constexpr auto sleep_duration = std::chrono::microseconds(10);

// General test configuration parameters.
static uint64_t                           nof_repetitions             = 10;
static uint64_t                           nof_threads                 = max_nof_threads;
static uint64_t                           batch_size_per_thread       = 100;
static std::string                        selected_profile_name       = "default";
static std::string                        ldpc_encoder_type           = "auto";
static std::string                        pdsch_processor_type        = "flexible";
static benchmark_modes                    benchmark_mode              = benchmark_modes::throughput_total;
static std::string                        tracing_filename            = "";
static dmrs_type                          dmrs                        = dmrs_type::TYPE1;
static unsigned                           nof_cdm_groups_without_data = 2;
static bounded_bitset<MAX_NSYMB_PER_SLOT> dmrs_symbol_mask =
    {false, false, true, false, false, false, false, true, false, false, false, true, false, false};
static unsigned                                                 nof_pdsch_processor_concurrent_threads = 0;
static unsigned                                                 cb_batch_length = std::numeric_limits<unsigned>::max();
static std::unique_ptr<task_worker_pool<queue_policy>>          cb_worker_pool  = nullptr;
static std::unique_ptr<task_worker_pool_executor<queue_policy>> cb_executor     = nullptr;
static std::unique_ptr<task_worker_pool<queue_policy>>          pdsch_worker_pool = nullptr;
static std::unique_ptr<task_worker_pool_executor<queue_policy>> pdsch_executor    = nullptr;

#ifdef HWACC_PDSCH_ENABLED
static bool                 dedicated_queue = true;
static bool                 cb_mode         = false;
static bool                 std_out_sink    = true;
static srslog::basic_levels hal_log_level   = srslog::basic_levels::error;
static std::string          eal_arguments   = "";
#endif // HWACC_PDSCH_ENABLED

// Test profile structure, initialized with default profile values.
struct test_profile {
  enum class mimo_topology { one_port_one_layer = 0, two_port_two_layer, four_port_four_layer };

  std::string                      name         = "default";
  std::string                      description  = "Runs all combinations.";
  subcarrier_spacing               scs          = subcarrier_spacing::kHz15;
  std::vector<unsigned>            rv_set       = {0};
  cyclic_prefix                    cp           = cyclic_prefix::NORMAL;
  unsigned                         start_symbol = 2;
  mimo_topology                    mimo         = mimo_topology::one_port_one_layer;
  unsigned                         nof_symbols  = get_nsymb_per_slot(cyclic_prefix::NORMAL) - 2;
  std::vector<unsigned>            nof_prb_set  = {25, 52, 106, 270};
  std::vector<sch_mcs_description> mcs_set      = {{modulation_scheme::QPSK, 120.0F},
                                                   {modulation_scheme::QAM16, 658.0F},
                                                   {modulation_scheme::QAM64, 873.0F},
                                                   {modulation_scheme::QAM256, 948.0F}};
};

// Profile selected during test execution.
static test_profile selected_profile = {};

// Available test profiles.
static const std::vector<test_profile> profile_set = {
    {
        // Use default profile values.
    },

    {"pdsch_scs15_5MHz_qpsk_min",
     "Encodes PDSCH with 5 MHz of bandwidth and a 15 kHz SCS, QPSK modulation at minimum code rate.",
     subcarrier_spacing::kHz15,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::one_port_one_layer,
     12,
     {25},
     {{modulation_scheme::QPSK, 120.0F}}},

    {"pdsch_scs15_5MHz_256qam_max",
     "Encodes PDSCH with 5 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz15,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::one_port_one_layer,
     12,
     {25},
     {{modulation_scheme::QAM256, 948.0F}}},

    {"pdsch_scs15_20MHz_qpsk_min",
     "Encodes PDSCH with 20 MHz of bandwidth and a 15 kHz SCS, QPSK modulation at minimum code rate.",
     subcarrier_spacing::kHz15,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::one_port_one_layer,
     12,
     {106},
     {{modulation_scheme::QPSK, 120.0F}}},

    {"pdsch_scs15_20MHz_16qam_med",
     "Encodes PDSCH with 20 MHz of bandwidth and a 15 kHz SCS, 16-QAM modulation at a medium code rate.",
     subcarrier_spacing::kHz15,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::one_port_one_layer,
     12,
     {106},
     {{modulation_scheme::QAM16, 658.0F}}},

    {"pdsch_scs15_20MHz_64qam_high",
     "Encodes PDSCH with 20 MHz of bandwidth and a 15 kHz SCS, 64-QAM modulation at a high code rate.",
     subcarrier_spacing::kHz15,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::one_port_one_layer,
     12,
     {106},
     {{modulation_scheme::QAM64, 873.0F}}},

    {"pdsch_scs15_20MHz_256qam_max",
     "Encodes PDSCH with 20 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate",
     subcarrier_spacing::kHz15,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::one_port_one_layer,
     12,
     {106},
     {{modulation_scheme::QAM256, 948.0F}}},

    {"pdsch_scs15_50MHz_qpsk_min",
     "Encodes PDSCH with 50 MHz of bandwidth and a 15 kHz SCS, QPSK modulation at minimum code rate.",
     subcarrier_spacing::kHz15,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::one_port_one_layer,
     12,
     {270},
     {{modulation_scheme::QPSK, 120.0F}}},

    {"pdsch_scs15_50MHz_256qam_max",
     "Encodes PDSCH with 50 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz15,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::one_port_one_layer,
     12,
     {270},
     {{modulation_scheme::QAM256, 948.0F}}},

    {"scs30_100MHz_256qam_max",
     "Encodes PDSCH with 100 MHz of bandwidth and a 30 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz30,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::one_port_one_layer,
     12,
     {270},
     {{modulation_scheme::QAM256, 948.0F}}},

    {"2port_2layer_scs30_100MHz_256qam",
     "Encodes 2 layers of PDSCH with 100 MHz of bandwidth and a 30 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz30,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::two_port_two_layer,
     12,
     {270},
     {{modulation_scheme::QAM256, 948.0F}}},

    {"4port_4layer_scs30_100MHz",
     "Encodes 4 layers of PDSCH with 100 MHz of bandwidth and a 30 kHz SCS, 64-QAM and 256-QAM modulation at maximum "
     "code rate.",
     subcarrier_spacing::kHz30,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::four_port_four_layer,
     12,
     {270},
     {{modulation_scheme::QAM64, 948.0F}, {modulation_scheme::QAM256, 948.0F}}},

    {"4port_4layer_scs30_100MHz_256qam",
     "Encodes 4 layers of PDSCH with 100 MHz of bandwidth and a 30 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz30,
     {0},
     cyclic_prefix::NORMAL,
     0,
     test_profile::mimo_topology::four_port_four_layer,
     12,
     {270},
     {{modulation_scheme::QAM256, 948.0F}}},
};

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-m benchmark mode] [-R repetitions] [-B Batch size per thread] [-T number of threads] [-D "
             "LDPC type] [-M rate "
             "matcher type] [-P profile] [-w] [-x] [-y] [-z error|warning|info|debug] [-h] [eal_args ...]\n",
             prog);
  fmt::print("\t-m Benchmark mode. [Default {}]\n", to_string(benchmark_mode));
  fmt::print("\t\t {:<20}It does not print any result.\n", to_string(benchmark_modes::silent));
  fmt::print("\t\t {:<20}Prints the overall average execution time.\n", to_string(benchmark_modes::latency));
  fmt::print("\t\t {:<20}Prints the total aggregated throughput.\n", to_string(benchmark_modes::throughput_total));
  fmt::print("\t\t {:<20}Prints the average single thread throughput.\n",
             to_string(benchmark_modes::throughput_thread));
  fmt::print("\t\t {:<20}Prints all the previous modes.\n", to_string(benchmark_modes::all));
  fmt::print("\t-o Tracing filename [Default {}]\n", tracing_filename.empty() ? "none" : tracing_filename);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-B Batch size [Default {}]\n", batch_size_per_thread);
  fmt::print("\t-T Number of threads [Default {}, max. {}]\n", nof_threads, max_nof_threads);
  fmt::print("\t-D LDPC encoder type. [Default {}]\n", ldpc_encoder_type);
  fmt::print("\t-t PDSCH processor type. [Default {}]\n", pdsch_processor_type);
  fmt::print("\t\t generic        Unoptimized generic implementation.\n");
  fmt::print("\t\t lite           Flexible implementation configured for memory-optimized single thread.\n");
  fmt::print("\t\t flexible:N.M   Flexible implementation configured for performance using N threads and batches of M "
             "codeblocks.\n");
  fmt::print("\t-P Benchmark profile. [Default {}]\n", selected_profile_name);
  for (const test_profile& profile : profile_set) {
    fmt::print("\t\t {:<30}{}\n", profile.name, profile.description);
  }
#ifdef HWACC_PDSCH_ENABLED
  fmt::print("\t-w       Force shared hardware-queue use [Default {}]\n",
             dedicated_queue ? "dedicated_queue" : "shared_queue");
  fmt::print("\t-x       Force TB mode [Default {}]\n", cb_mode ? "cb_mode" : "tb_mode");
  fmt::print("\t-y       Force logging output written to a file [Default {}]\n", std_out_sink ? "std_out" : "file");
  fmt::print("\t-z       Set logging level for the HAL [Default {}]\n", fmt::underlying(hal_log_level));
  fmt::print("\teal_args EAL arguments\n");
#endif // HWACC_PDSCH_ENABLED
  fmt::print("\t-h Show this message\n");
}

#ifdef HWACC_PDSCH_ENABLED
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
#endif // HWACC_PDSCH_ENABLED

static int parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:T:B:D:P:m:t:o:wxyz:h")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'T':
        nof_threads = std::min(max_nof_threads, static_cast<unsigned>(std::strtol(optarg, nullptr, 10)));
        break;
      case 'B':
        batch_size_per_thread = std::strtol(optarg, nullptr, 10);
        break;
      case 'D':
        ldpc_encoder_type = std::string(optarg);
        break;
      case 't':
        pdsch_processor_type = std::string(optarg);
        if ((pdsch_processor_type.find("flexible") != std::string::npos)) {
          std::size_t pos = pdsch_processor_type.find(":");
          if (pos < pdsch_processor_type.size() - 1) {
            std::string str   = pdsch_processor_type.substr(pos + 1);
            std::size_t pos_d = str.find(".");
            if (pos_d < str.size() - 1) {
              std::string substr                     = str.substr(0, pos_d);
              nof_pdsch_processor_concurrent_threads = std::strtol(substr.c_str(), nullptr, 10);
              substr                                 = str.substr(pos_d + 1);
              cb_batch_length                        = std::strtol(substr.c_str(), nullptr, 10);
            } else {
              nof_pdsch_processor_concurrent_threads = std::strtol(str.c_str(), nullptr, 10);
              cb_batch_length                        = 4;
            }
          }
        } else if (pdsch_processor_type == "lite") {
          nof_pdsch_processor_concurrent_threads = 0;
          cb_batch_length                        = std::numeric_limits<unsigned>::max();
        }
        break;
      case 'P':
        selected_profile_name = std::string(optarg);
        break;
      case 'm':
        benchmark_mode = to_benchmark_mode(optarg);
        if (benchmark_mode == benchmark_modes::invalid) {
          fmt::print(stderr, "Invalid benchmark mode '{}'\n", optarg);
          usage(argv[0]);
          return -1;
        }
        break;
      case 'o':
        tracing_filename = std::string(optarg);
        break;
#ifdef HWACC_PDSCH_ENABLED
      case 'w':
        dedicated_queue = false;
        break;
      case 'x':
        cb_mode = true;
        break;
      case 'y':
        std_out_sink = false;
        break;
      case 'z': {
        auto level    = srslog::str_to_basic_level(std::string(optarg));
        hal_log_level = level.has_value() ? level.value() : srslog::basic_levels::error;
        break;
      }
#endif // HWACC_PDSCH_ENABLED
      case 'h':
      default:
        usage(argv[0]);
        std::exit(0);
    }
  }

  // Search profile.
  bool profile_found = false;
  for (const auto& candidate_profile : profile_set) {
    if (selected_profile_name == candidate_profile.name) {
      selected_profile = candidate_profile;
      srslog::fetch_basic_logger("TEST").info("Loading profile: {}", selected_profile.name);
      profile_found = true;
      break;
    }
  }
  if (!profile_found) {
    usage(argv[0]);
    srslog::fetch_basic_logger("TEST").error("Invalid profile: {}.", selected_profile_name);
    fmt::print(stderr, "Invalid profile: {}.\n", selected_profile_name);
    return -1;
  }

  return 0;
}

// Generates a set of test cases given a test profile.
static std::vector<test_case_type> generate_test_cases(const test_profile& profile)
{
  std::vector<test_case_type> test_case_set;

  // Select precoding configuration.
  precoding_configuration precoding_config;
  switch (profile.mimo) {
    case test_profile::mimo_topology::one_port_one_layer:
      precoding_config = precoding_configuration::make_wideband(make_single_port());
      break;
    case test_profile::mimo_topology::two_port_two_layer:
      precoding_config = precoding_configuration::make_wideband(make_two_layer_two_ports(0));
      break;
    case test_profile::mimo_topology::four_port_four_layer:
      precoding_config = precoding_configuration::make_wideband(make_four_layer_four_ports_type1_sp(0, 0));
      break;
  }

  for (sch_mcs_description mcs : profile.mcs_set) {
    for (unsigned nof_prb : profile.nof_prb_set) {
      for (unsigned i_rv : profile.rv_set) {
        // Determine the Transport Block Size.
        tbs_calculator_configuration tbs_config = {};
        tbs_config.mcs_descr                    = mcs;
        tbs_config.n_prb                        = nof_prb;
        tbs_config.nof_layers                   = precoding_config.get_nof_layers();
        tbs_config.nof_symb_sh                  = profile.nof_symbols;
        tbs_config.nof_dmrs_prb = dmrs.nof_dmrs_per_rb() * dmrs_symbol_mask.count() * nof_cdm_groups_without_data;
        unsigned tbs            = tbs_calculator_calculate(tbs_config);

        // Build the PDSCH PDU configuration.
        pdsch_processor::pdu_t config = {
            .context                     = std::nullopt,
            .slot                        = slot_point(to_numerology_value(profile.scs), 0),
            .rnti                        = 1,
            .bwp_size_rb                 = nof_prb,
            .bwp_start_rb                = 0,
            .cp                          = profile.cp,
            .codewords                   = {pdsch_processor::codeword_description{mcs.modulation, i_rv}},
            .n_id                        = 0,
            .ref_point                   = pdsch_processor::pdu_t::CRB0,
            .dmrs_symbol_mask            = dmrs_symbol_mask,
            .dmrs                        = dmrs_type::options::TYPE1,
            .scrambling_id               = 0,
            .n_scid                      = false,
            .nof_cdm_groups_without_data = nof_cdm_groups_without_data,
            .freq_alloc                  = rb_allocation::make_type1(config.bwp_start_rb, nof_prb),
            .start_symbol_index          = profile.start_symbol,
            .nof_symbols                 = profile.nof_symbols,
            .ldpc_base_graph             = get_ldpc_base_graph(mcs.get_normalised_target_code_rate(), units::bits(tbs)),
            .tbs_lbrm                    = units::bits(ldpc::MAX_CODEBLOCK_SIZE).truncate_to_bytes(),
            .reserved                    = {},
            .ptrs                        = std::nullopt,
            .ratio_pdsch_dmrs_to_sss_dB  = 0.0,
            .ratio_pdsch_data_to_sss_dB  = 0.0,
            .precoding                   = precoding_config};
        test_case_set.emplace_back(std::tuple<pdsch_processor::pdu_t, unsigned>(config, tbs));
      }
    }
  }
  return test_case_set;
}

static std::shared_ptr<pdsch_encoder_factory>
create_sw_pdsch_encoder_factory(std::shared_ptr<crc_calculator_factory> crc_calculator_factory)
{
  std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw(ldpc_encoder_type);
  TESTASSERT(ldpc_encoder_factory);

  std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
  TESTASSERT(ldpc_rate_matcher_factory);

  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory =
      create_ldpc_segmenter_tx_factory_sw(crc_calculator_factory);
  TESTASSERT(segmenter_factory);

  pdsch_encoder_factory_sw_configuration encoder_factory_config;
  encoder_factory_config.encoder_factory      = ldpc_encoder_factory;
  encoder_factory_config.rate_matcher_factory = ldpc_rate_matcher_factory;
  encoder_factory_config.segmenter_factory    = segmenter_factory;
  return create_pdsch_encoder_factory_sw(encoder_factory_config);
}

static std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> create_hw_accelerator_pdsch_enc_factory()
{
#ifdef HWACC_PDSCH_ENABLED
  // Intefacing to the bbdev-based hardware-accelerator.
  srslog::basic_logger& logger = srslog::fetch_basic_logger("HWACC", false);
  logger.set_level(hal_log_level);
  unsigned nof_ldpc_enc_cores = nof_threads;
  if (nof_pdsch_processor_concurrent_threads > 0) {
    nof_ldpc_enc_cores *= nof_pdsch_processor_concurrent_threads;
  }
  TESTASSERT(nof_ldpc_enc_cores <= dpdk::MAX_NOF_BBDEV_VF_INSTANCES,
             "Requested {} accelerated LDPC encoder functions, but only {} are supported.",
             nof_ldpc_enc_cores,
             dpdk::MAX_NOF_BBDEV_VF_INSTANCES);
  dpdk::bbdev_acc_configuration bbdev_config;
  bbdev_config.id                                    = 0;
  bbdev_config.nof_ldpc_enc_lcores                   = nof_ldpc_enc_cores;
  bbdev_config.nof_ldpc_dec_lcores                   = 0;
  bbdev_config.nof_fft_lcores                        = 0;
  bbdev_config.nof_mbuf                              = static_cast<unsigned>(pow2(log2_ceil(MAX_NOF_SEGMENTS)));
  std::shared_ptr<dpdk::bbdev_acc> bbdev_accelerator = create_bbdev_acc(bbdev_config, logger);
  TESTASSERT(bbdev_accelerator);

  // Set the PDSCH encoder hardware-accelerator factory configuration for the ACC100.
  hal::bbdev_hwacc_pdsch_enc_factory_configuration hw_encoder_config;
  hw_encoder_config.acc_type          = "acc100";
  hw_encoder_config.bbdev_accelerator = bbdev_accelerator;
  hw_encoder_config.cb_mode           = cb_mode;
  hw_encoder_config.max_tb_size       = RTE_BBDEV_LDPC_E_MAX_MBUF;
  hw_encoder_config.dedicated_queue   = dedicated_queue;

  // ACC100 hardware-accelerator implementation.
  return srsran::hal::create_bbdev_pdsch_enc_acc_factory(hw_encoder_config);
#else  // HWACC_PDSCH_ENABLED
  return nullptr;
#endif // HWACC_PDSCH_ENABLED
}

static std::shared_ptr<pdsch_encoder_factory>
create_acc100_pdsch_encoder_factory(std::shared_ptr<crc_calculator_factory> crc_calculator_factory)
{
  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory =
      create_ldpc_segmenter_tx_factory_sw(crc_calculator_factory);
  TESTASSERT(segmenter_factory);

  std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hw_encoder_factory = create_hw_accelerator_pdsch_enc_factory();
  TESTASSERT(hw_encoder_factory, "Failed to create a HW acceleration encoder factory.");

  // Set the hardware-accelerated PDSCH encoder configuration.
  pdsch_encoder_factory_hw_configuration encoder_hw_factory_config;
  encoder_hw_factory_config.crc_factory        = crc_calculator_factory;
  encoder_hw_factory_config.segmenter_factory  = segmenter_factory;
  encoder_hw_factory_config.hw_encoder_factory = hw_encoder_factory;
  return create_pdsch_encoder_factory_hw(encoder_hw_factory_config);
}

static std::shared_ptr<pdsch_encoder_factory>
create_pdsch_encoder_factory(std::shared_ptr<crc_calculator_factory> crc_calculator_factory)
{
  if (ldpc_encoder_type == "acc100") {
    return create_acc100_pdsch_encoder_factory(crc_calculator_factory);
  }

  return create_sw_pdsch_encoder_factory(crc_calculator_factory);
}

static pdsch_processor_factory& get_processor_factory()
{
  static std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory = nullptr;

  if (pdsch_proc_factory) {
    return *pdsch_proc_factory;
  }

  // Create pseudo-random sequence generator.
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  // Create demodulator mapper factory.
  std::shared_ptr<modulation_mapper_factory> chan_modulation_factory = create_modulation_mapper_factory();
  TESTASSERT(chan_modulation_factory);

  // Create CRC calculator factory.
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calc_factory);

  // Create LDPC encoder factory.
  std::shared_ptr<ldpc_encoder_factory> ldpc_enc_factory = create_ldpc_encoder_factory_sw(ldpc_encoder_type);
  TESTASSERT(ldpc_enc_factory);

  // Create LDPC rate matcher factory.
  std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rm_factory = create_ldpc_rate_matcher_factory_sw();
  TESTASSERT(ldpc_rm_factory);

  // Create LDPC segmenter factory.
  std::shared_ptr<ldpc_segmenter_tx_factory> ldpc_segm_tx_factory =
      create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
  TESTASSERT(ldpc_segm_tx_factory);

  // Create channel precoder factory.
  std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("auto");
  TESTASSERT(precoding_factory);

  // Create resource grid mapper factory.
  std::shared_ptr<resource_grid_mapper_factory> rg_mapper_factory =
      create_resource_grid_mapper_factory(precoding_factory);
  TESTASSERT(rg_mapper_factory);

  // Create DM-RS for PDSCH channel estimator.
  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_gen_factory =
      create_dmrs_pdsch_processor_factory_sw(prg_factory, rg_mapper_factory);
  TESTASSERT(dmrs_pdsch_gen_factory);

  // Create PT-RS for PDSCH channel estimator.
  std::shared_ptr<ptrs_pdsch_generator_factory> ptrs_pdsch_gen_factory =
      create_ptrs_pdsch_generator_generic_factory(prg_factory, rg_mapper_factory);
  TESTASSERT(ptrs_pdsch_gen_factory);

  // Create PDSCH demodulator factory.
  std::shared_ptr<pdsch_modulator_factory> pdsch_mod_factory =
      create_pdsch_modulator_factory_sw(chan_modulation_factory, prg_factory, rg_mapper_factory);
  TESTASSERT(pdsch_mod_factory);

  std::shared_ptr<pdsch_encoder_factory>         pdsch_enc_factory;
  std::shared_ptr<pdsch_block_processor_factory> block_processor_factory;
  if (pdsch_processor_type == "generic") {
    // Create PDSCH encoder factory and generic PDSCH processor.
    pdsch_enc_factory = create_pdsch_encoder_factory(crc_calc_factory);
    TESTASSERT(pdsch_enc_factory);

    pdsch_proc_factory = create_pdsch_processor_factory_sw(
        pdsch_enc_factory, pdsch_mod_factory, dmrs_pdsch_gen_factory, ptrs_pdsch_gen_factory);
    TESTASSERT(pdsch_proc_factory);

    // When required create a synchronous PDSCH processor pool.
    if (nof_threads > 1) {
      pdsch_proc_factory = create_pdsch_processor_pool(std::move(pdsch_proc_factory), nof_threads);
      TESTASSERT(pdsch_proc_factory);
    }

    return *pdsch_proc_factory;
  }

  // Create PDSCH block processor factory.
  if (ldpc_encoder_type != "acc100") {
    block_processor_factory = create_pdsch_block_processor_factory_sw(
        ldpc_enc_factory, ldpc_rm_factory, prg_factory, chan_modulation_factory);
  } else {
    std::shared_ptr<hal::hw_accelerator_pdsch_enc_factory> hw_encoder_factory =
        create_hw_accelerator_pdsch_enc_factory();
    TESTASSERT(hw_encoder_factory, "Failed to create a HW acceleration encoder factory.");

    block_processor_factory =
        create_pdsch_block_processor_factory_hw(hw_encoder_factory, prg_factory, chan_modulation_factory);
  }
  TESTASSERT(block_processor_factory, "Failed to create a PDSCH block processor factory.");

  // Create PDSCH concurrent pool for asynchronous codeblock processing.
  cb_worker_pool = std::make_unique<task_worker_pool<queue_policy>>(
      "pdsch_proc", std::max(1U, nof_pdsch_processor_concurrent_threads), 1024, sleep_duration);
  cb_executor = std::make_unique<task_worker_pool_executor<queue_policy>>(*cb_worker_pool);

  // Initialize task executors for asynchronous PDSCH processors only.
  unsigned nof_concurrent_pdsch = nof_threads;
  if (nof_pdsch_processor_concurrent_threads > 0) {
    nof_concurrent_pdsch += nof_pdsch_processor_concurrent_threads;
  } else {
    cb_worker_pool = std::make_unique<task_worker_pool<queue_policy>>("pdsch_proc", 1, 1024);
  }

  // Create flexible PDSCH processor.
  pdsch_proc_factory = create_pdsch_flexible_processor_factory_sw(ldpc_segm_tx_factory,
                                                                  block_processor_factory,
                                                                  rg_mapper_factory,
                                                                  dmrs_pdsch_gen_factory,
                                                                  ptrs_pdsch_gen_factory,
                                                                  *cb_executor,
                                                                  nof_concurrent_pdsch,
                                                                  cb_batch_length);

  // Wrap the PDSCH processor with a pool. It assumes that each thread will only spawn one PDSCH transmission.
  pdsch_proc_factory = create_pdsch_processor_pool(std::move(pdsch_proc_factory), nof_threads);

  TESTASSERT(pdsch_proc_factory);

  return *pdsch_proc_factory;
}

// Instantiates the PDSCH processor and validator.
static std::unique_ptr<pdsch_pdu_validator> create_validator()
{
  return get_processor_factory().create_validator();
}

// Instantiates the PDSCH processor and validator.
static std::unique_ptr<pdsch_processor> create_processor()
{
  return get_processor_factory().create();
}

// Creates a resource grid.
static std::unique_ptr<resource_grid> create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc)
{
  std::shared_ptr<resource_grid_factory> rg_factory = create_resource_grid_factory();
  TESTASSERT(rg_factory != nullptr, "Invalid resource grid factory.");

  return rg_factory->create(nof_ports, nof_symbols, nof_subc);
}

static bounded_unique_object_pool<resource_grid> create_resource_grid_pool()
{
  std::vector<std::unique_ptr<resource_grid>> grids(nof_threads);
  std::generate(
      grids.begin(), grids.end(), []() { return create_resource_grid(MAX_PORTS, MAX_NSYMB_PER_SLOT, MAX_RB * NRE); });

  return bounded_unique_object_pool<resource_grid>(grids);
}

int main(int argc, char** argv)
{
  // Prepare logging for only errors.
  srslog::init();
  srslog::fetch_basic_logger("PHY").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("TEST").set_level(srslog::basic_levels::warning);
  srslog::fetch_basic_logger("ALL").set_level(srslog::basic_levels::warning);

#ifdef HWACC_PDSCH_ENABLED
  // Separate EAL and non-EAL arguments.
  eal_arguments = capture_eal_args(&argc, &argv);
#endif // HWACC_PDSCH_ENABLED

  int ret = parse_args(argc, argv);
  if (ret < 0) {
    return ret;
  }

  if (!tracing_filename.empty()) {
    open_trace_file(tracing_filename);
  }

#ifdef HWACC_PDSCH_ENABLED
  // Check if we actually need to initialize the EAL.
  static std::unique_ptr<dpdk::dpdk_eal> dpdk_interface = nullptr;
  if (ldpc_encoder_type == "acc100") {
    srslog::sink* log_sink =
        std_out_sink ? srslog::create_stdout_sink() : srslog::create_file_sink("pdsch_processor_benchmark.log");
    srslog::set_default_sink(*log_sink);
    srslog::basic_logger& logger = srslog::fetch_basic_logger("EAL", false);
    logger.set_level(hal_log_level);
    dpdk_interface = dpdk::create_dpdk_eal(eal_arguments, logger);
    TESTASSERT(dpdk_interface, "Failed to open DPDK EAL with arguments.");
  }
#endif // HWACC_PDSCH_ENABLED

  // Inform of the benchmark configuration.
  if (benchmark_mode != benchmark_modes::silent) {
    std::string hwacc_verbose = "";
    if (ldpc_encoder_type == "acc100") {
      hwacc_verbose = fmt::format(" ({} VFs)", nof_threads);
    }
    fmt::print("Launching benchmark for {} threads, {} times per thread, and {} repetitions. Using {} profile, and {} "
               "LDPC encoder{}.\n",
               nof_threads,
               batch_size_per_thread,
               nof_repetitions,
               selected_profile_name,
               ldpc_encoder_type,
               hwacc_verbose);
  }

  benchmarker perf_meas("PDSCH processor", nof_repetitions);

  // Pseudo-random generator.
  std::mt19937 rgen(0);

  // Generate the test cases.
  std::vector<test_case_type> test_case_set = generate_test_cases(selected_profile);

  // Create processor.
  std::unique_ptr<pdsch_processor> proc = create_processor();

  // Create resource grids.
  bounded_unique_object_pool<resource_grid> grids = create_resource_grid_pool();

  // Create concurrent PDSCH processing executors.
  pdsch_worker_pool = std::make_unique<task_worker_pool<queue_policy>>("worker", nof_threads, 1024, sleep_duration);
  pdsch_executor    = std::make_unique<task_worker_pool_executor<queue_policy>>(*pdsch_worker_pool);

  // Prepare PDSCH processor notifiers.
  std::vector<pdsch_processor_notifier_spy> notifiers(nof_threads);

  for (const test_case_type& test_case : test_case_set) {
    // Get the PDSCH configuration.
    pdsch_processor::pdu_t config = std::get<0>(test_case);

    // Get the TBS in bits.
    unsigned tbs = std::get<1>(test_case);

    // Create transport block.
    std::vector<uint8_t> data_vector(tbs / 8);
    std::generate(data_vector.begin(), data_vector.end(), [&rgen]() { return static_cast<uint8_t>(rgen() & 0xff); });

    // Update data reference.
    span<const uint8_t> data = data_vector;

    std::unique_ptr<pdsch_pdu_validator> validator = create_validator();

    // Make sure the configuration is valid.
    TESTASSERT(validator->is_valid(config));

    // Calculate the peak throughput, considering that the number of bits is for a slot.
    double slot_duration_us     = 1e3 / static_cast<double>(pow2(config.slot.numerology()));
    double peak_throughput_Mbps = static_cast<double>(tbs) / slot_duration_us;

    // Measurement description.
    fmt::memory_buffer meas_description;
    fmt::format_to(std::back_inserter(meas_description),
                   "PDSCH RB={:<3} Mod={:<6} rv={} - {:>5.1f} Mbps",
                   config.freq_alloc.get_nof_rb(),
                   to_string(config.codewords.front().modulation),
                   config.codewords.front().rv,
                   peak_throughput_Mbps);

    std::atomic<unsigned> completion_counter = 0;

    // Create benchmark routine.
    auto benchmark_task =
        [&notifiers, &grids, &data, &config, &proc, &completion_counter]() noexcept SRSRAN_RTSAN_NONBLOCKING {
          // Reset counter.
          completion_counter = 0;

          // Spawn tasks for each therad.
          for (unsigned i_thread = 0; i_thread != nof_threads; ++i_thread) {
            // Select notifier.
            pdsch_processor_notifier_spy& notifier = notifiers[i_thread];

            bool success = pdsch_executor->execute(
                [&notifier, &grids, &data, &config, &proc, &completion_counter]() noexcept SRSRAN_RTSAN_NONBLOCKING {
                  // Get a resource grid.
                  auto grid = grids.get();
                  report_fatal_error_if_not(grid, "Failed to retrieve resource grid.");

                  // Repeat PDSCH transmission.
                  for (unsigned i_pdsch = 0; i_pdsch != batch_size_per_thread; ++i_pdsch) {
                    // Reset notifier.
                    notifier.reset();

                    // Process PDSCH transmission.
                    proc->process(grid->get_writer(), notifier, {shared_transport_block(data)}, config);

                    // Wait for notifier before starting next PDSCH transmission.
                    notifier.wait_for_finished();
                  }

                  // Count the completion of the thread.
                  ++completion_counter;
                });

            report_fatal_error_if_not(success, "Failed to execute.");
          }

          // Wait for completion.
          while (completion_counter != nof_threads) {
            std::this_thread::sleep_for(sleep_duration);
          }
        };

    // Run the benchmark.
    perf_meas.new_measure(to_string(meas_description), nof_threads * batch_size_per_thread * tbs, benchmark_task);
  }

  // Print latency.
  if ((benchmark_mode == benchmark_modes::latency) || (benchmark_mode == benchmark_modes::all)) {
    fmt::print("\n--- Average latency ---\n");
    perf_meas.print_percentiles_time("microseconds", 1e-3 / static_cast<double>(nof_threads * batch_size_per_thread));
  }

  // Print total aggregated throughput.
  if ((benchmark_mode == benchmark_modes::throughput_total) || (benchmark_mode == benchmark_modes::all)) {
    fmt::print("\n--- Total throughput ---\n");
    perf_meas.print_percentiles_throughput("bits");
  }

  // Print average throughput per thread.
  if ((benchmark_mode == benchmark_modes::throughput_thread) || (benchmark_mode == benchmark_modes::all)) {
    fmt::print("\n--- Thread throughput ---\n");
    perf_meas.print_percentiles_throughput("bits", 1.0 / static_cast<double>(nof_threads));
  }

  if (cb_worker_pool) {
    cb_worker_pool->stop();
  }

  if (pdsch_worker_pool) {
    pdsch_worker_pool->stop();
  }

  if (!tracing_filename.empty()) {
    close_trace_file();
  }

  return 0;
}
