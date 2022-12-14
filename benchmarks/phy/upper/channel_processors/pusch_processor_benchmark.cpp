/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/phy/upper/rx_softbuffer_pool_impl.h"
#include "../../../lib/scheduler/support/tbs_calculator.h"
#include "srsgnb/phy/support/support_factories.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/support/benchmark_utils.h"
#include "srsgnb/support/srsgnb_test.h"
#include <getopt.h>
#include <random>

using namespace srsgnb;

// A test case consists of a PUSCH PDU configuration and a Transport Block Size.
using test_case_type = std::tuple<pusch_processor::pdu_t, unsigned>;

// General test configuration parameters.
static unsigned                           nof_repetitions             = 10;
static std::string                        selected_profile_name       = "default";
static std::string                        ldpc_decoder_type           = "auto";
static bool                               silent                      = false;
static unsigned                           nof_rx_ports                = 1;
static unsigned                           nof_tx_layers               = 1;
static dmrs_type                          dmrs                        = dmrs_type::TYPE1;
static unsigned                           nof_cdm_groups_without_data = 2;
static bounded_bitset<MAX_NSYMB_PER_SLOT> dmrs_symbol_mask =
    {false, false, true, false, false, false, false, false, false, false, false, false, false, false};

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
     {{modulation_scheme::QPSK, 120.0F}}},

    {"pusch_scs15_5MHz_256qam_max",
     "Decodes PUSCH with 5 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {25},
     {{modulation_scheme::QAM256, 948.0F}}},

    {"pusch_scs15_20MHz_qpsk_min",
     "Decodes PUSCH with 20 MHz of bandwidth and a 15 kHz SCS, QPSK modulation at minimum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {106},
     {{modulation_scheme::QPSK, 120.0F}}},

    {"pusch_scs15_20MHz_16qam_med",
     "Decodes PUSCH with 20 MHz of bandwidth and a 15 kHz SCS, 16-QAM modulation at a medium code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {106},
     {{modulation_scheme::QAM16, 658.0F}}},

    {"pusch_scs15_20MHz_64qam_high",
     "Decodes PUSCH with 20 MHz of bandwidth and a 15 kHz SCS, 64-QAM modulation at a high code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {106},
     {{modulation_scheme::QAM64, 873.0F}}},

    {"pusch_scs15_20MHz_256qam_max",
     "Decodes PUSCH with 20 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {106},
     {{modulation_scheme::QAM256, 948.0F}}},

    {"pusch_scs15_50MHz_qpsk_min",
     "Decodes PUSCH with 50 MHz of bandwidth and a 15 kHz SCS, QPSK modulation at minimum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {270},
     {{modulation_scheme::QPSK, 120.0F}}},

    {"pusch_scs15_50MHz_256qam_max",
     "Decodes PUSCH with 50 MHz of bandwidth and a 15 kHz SCS, 256-QAM modulation at maximum code rate.",
     subcarrier_spacing::kHz15,
     cyclic_prefix::NORMAL,
     get_nsymb_per_slot(cyclic_prefix::NORMAL),
     {270},
     {{modulation_scheme::QAM256, 948.0F}}},
};

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-R repetitions] [-D LDPC type] [-P profile] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-D LDPC decoder type. [Default {}]\n", ldpc_decoder_type);
  fmt::print("\t-P Benchmark profile. [Default {}]\n", selected_profile_name);
  for (const test_profile& profile : profile_set) {
    fmt::print("\t\t {:<30}{}\n", profile.name, profile.description);
  }
  fmt::print("\t-s Toggle silent operation [Default {}]\n", silent);
  fmt::print("\t-h Show this message\n");
}

static int parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "R:D:P:sh")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'D':
        ldpc_decoder_type = std::string(optarg);
        break;
      case 'P':
        selected_profile_name = std::string(optarg);
        break;
      case 's':
        silent = (!silent);
        break;
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
      // Determine the Transport Block Size.
      tbs_calculator_configuration tbs_config = {};
      tbs_config.mcs_descr                    = mcs;
      tbs_config.n_prb                        = nof_prb;
      tbs_config.nof_layers                   = nof_tx_layers;
      tbs_config.nof_symb_sh                  = profile.nof_symbols;
      tbs_config.nof_dmrs_prb                 = dmrs.nof_dmrs_per_rb() * dmrs_symbol_mask.count();
      unsigned tbs                            = tbs_calculator_calculate(tbs_config);

      // Build the PUSCH PDU configuration.
      pusch_processor::pdu_t config = {};
      config.slot                   = slot_point(to_numerology_value(profile.scs), 0);
      config.rnti                   = 1;
      config.bwp_size_rb            = nof_prb;
      config.bwp_start_rb           = 0;
      config.cp                     = profile.cp;
      config.mcs_descr              = mcs;
      config.codeword.emplace(pusch_processor::codeword_description{
          0, get_ldpc_base_graph(mcs.get_normalised_target_code_rate(), units::bits(tbs)), true});
      config.uci           = {};
      config.n_id          = 0;
      config.nof_tx_layers = nof_tx_layers;
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
  return test_case_set;
}

// Instantiates the PUSCH processor and validator.
static std::tuple<std::unique_ptr<pusch_processor>, std::unique_ptr<pusch_pdu_validator>> create_processor()
{
  // Create pseudo-random sequence generator.
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  // Create demodulator mapper factory.
  std::shared_ptr<channel_modulation_factory> chan_modulation_factory = create_channel_modulation_sw_factory();
  TESTASSERT(chan_modulation_factory);

  // Create CRC calculator factory.
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw();
  TESTASSERT(crc_calc_factory);

  // Create LDPC decoder factory.
  std::shared_ptr<ldpc_decoder_factory> ldpc_dec_factory = create_ldpc_decoder_factory_sw(ldpc_decoder_type);
  TESTASSERT(ldpc_dec_factory);

  // Create LDPC rate dematcher factory.
  std::shared_ptr<ldpc_rate_dematcher_factory> ldpc_rm_factory = create_ldpc_rate_dematcher_factory_sw();
  TESTASSERT(ldpc_rm_factory);

  // Create LDPC desegmenter factory.
  std::shared_ptr<ldpc_segmenter_rx_factory> ldpc_segm_rx_factory = create_ldpc_segmenter_rx_factory_sw();
  TESTASSERT(ldpc_segm_rx_factory);

  std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
  TESTASSERT(short_block_det_factory);

  // Create port channel estimator factory.
  std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
      create_port_channel_estimator_factory_sw();
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
      create_pusch_demodulator_factory_sw(eq_factory, chan_modulation_factory, prg_factory);
  TESTASSERT(pusch_demod_factory);

  // Create PUSCH demultiplexer factory.
  std::shared_ptr<ulsch_demultiplex_factory> demux_factory = create_ulsch_demultiplex_factory_sw();
  TESTASSERT(demux_factory);

  // Create PUSCH decoder factory.
  pusch_decoder_factory_sw_configuration pusch_dec_config;
  pusch_dec_config.crc_factory                             = crc_calc_factory;
  pusch_dec_config.decoder_factory                         = ldpc_dec_factory;
  pusch_dec_config.dematcher_factory                       = ldpc_rm_factory;
  pusch_dec_config.segmenter_factory                       = ldpc_segm_rx_factory;
  std::shared_ptr<pusch_decoder_factory> pusch_dec_factory = create_pusch_decoder_factory_sw(pusch_dec_config);
  TESTASSERT(pusch_dec_factory);

  // Create UCI decoder factory.
  uci_decoder_factory_sw_configuration uci_dec_factory_config;
  uci_dec_factory_config.decoder_factory               = short_block_det_factory;
  std::shared_ptr<uci_decoder_factory> uci_dec_factory = create_uci_decoder_factory_sw(uci_dec_factory_config);
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
  std::shared_ptr<pusch_processor_factory> pusch_proc_factory =
      create_pusch_processor_factory_sw(pusch_proc_factory_config);
  TESTASSERT(pusch_proc_factory);

  // Create PUSCH processor.
  std::unique_ptr<pusch_processor> processor = pusch_proc_factory->create();
  TESTASSERT(processor);

  // Create PUSCH processor validator.
  std::unique_ptr<pusch_pdu_validator> validator = pusch_proc_factory->create_validator();
  TESTASSERT(validator);

  return std::make_tuple(std::move(processor), std::move(validator));
}

int main(int argc, char** argv)
{
  int ret = parse_args(argc, argv);
  if (ret < 0) {
    return ret;
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

  // Normal distribution with zero mean.
  std::normal_distribution<float> normal_dist(0.0F, M_SQRT1_2);

  // Generate random RE.
  std::generate(random_re.begin(), random_re.end(), [&rgen, &normal_dist]() {
    return std::complex<float>(normal_dist(rgen), normal_dist(rgen));
  });

  // Generate a RE mask and set all elements to true.
  bounded_bitset<NRE* MAX_RB> re_mask = ~bounded_bitset<NRE * MAX_RB>(grid_nof_subcs);

  // Fill the grid with the random RE.
  span<const cf_t> re_view(random_re);
  for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
    for (unsigned i_symbol = 0; i_symbol != grid_nof_symbols; ++i_symbol) {
      re_view = grid->put(i_rx_port, i_symbol, 0, re_mask, re_view);
    }
  }

  // Generate the test cases.
  std::vector<test_case_type> test_case_set = generate_test_cases(selected_profile);

  for (const test_case_type& test_case : test_case_set) {
    // Get the PUSCH configuration.
    const pusch_processor::pdu_t& config = std::get<0>(test_case);
    // Get the TBS in bits.
    unsigned tbs = std::get<1>(test_case);

    // Create the PUSCH processor and validator.
    std::tuple<std::unique_ptr<pusch_processor>, std::unique_ptr<pusch_pdu_validator>> proc = create_processor();

    std::unique_ptr<pusch_processor>     processor(std::move(std::get<0>(proc)));
    std::unique_ptr<pusch_pdu_validator> validator(std::move(std::get<1>(proc)));

    // Make sure the configuration is valid.
    TESTASSERT(validator->is_valid(config));

    // Compute the number of codeblocks.
    unsigned nof_codeblocks = ldpc::compute_nof_codeblocks(units::bits(tbs), config.codeword.value().ldpc_base_graph);

    // Softbuffer pool configuration.
    rx_softbuffer_pool_config softbuffer_config = {};
    softbuffer_config.max_softbuffers           = 1;
    softbuffer_config.max_nof_codeblocks        = nof_codeblocks;
    softbuffer_config.max_codeblock_size        = ldpc::MAX_CODEBLOCK_SIZE;
    softbuffer_config.expire_timeout_slots =
        100 * get_nof_slots_per_subframe(to_subcarrier_spacing(config.slot.numerology()));

    rx_softbuffer_identifier softbuffer_id = {};
    softbuffer_id.rnti                     = config.rnti;

    // Create softbuffer pool.
    std::unique_ptr<rx_softbuffer_pool> softbuffer_pool = create_rx_softbuffer_pool(softbuffer_config);

    // Reserve softbuffer.
    rx_softbuffer* softbuffer = softbuffer_pool->reserve_softbuffer(config.slot, softbuffer_id, nof_codeblocks);

    // Prepare receive data buffer.
    std::vector<uint8_t> data(tbs / 8);

    // Calculate the peak throughput, considering that the number of bits is for a slot.
    double slot_duration_us     = 1e3 / static_cast<double>(pow2(config.slot.numerology()));
    double peak_throughput_Mbps = static_cast<double>(tbs) / slot_duration_us;

    // Measurement description.
    fmt::memory_buffer meas_description;
    fmt::format_to(meas_description,
                   "PUSCH RB={:<3} Mod={:<6} R={:<5.3f} - {:>5.1f} Mbps",
                   config.freq_alloc.get_nof_rb(),
                   to_string(config.mcs_descr.modulation),
                   config.mcs_descr.get_normalised_target_code_rate(),
                   peak_throughput_Mbps);

    // Run the benchmark.
    perf_meas.new_measure(to_string(meas_description), tbs, [&proc = processor, &grid, &softbuffer, &data, &config]() {
      proc->process(data, *softbuffer, *grid, config);
    });
  }

  if (!silent) {
    // Print results.
    perf_meas.print_percentiles_throughput("bits");
  }

  return 0;
}
