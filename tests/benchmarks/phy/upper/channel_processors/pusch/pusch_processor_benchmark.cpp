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

#include "../../../lib/phy/upper/rx_buffer_pool_impl.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/ran/sch/tbs_calculator.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/complex_normal_random.h"
#include "srsran/support/executors/task_worker_pool.h"
#include "srsran/support/math_utils.h"
#include "srsran/support/srsran_test.h"
#include "srsran/support/unique_thread.h"
#ifdef HWACC_PUSCH_ENABLED
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "srsran/hal/dpdk/dpdk_eal_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_pusch_dec_factory.h"
#endif // HWACC_PUSCH_ENABLED
#include <getopt.h>
#include <random>

using namespace srsran;

// A test case consists of a PUSCH PDU configuration and a Transport Block Size.
using test_case_type = std::tuple<pusch_processor::pdu_t, unsigned>;

namespace {

enum class benchmark_modes : unsigned { silent, latency, throughput_total, throughput_thread, all, invalid };

class pusch_processor_result_notifier_adaptor : public pusch_processor_result_notifier
{
public:
  void on_uci(const pusch_processor_result_control& uci) override {}

  void on_sch(const pusch_processor_result_data& sch) override { completed = true; }

  void reset() { completed = false; }

  void wait_for_completion()
  {
    while (!completed.load()) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
  }

private:
  std::atomic<bool> completed = {false};
};

} // namespace

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

// General test configuration parameters.
static uint64_t                           nof_repetitions             = 10;
static uint64_t                           nof_threads                 = max_nof_threads;
static uint64_t                           batch_size_per_thread       = 100;
static std::string                        selected_profile_name       = "default";
static std::string                        ldpc_decoder_type           = "auto";
static std::string                        rate_dematcher_type         = "auto";
static bool                               enable_evm                  = false;
static benchmark_modes                    benchmark_mode              = benchmark_modes::throughput_total;
static unsigned                           nof_rx_ports                = 1;
static constexpr unsigned                 max_nof_rx_ports            = 4;
static unsigned                           nof_tx_layers               = 1;
static unsigned                           nof_harq_ack                = 0;
static unsigned                           nof_csi_part1               = 0;
static unsigned                           nof_csi_part2               = 0;
static dmrs_type                          dmrs                        = dmrs_type::TYPE1;
static unsigned                           nof_cdm_groups_without_data = 2;
static bounded_bitset<MAX_NSYMB_PER_SLOT> dmrs_symbol_mask =
    {false, false, true, false, false, false, false, false, false, false, false, false, false, false};
static unsigned                                                                          nof_pusch_decoder_threads = 8;
static std::unique_ptr<task_worker_pool<concurrent_queue_policy::locking_mpmc>>          worker_pool = nullptr;
static std::unique_ptr<task_worker_pool_executor<concurrent_queue_policy::locking_mpmc>> executor    = nullptr;

// Thread shared variables.
static std::atomic<bool>     thread_quit   = {};
static std::atomic<int>      pending_count = {0};
static std::atomic<unsigned> finish_count  = {0};

#ifdef HWACC_PUSCH_ENABLED
static bool        ext_softbuffer = true;
static bool        std_out_sink   = true;
static std::string hal_log_level  = "error";
static std::string eal_arguments  = "";
#endif // HWACC_PUSCH_ENABLED

// Test profile structure, initialized with default profile values.
struct test_profile {
  std::string                      name        = "default";
  std::string                      description = "Runs all combinations.";
  subcarrier_spacing               scs         = subcarrier_spacing::kHz15;
  cyclic_prefix                    cp          = cyclic_prefix::NORMAL;
  unsigned                         nof_symbols = get_nsymb_per_slot(cyclic_prefix::NORMAL);
  std::vector<unsigned>            nof_prb_set = {25, 52, 106, 270};
  std::vector<sch_mcs_description> mcs_set     = {{modulation_scheme::QPSK, 120.0F},
                                                  {modulation_scheme::QAM16, 658.0F},
                                                  {modulation_scheme::QAM64, 873.0F},
                                                  {modulation_scheme::QAM256, 948.0F}};
  std::vector<unsigned>            rv_set      = {0};
};

// Profile selected during test execution.
static test_profile selected_profile = {};

// Available test profiles.
static const std::vector<test_profile> profile_set = {
    {
        // Use default profile values.
    },

    {"pusch_scs15_5MHz_qpsk_min",
     "Decodes PUSCH with 5 MHz of bandwidth and a 15 kHz SCS, QPSK modulation at minimum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {25},
     {{modulation_scheme::QPSK, 120.0F}},
     {0}},

    {"pusch_scs15_5MHz_256qam_max",
     "Decodes PUSCH with 5 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {25},
     {{modulation_scheme::QAM256, 948.0F}},
     {0}},

    {"pusch_scs15_20MHz_qpsk_min",
     "Decodes PUSCH with 20 MHz of bandwidth and a 15 kHz SCS, QPSK modulation at minimum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {106},
     {{modulation_scheme::QPSK, 120.0F}},
     {0}},

    {"pusch_scs15_20MHz_16qam_med",
     "Decodes PUSCH with 20 MHz of bandwidth and a 15 kHz SCS, 16-QAM modulation at a medium code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {106},
     {{modulation_scheme::QAM16, 658.0F}},
     {0}},

    {"pusch_scs15_50MHz_64qam_max",
     "Decodes PUSCH with 50 MHz of bandwidth and a 15 kHz SCS, 64-QAM modulation at a high code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {270},
     {{modulation_scheme::QAM64, 873.0F}},
     {0}},

    {"pusch_scs15_20MHz_256qam_max",
     "Decodes PUSCH with 20 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {106},
     {{modulation_scheme::QAM256, 948.0F}},
     {0}},

    {"pusch_scs15_50MHz_qpsk_min",
     "Decodes PUSCH with 50 MHz of bandwidth and a 15 kHz SCS, QPSK modulation at minimum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {270},
     {{modulation_scheme::QPSK, 120.0F}},
     {0}},

    {"pusch_scs15_50MHz_256qam_max",
     "Decodes PUSCH with 50 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {270},
     {{modulation_scheme::QAM256, 948.0F}},
     {0}},

    {"pusch_scs30_100MHz_256qam_max",
     "Decodes PUSCH with 100 MHz of bandwidth and a 30 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz30,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {273},
     {{modulation_scheme::QAM256, 948.0F}},
     {0, 2}},
};

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-m benchmark mode] [-R repetitions] [-B Batch size per thread] [-T number of threads] [-D "
             "LDPC type] [-M rate "
             "dematcher type] [-P profile] [-x] [-y] [-z error|warning|info|debug] [-h] [eal_args ...]\n",
             prog);
  fmt::print("\t-m Benchmark mode. [Default {}]\n", to_string(benchmark_mode));
  fmt::print("\t\t {:<20}It does not print any result.\n", to_string(benchmark_modes::silent));
  fmt::print("\t\t {:<20}Prints the overall average execution time.\n", to_string(benchmark_modes::latency));
  fmt::print("\t\t {:<20}Prints the total aggregated throughput.\n", to_string(benchmark_modes::throughput_total));
  fmt::print("\t\t {:<20}Prints the average single thread throughput.\n",
             to_string(benchmark_modes::throughput_thread));
  fmt::print("\t\t {:<20}Prints all the previous modes.\n", to_string(benchmark_modes::all));
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-B Batch size [Default {}]\n", batch_size_per_thread);
  fmt::print("\t-T Number of threads [Default {}, max. {}]\n", nof_threads, max_nof_threads);
  fmt::print("\t-t Number of concurrent PUSCH decoder threads. Set to zero for no concurrency. [Default {}, max. {}]\n",
             nof_pusch_decoder_threads,
             max_nof_threads);
  fmt::print("\t-D LDPC decoder type. [Default {}]\n", ldpc_decoder_type);
  fmt::print("\t-p Number of RX ports. [Default {}, max. {}]\n", nof_rx_ports, max_nof_rx_ports);
  fmt::print("\t-M Rate dematcher type. [Default {}]\n", rate_dematcher_type);
  fmt::print("\t-E Toggle EVM enable/disable. [Default {}]\n", enable_evm ? "enable" : "disable");
  fmt::print("\t-P Benchmark profile. [Default {}]\n", selected_profile_name);
  for (const test_profile& profile : profile_set) {
    fmt::print("\t\t {:<30}{}\n", profile.name, profile.description);
  }
#ifdef HWACC_PUSCH_ENABLED
  fmt::print("\t-x       Use the host's memory for the soft-buffer [Default {}]\n", !ext_softbuffer);
  fmt::print("\t-y       Force logging output written to a file [Default {}]\n", std_out_sink ? "std_out" : "file");
  fmt::print("\t-z       Set logging level for the HAL [Default {}]\n", hal_log_level);
  fmt::print("\teal_args EAL arguments\n");
#endif // HWACC_PUSCH_ENABLED

  fmt::print("\t-h Show this message\n");
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

static int parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:T:t:p:B:D:M:EP:m:xyz:h")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'T':
        nof_threads = std::min(max_nof_threads, static_cast<unsigned>(std::strtol(optarg, nullptr, 10)));
        break;
      case 't':
        nof_pusch_decoder_threads = std::min(max_nof_threads, static_cast<unsigned>(std::strtol(optarg, nullptr, 10)));
        break;
      case 'p':
        nof_rx_ports = std::min(max_nof_rx_ports, static_cast<unsigned>(std::strtol(optarg, nullptr, 10)));
        break;
      case 'B':
        batch_size_per_thread = std::strtol(optarg, nullptr, 10);
        break;
      case 'D':
        ldpc_decoder_type = std::string(optarg);
        break;
      case 'M':
        rate_dematcher_type = std::string(optarg);
        break;
      case 'E':
        enable_evm = !enable_evm;
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
        exit(0);
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

  for (sch_mcs_description mcs : profile.mcs_set) {
    for (unsigned nof_prb : profile.nof_prb_set) {
      for (unsigned rv : profile.rv_set) {
        // Determine the Transport Block Size.
        tbs_calculator_configuration tbs_config = {};
        tbs_config.mcs_descr                    = mcs;
        tbs_config.n_prb                        = nof_prb;
        tbs_config.nof_layers                   = nof_tx_layers;
        tbs_config.nof_symb_sh                  = profile.nof_symbols;
        tbs_config.nof_dmrs_prb = dmrs.nof_dmrs_per_rb() * dmrs_symbol_mask.count() * nof_cdm_groups_without_data;
        unsigned tbs            = tbs_calculator_calculate(tbs_config);

        // Build the PUSCH PDU configuration.
        pusch_processor::pdu_t config = {};
        config.slot                   = slot_point(to_numerology_value(profile.scs), 0);
        config.rnti                   = 1;
        config.bwp_size_rb            = nof_prb;
        config.bwp_start_rb           = 0;
        config.cp                     = profile.cp;
        config.mcs_descr              = mcs;
        config.codeword.emplace(pusch_processor::codeword_description{
            rv, get_ldpc_base_graph(mcs.get_normalised_target_code_rate(), units::bits(tbs)), true});
        config.uci.alpha_scaling         = 1.0;
        config.uci.beta_offset_harq_ack  = 5.0;
        config.uci.beta_offset_csi_part1 = 5.0;
        config.uci.beta_offset_csi_part2 = 5.0;
        config.uci.nof_harq_ack          = nof_harq_ack;
        config.uci.nof_csi_part1         = nof_csi_part1;
        config.uci.csi_part2_size        = uci_part2_size_description(nof_csi_part2);
        config.n_id                      = 0;
        config.nof_tx_layers             = nof_tx_layers;
        config.rx_ports.resize(nof_rx_ports);
        std::iota(config.rx_ports.begin(), config.rx_ports.end(), 0);
        config.dmrs_symbol_mask            = dmrs_symbol_mask;
        config.dmrs                        = dmrs;
        config.scrambling_id               = 0;
        config.nof_cdm_groups_without_data = nof_cdm_groups_without_data;
        config.freq_alloc                  = rb_allocation::make_type1(config.bwp_start_rb, nof_prb);
        config.start_symbol_index          = 0;
        config.nof_symbols                 = profile.nof_symbols;
        config.tbs_lbrm_bytes              = ldpc::MAX_CODEBLOCK_SIZE / 8;

        test_case_set.emplace_back(std::tuple<pusch_processor::pdu_t, unsigned>(config, tbs));
      }
    }
  }
  return test_case_set;
}

static std::shared_ptr<pusch_decoder_factory>
create_sw_pusch_decoder_factory(std::shared_ptr<crc_calculator_factory> crc_calculator_factory)
{
  std::shared_ptr<ldpc_decoder_factory> ldpc_decoder_factory = create_ldpc_decoder_factory_sw(ldpc_decoder_type);
  TESTASSERT(ldpc_decoder_factory);

  std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rate_dematcher_factory =
      create_ldpc_rate_dematcher_factory_sw(rate_dematcher_type);
  TESTASSERT(ldpc_rate_dematcher_factory);

  std::shared_ptr<ldpc_segmenter_rx_factory> segmenter_rx_factory = create_ldpc_segmenter_rx_factory_sw();
  TESTASSERT(segmenter_rx_factory);

  pusch_decoder_factory_sw_configuration pusch_decoder_factory_sw_config;
  pusch_decoder_factory_sw_config.crc_factory               = crc_calculator_factory;
  pusch_decoder_factory_sw_config.decoder_factory           = ldpc_decoder_factory;
  pusch_decoder_factory_sw_config.dematcher_factory         = ldpc_rate_dematcher_factory;
  pusch_decoder_factory_sw_config.segmenter_factory         = segmenter_rx_factory;
  pusch_decoder_factory_sw_config.nof_pusch_decoder_threads = nof_threads + nof_pusch_decoder_threads + 1;
  pusch_decoder_factory_sw_config.executor                  = executor.get();
  pusch_decoder_factory_sw_config.nof_prb                   = MAX_RB;
  pusch_decoder_factory_sw_config.nof_layers                = pusch_constants::MAX_NOF_LAYERS;

  return create_pusch_decoder_factory_sw(pusch_decoder_factory_sw_config);
}

static std::shared_ptr<hal::hw_accelerator_pusch_dec_factory> create_hw_accelerator_pusch_dec_factory()
{
#ifdef HWACC_PUSCH_ENABLED
  // Intefacing to the bbdev-based hardware-accelerator.
  srslog::basic_logger& logger = srslog::fetch_basic_logger("HWACC", false);
  logger.set_level(srslog::str_to_basic_level(hal_log_level));
  dpdk::bbdev_acc_configuration bbdev_config;
  bbdev_config.id                                    = 0;
  bbdev_config.nof_ldpc_enc_lcores                   = 0;
  bbdev_config.nof_ldpc_dec_lcores                   = nof_threads;
  bbdev_config.nof_fft_lcores                        = 0;
  bbdev_config.nof_mbuf                              = static_cast<unsigned>(pow2(log2_ceil(MAX_NOF_SEGMENTS)));
  std::shared_ptr<dpdk::bbdev_acc> bbdev_accelerator = create_bbdev_acc(bbdev_config, logger);
  TESTASSERT(bbdev_accelerator);

  // Interfacing to a shared external HARQ buffer context repository.
  unsigned nof_cbs                   = MAX_NOF_SEGMENTS;
  uint64_t acc100_ext_harq_buff_size = bbdev_accelerator->get_harq_buff_size_bytes();
  std::shared_ptr<ext_harq_buffer_context_repository> harq_buffer_context =
      create_ext_harq_buffer_context_repository(nof_cbs, acc100_ext_harq_buff_size, false);
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

static std::shared_ptr<pusch_decoder_factory>
create_acc100_pusch_decoder_factory(std::shared_ptr<crc_calculator_factory> crc_calculator_factory)
{
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

static std::shared_ptr<pusch_decoder_factory>
create_pusch_decoder_factory(std::shared_ptr<crc_calculator_factory> crc_calculator_factory)
{
  if (ldpc_decoder_type == "acc100") {
    return create_acc100_pusch_decoder_factory(crc_calculator_factory);
  }

  return create_sw_pusch_decoder_factory(crc_calculator_factory);
}

static pusch_processor_factory& get_pusch_processor_factory()
{
  static std::shared_ptr<pusch_processor_factory> pusch_proc_factory = nullptr;

  if (pusch_proc_factory) {
    return *pusch_proc_factory;
  }

  // Create pseudo-random sequence generator.
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  // Create demodulator mapper factory.
  std::shared_ptr<channel_modulation_factory> chan_modulation_factory = create_channel_modulation_sw_factory();
  TESTASSERT(chan_modulation_factory);

  // Create CRC calculator factory.
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calc_factory);

  std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
  TESTASSERT(short_block_det_factory);

  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_fast();
  if (!dft_factory) {
    dft_factory = create_dft_processor_factory_generic();
  }
  TESTASSERT(dft_factory, "Cannot create DFT factory.");

  std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory =
      create_time_alignment_estimator_dft_factory(dft_factory);
  TESTASSERT(ta_estimator_factory, "Cannot create TA estimator factory.");

  // Create port channel estimator factory.
  std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
      create_port_channel_estimator_factory_sw(ta_estimator_factory);
  TESTASSERT(port_chan_estimator_factory);

  // Create DM-RS for PUSCH channel estimator.
  std::shared_ptr<dmrs_pusch_estimator_factory> dmrs_pusch_chan_estimator_factory =
      create_dmrs_pusch_estimator_factory_sw(prg_factory, port_chan_estimator_factory);
  TESTASSERT(dmrs_pusch_chan_estimator_factory);

  // Create channel equalizer factory.
  std::shared_ptr<channel_equalizer_factory> eq_factory = create_channel_equalizer_factory_zf();
  TESTASSERT(eq_factory);

  // Create PUSCH demodulator factory.
  std::shared_ptr<pusch_demodulator_factory> pusch_demod_factory =
      create_pusch_demodulator_factory_sw(eq_factory, chan_modulation_factory, prg_factory, enable_evm);
  TESTASSERT(pusch_demod_factory);

  // Create PUSCH demultiplexer factory.
  std::shared_ptr<ulsch_demultiplex_factory> demux_factory = create_ulsch_demultiplex_factory_sw();
  TESTASSERT(demux_factory);

  // Create worker pool and exectuors for concurrent PUSCH processor implementations.
  // Note that currently hardware-acceleration is limited to "generic" processor types.
  if (nof_pusch_decoder_threads != 0 && ldpc_decoder_type != "acc100" && rate_dematcher_type != "acc100") {
    worker_pool = std::make_unique<task_worker_pool<concurrent_queue_policy::locking_mpmc>>(
        nof_pusch_decoder_threads, 1024, "decoder");
    executor = std::make_unique<task_worker_pool_executor<concurrent_queue_policy::locking_mpmc>>(*worker_pool);
  }

  // Create PUSCH decoder factory.
  std::shared_ptr<pusch_decoder_factory> pusch_dec_factory = create_pusch_decoder_factory(crc_calc_factory);
  TESTASSERT(pusch_dec_factory);

  // Create polar decoder factory.
  std::shared_ptr<polar_factory> polar_dec_factory = create_polar_factory_sw();
  report_fatal_error_if_not(polar_dec_factory, "Invalid polar decoder factory.");

  // Create UCI decoder factory.
  std::shared_ptr<uci_decoder_factory> uci_dec_factory =
      create_uci_decoder_factory_generic(short_block_det_factory, polar_dec_factory, crc_calc_factory);
  TESTASSERT(uci_dec_factory);

  // Create PUSCH processor.
  pusch_processor_factory_sw_configuration pusch_proc_factory_config;
  pusch_proc_factory_config.estimator_factory                    = dmrs_pusch_chan_estimator_factory;
  pusch_proc_factory_config.demodulator_factory                  = pusch_demod_factory;
  pusch_proc_factory_config.demux_factory                        = demux_factory;
  pusch_proc_factory_config.decoder_factory                      = pusch_dec_factory;
  pusch_proc_factory_config.uci_dec_factory                      = uci_dec_factory;
  pusch_proc_factory_config.ch_estimate_dimensions.nof_prb       = MAX_RB;
  pusch_proc_factory_config.ch_estimate_dimensions.nof_symbols   = MAX_NSYMB_PER_SLOT;
  pusch_proc_factory_config.ch_estimate_dimensions.nof_rx_ports  = nof_rx_ports;
  pusch_proc_factory_config.ch_estimate_dimensions.nof_tx_layers = nof_tx_layers;
  pusch_proc_factory_config.dec_nof_iterations                   = 2;
  pusch_proc_factory_config.dec_enable_early_stop                = true;
  pusch_proc_factory_config.max_nof_concurrent_threads           = nof_threads;
  pusch_proc_factory = create_pusch_processor_factory_sw(pusch_proc_factory_config);
  TESTASSERT(pusch_proc_factory);

  pusch_proc_factory = create_pusch_processor_pool(std::move(pusch_proc_factory), nof_threads, true);
  TESTASSERT(pusch_proc_factory);

  return *pusch_proc_factory;
}

// Instantiates the PUSCH processor and validator.
static std::tuple<std::unique_ptr<pusch_processor>, std::unique_ptr<pusch_pdu_validator>> create_processor()
{
  pusch_processor_factory& pusch_proc_factory = get_pusch_processor_factory();

  // Create PUSCH processor.
  std::unique_ptr<pusch_processor> processor = pusch_proc_factory.create();
  TESTASSERT(processor);

  // Create PUSCH processor validator.
  std::unique_ptr<pusch_pdu_validator> validator = pusch_proc_factory.create_validator();
  TESTASSERT(validator);

  return std::make_tuple(std::move(processor), std::move(validator));
}

static void thread_process(pusch_processor&              proc,
                           const pusch_processor::pdu_t& config,
                           unsigned                      tbs,
                           const resource_grid_reader&   grid)
{
  // Compute the number of codeblocks.
  unsigned nof_codeblocks = ldpc::compute_nof_codeblocks(units::bits(tbs), config.codeword.value().ldpc_base_graph);

  // Buffer pool configuration.
  rx_buffer_pool_config buffer_pool_config = {};
  buffer_pool_config.nof_buffers           = 1;
  buffer_pool_config.nof_codeblocks        = nof_codeblocks;
  buffer_pool_config.max_codeblock_size    = ldpc::MAX_CODEBLOCK_SIZE;
  buffer_pool_config.expire_timeout_slots =
      100 * get_nof_slots_per_subframe(to_subcarrier_spacing(config.slot.numerology()));
  buffer_pool_config.external_soft_bits = false;

  trx_buffer_identifier buffer_id = trx_buffer_identifier(config.rnti, 0);

  // Create buffer pool.
  std::unique_ptr<rx_buffer_pool_controller> buffer_pool = create_rx_buffer_pool(buffer_pool_config);

  // Prepare receive data buffer.
  std::vector<uint8_t> data(tbs / 8);

  while (!thread_quit) {
    // If the pending count is equal to or lower than zero, wait for a new start.
    while (pending_count.fetch_sub(1) <= 0) {
      // Wait for pending to non-negative.
      while (pending_count.load() <= 0) {
        // Sleep.
        std::this_thread::sleep_for(std::chrono::microseconds(10));

        // Quit if signaled.
        if (thread_quit) {
          return;
        }
      }
    }

    // Prepare notifier.
    pusch_processor_result_notifier_adaptor result_notifier;

    // Reserve buffer.
    unique_rx_buffer rm_buffer = buffer_pool->get_pool().reserve(config.slot, buffer_id, nof_codeblocks, true);

    // Process PDU.
    proc.process(data, std::move(rm_buffer), result_notifier, grid, config);

    // Wait for finish the task.
    result_notifier.wait_for_completion();

    // Notify finish count.
    ++finish_count;
  }
}

// Creates a resource grid.
static std::unique_ptr<resource_grid> create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc)
{
  std::shared_ptr<channel_precoder_factory> precoding_factory = create_channel_precoder_factory("generic");
  TESTASSERT(precoding_factory != nullptr, "Invalid channel precoder factory.");
  std::shared_ptr<resource_grid_factory> rg_factory = create_resource_grid_factory(precoding_factory);
  TESTASSERT(rg_factory != nullptr, "Invalid resource grid factory.");

  return rg_factory->create(nof_ports, nof_symbols, nof_subc);
}

int main(int argc, char** argv)
{
#ifdef HWACC_PUSCH_ENABLED
  // Separate EAL and non-EAL arguments.
  eal_arguments = capture_eal_args(&argc, &argv);
#endif // HWACC_PUSCH_ENABLED

  int ret = parse_args(argc, argv);
  if (ret < 0) {
    return ret;
  }

#ifdef HWACC_PUSCH_ENABLED
  // Check if we actually need to initialize the EAL.
  static std::unique_ptr<dpdk::dpdk_eal> dpdk_interface = nullptr;
  if (ldpc_decoder_type == "acc100" || rate_dematcher_type == "acc100") {
    ldpc_decoder_type   = "acc100";
    rate_dematcher_type = "acc100";
    srslog::sink* log_sink =
        std_out_sink ? srslog::create_stdout_sink() : srslog::create_file_sink("pusch_processor_benchmark.log");
    srslog::set_default_sink(*log_sink);
    srslog::init();
    srslog::basic_logger& logger = srslog::fetch_basic_logger("EAL", false);
    logger.set_level(srslog::str_to_basic_level(hal_log_level));
    dpdk_interface = dpdk::create_dpdk_eal(eal_arguments, logger);
    TESTASSERT(dpdk_interface, "Failed to open DPDK EAL with arguments.");
  }
#endif // HWACC_PUSCH_ENABLED

  // Inform of the benchmark configuration.
  if (benchmark_mode != benchmark_modes::silent) {
    fmt::print("Launching benchmark for {} threads, {} times per thread, and {} repetitions. Using {} profile, {} LDPC "
               "decoder, and {} rate dematcher.\n",
               nof_threads,
               batch_size_per_thread,
               nof_repetitions,
               selected_profile_name,
               ldpc_decoder_type,
               rate_dematcher_type);
  }

  benchmarker perf_meas("PUSCH processor", nof_repetitions);

  // Pseudo-random generator.
  std::mt19937 rgen(0);

  // Grid dimensions for all test cases.
  unsigned grid_nof_symbols = get_nsymb_per_slot(selected_profile.cp);
  unsigned grid_nof_subcs   = MAX_RB * NRE;

  // Create resource grid.
  std::unique_ptr<resource_grid> grid = create_resource_grid(nof_rx_ports, grid_nof_symbols, grid_nof_subcs);
  TESTASSERT(grid);

  unsigned nof_grid_re = grid_nof_subcs * grid_nof_symbols * nof_rx_ports;

  // Create a vector to hold the randomly generated RE.
  std::vector<cf_t> random_re(nof_grid_re);

  // Standard complex normal distribution with zero mean.
  complex_normal_distribution<cf_t> c_normal_dist = {};

  // Generate random RE.
  std::generate(random_re.begin(), random_re.end(), [&rgen, &c_normal_dist]() { return c_normal_dist(rgen); });

  // Generate a RE mask and set all elements to true.
  bounded_bitset<NRE* MAX_RB> re_mask = ~bounded_bitset<NRE * MAX_RB>(grid_nof_subcs);

  // Fill the grid with the random RE.
  span<const cf_t> re_view(random_re);
  for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
    for (unsigned i_symbol = 0; i_symbol != grid_nof_symbols; ++i_symbol) {
      re_view = grid->get_writer().put(i_rx_port, i_symbol, 0, re_mask, re_view);
    }
  }

  // Create processor and validator.
  std::unique_ptr<pusch_processor>     processor;
  std::unique_ptr<pusch_pdu_validator> validator;
  std::tie(processor, validator) = create_processor();

  // Generate the test cases.
  std::vector<test_case_type> test_case_set = generate_test_cases(selected_profile);

  for (const test_case_type& test_case : test_case_set) {
    // Get the PUSCH configuration.
    const pusch_processor::pdu_t& config = std::get<0>(test_case);
    // Get the TBS in bits.
    unsigned tbs = std::get<1>(test_case);

    // Make sure the configuration is valid.
    TESTASSERT(validator->is_valid(config));

    // Reset finish counter.
    finish_count  = 0;
    pending_count = 0;
    thread_quit   = false;

    // Prepare threads for the current case.
    std::vector<unique_thread> threads(nof_threads);
    for (unsigned thread_id = 0; thread_id != nof_threads; ++thread_id) {
      // Select thread.
      unique_thread& thread = threads[thread_id];

      // Create thread.
      thread = unique_thread("thread_" + std::to_string(thread_id), [&proc = *processor, &config, &tbs, &grid] {
        thread_process(proc, config, tbs, grid.get()->get_reader());
      });
    }

    // Wait for finish thread init.
    while (pending_count.load() != -static_cast<int>(nof_threads)) {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    // Calculate the peak throughput, considering that the number of bits is for a slot.
    double slot_duration_us     = 1e3 / static_cast<double>(pow2(config.slot.numerology()));
    double peak_throughput_Mbps = static_cast<double>(tbs) / slot_duration_us;

    // Measurement description.
    fmt::memory_buffer meas_description;
    fmt::format_to(meas_description,
                   "PUSCH RB={:<3} Mod={:<6} R={:<5.3f} rv={} - {:>5.1f} Mbps",
                   config.freq_alloc.get_nof_rb(),
                   to_string(config.mcs_descr.modulation),
                   config.mcs_descr.get_normalised_target_code_rate(),
                   config.codeword.value().rv,
                   peak_throughput_Mbps);

    // Run the benchmark.
    perf_meas.new_measure(to_string(meas_description), nof_threads * batch_size_per_thread * tbs, []() mutable {
      // Notify start.
      finish_count  = 0;
      pending_count = nof_threads * batch_size_per_thread;

      // Wait for finish.
      while (finish_count.load() != (nof_threads * batch_size_per_thread)) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
    });

    thread_quit = true;

    for (unique_thread& thread : threads) {
      thread.join();
    }
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

  if (worker_pool) {
    worker_pool->stop();
  }

  return 0;
}
