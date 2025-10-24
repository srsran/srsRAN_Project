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

#include "srsran/adt/to_array.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/pucch/factories.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/support/benchmark_utils.h"
#include "srsran/support/executors/task_worker_pool.h"
#include "srsran/support/executors/unique_thread.h"
#include "srsran/support/math/complex_normal_random.h"
#include "srsran/support/math/math_utils.h"
#include "srsran/support/srsran_test.h"
#include <getopt.h>
#include <random>
#include <variant>

using namespace srsran;

// The benchmark configuration consists any of the PUCCH configurations.
using pucch_configuration = std::variant<pucch_processor::format0_configuration,
                                         pucch_processor::format1_configuration,
                                         pucch_processor::format2_configuration,
                                         pucch_processor::format3_configuration,
                                         pucch_processor::format4_configuration>;

namespace {

enum class benchmark_modes : unsigned { silent, latency, throughput_total, throughput_thread, all, invalid };

} // namespace

static const char* to_string(benchmark_modes mode)
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

static benchmark_modes to_benchmark_mode(const char* string)
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
static constexpr subcarrier_spacing scs                   = subcarrier_spacing::kHz30;
static constexpr cyclic_prefix      cy_prefix             = cyclic_prefix::NORMAL;
static constexpr unsigned           bwp_start_rb          = 0;
static constexpr unsigned           bwp_size_rb           = MAX_RB;
static constexpr unsigned           max_nof_ports         = 4;
static uint64_t                     nof_repetitions       = 1000;
static uint64_t                     nof_threads           = 1;
static uint64_t                     batch_size_per_thread = 100;
static std::string                  selected_profile_name = "all";
static benchmark_modes              benchmark_mode        = benchmark_modes::latency;

// Thread shared variables.
static constexpr auto        thread_sync_sleep_duration = std::chrono::nanoseconds(10U);
static std::atomic<bool>     thread_quit                = {};
static std::atomic<int>      pending_count              = {0};
static std::atomic<unsigned> finish_count               = {0};

// Test profile structure, initialized with default profile values.
struct test_profile {
  std::string         name         = "none";
  std::string         description  = "TBD.";
  pucch_configuration pucch_config = pucch_processor::format0_configuration{};
};

// Profile selected during test execution.
static test_profile selected_profile = {};

// Available test profiles.
static const auto profile_set = to_array<test_profile>({
    test_profile{
        .name = "Format0",
        .description =
            "PUCCH Format 0 with frequency hopping, the maximum number of cyclic shifts and four receive ports.",
        .pucch_config = pucch_processor::format0_configuration{.context      = std::nullopt,
                                                               .slot         = slot_point(to_numerology_value(scs), 0),
                                                               .cp           = cy_prefix,
                                                               .bwp_size_rb  = bwp_size_rb,
                                                               .bwp_start_rb = bwp_start_rb,
                                                               .starting_prb = 0,
                                                               .second_hop_prb       = 5,
                                                               .start_symbol_index   = 0,
                                                               .nof_symbols          = 2,
                                                               .initial_cyclic_shift = 0,
                                                               .n_id                 = 0,
                                                               .nof_harq_ack         = 2,
                                                               .sr_opportunity       = true,
                                                               .ports                = {0, 1, 2, 3}}},

    test_profile{
        .name = "Format1",
        .description =
            "PUCCH Format 1 with frequency hopping, the maximum HARQ-ACK feedback bits and four receive ports.",
        .pucch_config = pucch_processor::format1_configuration{.context      = std::nullopt,
                                                               .slot         = slot_point(to_numerology_value(scs), 0),
                                                               .bwp_size_rb  = bwp_size_rb,
                                                               .bwp_start_rb = bwp_start_rb,
                                                               .cp           = cy_prefix,
                                                               .starting_prb = 0,
                                                               .second_hop_prb       = 5,
                                                               .n_id                 = 0,
                                                               .nof_harq_ack         = 2,
                                                               .ports                = {0, 1, 2, 3},
                                                               .initial_cyclic_shift = 0,
                                                               .nof_symbols          = 14,
                                                               .start_symbol_index   = 0,
                                                               .time_domain_occ      = 1}},

    test_profile{
        .name = "Format2_4bit",
        .description =
            "PUCCH Format 2 with frequency hopping, the maximum HARQ-ACK feedback bits and four receive ports.",
        .pucch_config = pucch_processor::format2_configuration{.context      = std::nullopt,
                                                               .slot         = slot_point(to_numerology_value(scs), 0),
                                                               .cp           = cy_prefix,
                                                               .ports        = {0, 1, 2, 3},
                                                               .bwp_size_rb  = bwp_size_rb,
                                                               .bwp_start_rb = bwp_start_rb,
                                                               .starting_prb = 0,
                                                               .second_hop_prb     = 5,
                                                               .nof_prb            = 16,
                                                               .start_symbol_index = 0,
                                                               .nof_symbols        = 2,
                                                               .rnti               = 0x1234,
                                                               .n_id               = 0,
                                                               .n_id_0             = 0,
                                                               .nof_harq_ack       = 0,
                                                               .nof_csi_part1      = 4,
                                                               .nof_csi_part2      = 0}},

    test_profile{
        .name = "Format2_10bit",
        .description =
            "PUCCH Format 2 with frequency hopping, the maximum HARQ-ACK feedback bits and four receive ports.",
        .pucch_config = pucch_processor::format2_configuration{.context      = std::nullopt,
                                                               .slot         = slot_point(to_numerology_value(scs), 0),
                                                               .cp           = cy_prefix,
                                                               .ports        = {0, 1, 2, 3},
                                                               .bwp_size_rb  = bwp_size_rb,
                                                               .bwp_start_rb = bwp_start_rb,
                                                               .starting_prb = 0,
                                                               .second_hop_prb     = 5,
                                                               .nof_prb            = 16,
                                                               .start_symbol_index = 0,
                                                               .nof_symbols        = 2,
                                                               .rnti               = 0x1234,
                                                               .n_id               = 0,
                                                               .n_id_0             = 0,
                                                               .nof_harq_ack       = 0,
                                                               .nof_csi_part1      = 11,
                                                               .nof_csi_part2      = 0}},

    test_profile{
        .name = "Format2_30bit",
        .description =
            "PUCCH Format 2 with frequency hopping, the maximum HARQ-ACK feedback bits and four receive ports.",
        .pucch_config = pucch_processor::format2_configuration{.context      = std::nullopt,
                                                               .slot         = slot_point(to_numerology_value(scs), 0),
                                                               .cp           = cy_prefix,
                                                               .ports        = {0, 1, 2, 3},
                                                               .bwp_size_rb  = bwp_size_rb,
                                                               .bwp_start_rb = bwp_start_rb,
                                                               .starting_prb = 0,
                                                               .second_hop_prb     = 5,
                                                               .nof_prb            = 16,
                                                               .start_symbol_index = 0,
                                                               .nof_symbols        = 2,
                                                               .rnti               = 0x1234,
                                                               .n_id               = 0,
                                                               .n_id_0             = 0,
                                                               .nof_harq_ack       = 0,
                                                               .nof_csi_part1      = 30,
                                                               .nof_csi_part2      = 0}},
});

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-m benchmark mode] [-R repetitions] [-B Batch size per thread] [-T number of threads] [-P "
             "profile] [-h]\n",
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
  fmt::print("\t-P Benchmark profile. [Default {}]\n", selected_profile_name);
  for (const test_profile& profile : profile_set) {
    fmt::print("\t\t {:<40} {}\n", profile.name, profile.description);
  }

  fmt::print("\t-h Show this message\n");
}

static int parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:T:B:P:m:h")) != -1) {
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
      case 'h':
      default:
        usage(argv[0]);
        std::exit(0);
    }
  }

  if (selected_profile_name != "all") {
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
  }

  return 0;
}

static pucch_processor_factory& get_pucch_processor_factory()
{
  static std::shared_ptr<pucch_processor_factory> pucch_proc_factory = nullptr;

  if (pucch_proc_factory) {
    return *pucch_proc_factory;
  }

  // Create factories required by the PUCCH demodulator factory.
  std::shared_ptr<channel_equalizer_factory> equalizer_factory = create_channel_equalizer_generic_factory();
  TESTASSERT(equalizer_factory);

  std::shared_ptr<demodulation_mapper_factory> demod_factory = create_demodulation_mapper_factory();
  TESTASSERT(demod_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
  TESTASSERT(dft_factory);

  std::shared_ptr<transform_precoder_factory> precoding_factory =
      create_dft_transform_precoder_factory(dft_factory, pucch_constants::FORMAT3_MAX_NPRB + 1);
  TESTASSERT(precoding_factory);

  // Create PUCCH demodulator factory.
  std::shared_ptr<pucch_demodulator_factory> pucch_demod_factory =
      create_pucch_demodulator_factory_sw(equalizer_factory, demod_factory, prg_factory, precoding_factory);
  TESTASSERT(pucch_demod_factory);

  // Create factories required by the PUCCH channel estimator factory.
  std::shared_ptr<low_papr_sequence_generator_factory> lpg_factory = create_low_papr_sequence_generator_sw_factory();
  TESTASSERT(lpg_factory);

  std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory =
      create_low_papr_sequence_collection_sw_factory(lpg_factory);
  TESTASSERT(lpc_factory);

  std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory =
      create_time_alignment_estimator_dft_factory(dft_factory);
  TESTASSERT(ta_estimator_factory);

  // Create channel estimator factory.
  std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
      create_port_channel_estimator_factory_sw(ta_estimator_factory);
  TESTASSERT(port_chan_estimator_factory);

  std::shared_ptr<dmrs_pucch_estimator_factory> estimator_factory =
      create_dmrs_pucch_estimator_factory_sw(prg_factory, lpc_factory, lpg_factory, port_chan_estimator_factory);
  TESTASSERT(estimator_factory);

  // Create PUCCH detector factory.
  std::shared_ptr<pucch_detector_factory> detector_factory =
      create_pucch_detector_factory_sw(lpc_factory, prg_factory, equalizer_factory, dft_factory);
  TESTASSERT(detector_factory);

  // Create short block detector factory.
  std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
  TESTASSERT(short_block_det_factory);

  // Create polar decoder factory.
  std::shared_ptr<polar_factory> polar_dec_factory = create_polar_factory_sw();
  TESTASSERT(polar_dec_factory);

  // Create CRC calculator factory.
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calc_factory);

  // Create UCI decoder factory.
  std::shared_ptr<uci_decoder_factory> uci_dec_factory =
      create_uci_decoder_factory_generic(short_block_det_factory, polar_dec_factory, crc_calc_factory);
  TESTASSERT(uci_dec_factory);

  // Create PUCCH processor factory.
  channel_estimate::channel_estimate_dimensions max_dimensions = {.nof_prb       = bwp_size_rb,
                                                                  .nof_symbols   = get_nsymb_per_slot(cy_prefix),
                                                                  .nof_rx_ports  = max_nof_ports,
                                                                  .nof_tx_layers = pucch_constants::MAX_LAYERS};
  pucch_proc_factory                                           = create_pucch_processor_factory_sw(
      estimator_factory, detector_factory, pucch_demod_factory, uci_dec_factory, max_dimensions);
  TESTASSERT(pucch_proc_factory);

  if (nof_threads > 1) {
    pucch_proc_factory = create_pucch_processor_pool_factory(std::move(pucch_proc_factory), nof_threads);
    TESTASSERT(pucch_proc_factory);
  }

  return *pucch_proc_factory;
}

// Instantiates the pucch processor and validator.
static std::tuple<std::unique_ptr<pucch_processor>, std::unique_ptr<pucch_pdu_validator>> create_processor()
{
  pucch_processor_factory& pucch_proc_factory = get_pucch_processor_factory();

  // Create pucch processor.
  std::unique_ptr<pucch_processor> processor = pucch_proc_factory.create();
  TESTASSERT(processor);

  // Create pucch processor validator.
  std::unique_ptr<pucch_pdu_validator> validator = pucch_proc_factory.create_validator();
  TESTASSERT(validator);

  return std::make_tuple(std::move(processor), std::move(validator));
}

template <typename ConfigType>
std::optional<ConfigType> get_config(const pucch_configuration& test_case)
{
  if (std::holds_alternative<ConfigType>(test_case)) {
    return std::get<ConfigType>(test_case);
  }

  return std::nullopt;
}

static void thread_process(pucch_processor& proc, const pucch_configuration& config, const resource_grid_reader& grid)
{
  while (!thread_quit) {
    // If the pending count is equal to or lower than zero, wait for a new start.
    while (pending_count.fetch_sub(1) <= 0) {
      // Wait for pending to non-negative.
      while (pending_count.load() <= 0) {
        // Sleep.
        std::this_thread::sleep_for(thread_sync_sleep_duration);

        // Quit if signaled.
        if (thread_quit) {
          return;
        }
      }
    }

    if (auto pucch0 = get_config<pucch_processor::format0_configuration>(config)) {
      proc.process(grid, *pucch0);
    } else if (auto pucch1 = get_config<pucch_processor::format1_configuration>(config)) {
      proc.process(grid, pucch_processor::format1_batch_configuration(*pucch1));
    } else if (auto pucch2 = get_config<pucch_processor::format2_configuration>(config)) {
      proc.process(grid, *pucch2);
    } else if (auto pucch3 = get_config<pucch_processor::format3_configuration>(config)) {
      proc.process(grid, *pucch3);
    } else if (auto pucch4 = get_config<pucch_processor::format4_configuration>(config)) {
      proc.process(grid, *pucch4);
    }

    // Notify finish count.
    ++finish_count;
  }
}

// Creates a resource grid.
static std::unique_ptr<resource_grid> create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc)
{
  std::shared_ptr<resource_grid_factory> rg_factory = create_resource_grid_factory();
  TESTASSERT(rg_factory != nullptr, "Invalid resource grid factory.");

  return rg_factory->create(nof_ports, nof_symbols, nof_subc);
}

int main(int argc, char** argv)
{
  int ret = parse_args(argc, argv);
  if (ret < 0) {
    return ret;
  }

  // Inform of the benchmark configuration.
  if (benchmark_mode != benchmark_modes::silent) {
    fmt::print("Launching benchmark for {} threads, {} times per thread, and {} repetitions. Using {} profile.\n",
               nof_threads,
               batch_size_per_thread,
               nof_repetitions,
               selected_profile_name);
  }

  benchmarker perf_meas("pucch processor", nof_repetitions);

  // Pseudo-random generator.
  std::mt19937 rgen(0);

  // Create resource grid.
  std::unique_ptr<resource_grid> grid =
      create_resource_grid(max_nof_ports, get_nsymb_per_slot(cy_prefix), NRE * bwp_size_rb);
  TESTASSERT(grid);

  // Standard complex normal distribution with zero mean.
  complex_normal_distribution<cf_t> c_normal_dist = {};

  // Fill the grid with the random RE.
  for (unsigned i_rx_port = 0; i_rx_port != max_nof_ports; ++i_rx_port) {
    for (unsigned i_symbol = 0, i_symbol_end = get_nsymb_per_slot(cy_prefix); i_symbol != i_symbol_end; ++i_symbol) {
      // Obtain view of the OFDM symbol.
      span<cbf16_t> re_view = grid->get_writer().get_view(i_rx_port, i_symbol);

      // Generate random RE.
      std::generate(re_view.begin(), re_view.end(), [&rgen, &c_normal_dist]() { return c_normal_dist(rgen); });
    }
  }

  // Create processor and validator.
  std::unique_ptr<pucch_processor>     processor;
  std::unique_ptr<pucch_pdu_validator> validator;
  std::tie(processor, validator) = create_processor();

  for (const test_profile& profile : profile_set) {
    // Skip profile if the selected is not default and does not match the current profile.
    if ((selected_profile_name != "all") && (profile.name != selected_profile_name)) {
      continue;
    }
    // Get the pucch configuration.
    const pucch_configuration& config = profile.pucch_config;

    // Make sure the configuration is valid.
    if (auto pucch0 = get_config<pucch_processor::format0_configuration>(config)) {
      TESTASSERT(validator->is_valid(*pucch0));
    } else if (auto pucch1 = get_config<pucch_processor::format1_configuration>(config)) {
      TESTASSERT(validator->is_valid(*pucch1));
    } else if (auto pucch2 = get_config<pucch_processor::format2_configuration>(config)) {
      TESTASSERT(validator->is_valid(*pucch2));
    } else if (auto pucch3 = get_config<pucch_processor::format3_configuration>(config)) {
      TESTASSERT(validator->is_valid(*pucch3));
    } else if (auto pucch4 = get_config<pucch_processor::format4_configuration>(config)) {
      TESTASSERT(validator->is_valid(*pucch4));
    }

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
      thread = unique_thread("thread_" + std::to_string(thread_id), [&proc = *processor, &config, &grid] {
        thread_process(proc, config, grid.get()->get_reader());
      });
    }

    // Wait for finish thread init.
    while (pending_count.load() != -static_cast<int>(nof_threads)) {
      std::this_thread::sleep_for(thread_sync_sleep_duration);
    }

    // Run the benchmark.
    perf_meas.new_measure(profile.name, nof_threads * batch_size_per_thread, []() mutable {
      // Notify start.
      finish_count  = 0;
      pending_count = nof_threads * batch_size_per_thread;

      // Wait for finish.
      while (finish_count.load() != (nof_threads * batch_size_per_thread)) {
        std::this_thread::sleep_for(thread_sync_sleep_duration);
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
    perf_meas.print_percentiles_time("microseconds", 1e-3 / static_cast<double>(batch_size_per_thread));
  }

  // Print total aggregated throughput.
  if ((benchmark_mode == benchmark_modes::throughput_total) || (benchmark_mode == benchmark_modes::all)) {
    fmt::print("\n--- Total throughput ---\n");
    perf_meas.print_percentiles_throughput("transmissions");
  }

  // Print average throughput per thread.
  if ((benchmark_mode == benchmark_modes::throughput_thread) || (benchmark_mode == benchmark_modes::all)) {
    fmt::print("\n--- Thread throughput ---\n");
    perf_meas.print_percentiles_throughput("transmissions", 1.0 / static_cast<double>(nof_threads));
  }

  return 0;
}
