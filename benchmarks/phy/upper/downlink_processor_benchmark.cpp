/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/support/support_factories.h"
#include "srsgnb/phy/upper/upper_phy_factories.h"
#include "srsgnb/phy/upper/upper_phy_rg_gateway.h"
#include "srsgnb/support/benchmark_utils.h"
#include "srsgnb/support/executors/task_executor.h"
#include "srsgnb/support/srsgnb_test.h"
#include <getopt.h>
#include <random>

using namespace srsgnb;

// Random generator.
static std::mt19937 rgen(0);

// General parameters.
static unsigned           nof_repetitions   = 1000;
static bool               silent            = false;
static std::string        ldpc_encoder_type = "auto";
static std::string        benchmark_profile = "default";
static unsigned           rg_nof_ports      = 1;
static unsigned           rg_nof_symbols    = srsgnb::MAX_NSYMB_PER_SLOT;
static unsigned           rg_nof_prb        = srsgnb::MAX_RB;
static subcarrier_spacing common_scs        = subcarrier_spacing::kHz15;
static uint16_t           rnti              = 0x1234;
static unsigned           bwp_size_rb       = MAX_RB;
static unsigned           bwp_start_rb      = 0;
static cyclic_prefix      global_cp         = cyclic_prefix::NORMAL;

// PDSCH parameters.
static std::vector<std::pair<modulation_scheme, double>> pdsch_mcs_set = {{modulation_scheme::QPSK, 120.0 / 1024.0},
                                                                          {modulation_scheme::QAM16, 658.0 / 1024.0},
                                                                          {modulation_scheme::QAM64, 873.0 / 1024.0},
                                                                          {modulation_scheme::QAM256, 948.0 / 1024.0}};
static unsigned                                          pdsch_rv      = 0;
static unsigned                                          pdsch_n_id    = 0;
static std::array<bool, MAX_NSYMB_PER_SLOT> pdsch_dmrs_symbol_mask     = {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0};
static dmrs_type                            pdsch_dmrs                 = dmrs_type::TYPE1;
static unsigned                             pdsch_scrambling_id        = 0;
static bool                                 pdsch_nscid                = false;
static unsigned                             pdsch_nof_cdm_grps_wo_data = 2;
static std::vector<rb_allocation>           pdsch_freq_alloc_set       = {rb_allocation::make_type1(0, 26),
                                                                          rb_allocation::make_type1(0, 52),
                                                                          rb_allocation::make_type1(0, 102),
                                                                          rb_allocation::make_type1(0, 270)};
static unsigned                             pdsch_start_symbol         = 2;
static unsigned                             pdsch_nof_symbols          = 12;

// PDCCH parameters.
static unsigned                                              pdcch_start_symbol_index     = 0;
static std::vector<unsigned>                                 pdcch_duration_set           = {2};
static std::vector<pdcch_processor::cce_to_reg_mapping_type> pdcch_cce_to_reg_mapping_set = {
    pdcch_processor::cce_to_reg_mapping_type::CORESET0};
static std::vector<unsigned> pdcch_aggregation_level_set = {1, 2, 4, 8, 16};
static std::vector<unsigned> pdcch_payload_size_set      = {30, 60, pdcch_constants::MAX_DCI_PAYLOAD_SIZE};

namespace {
struct configuration_profile {
  std::string           name;
  std::string           description;
  std::function<void()> function;
};

} // namespace

static const std::vector<configuration_profile> profiles = {
    {"Default",
     "Runs all combinations.",
     []() {
       // Use defaults.
     }},
    {"pdsch_scs15_50MHz_qpsk_min",
     "Encodes PDSCH with 50MHz bandwidth with a subcarrier spacing of 15kHz, QPSK modulation at minimum rate.",
     []() {
       pdsch_freq_alloc_set = {rb_allocation::make_type1(0, 270)};
       pdsch_mcs_set        = {{modulation_scheme::QPSK, 120.0 / 1024.0}};

       // Disable PDCCH.
       pdcch_duration_set = {};
     }},
    {"pdsch_scs15_50MHz_256qam_max",
     "Encodes PDSCH with 50MHz bandwidth with a subcarrier spacing of 15kHz, 256-QAM modulation at maximum rate.",
     []() {
       pdsch_freq_alloc_set = {rb_allocation::make_type1(0, 270)};
       pdsch_mcs_set        = {{modulation_scheme::QAM256, 948.0 / 1024.0}};

       // Disable PDCCH.
       pdcch_duration_set = {};
     }},
    {"pdcch_coreset0_min",
     "Encodes PDCCH with a duration of one symbol, CCE-to-REG mapping as CORESET0, an aggregation level of one and "
     "a payload size of 16.",
     []() {
       pdcch_duration_set           = {1};
       pdcch_cce_to_reg_mapping_set = {pdcch_processor::cce_to_reg_mapping_type::CORESET0};
       pdcch_aggregation_level_set  = {1};
       pdcch_payload_size_set       = {16};

       // Disable PDSCH.
       pdsch_freq_alloc_set = {};
     }},
    {"pdcch",
     "Benchmarks all PDCCH",
     []() {
       // Disable PDSCH.
       pdsch_freq_alloc_set = {};
     }},
    {"pdcch_coreset0_max",
     "Encodes PDCCH with a duration of three symbols, CCE-to-REG mapping as CORESET0, an aggregation level of 16 and "
     "a payload size of 128.",
     []() {
       pdcch_duration_set           = {1};
       pdcch_cce_to_reg_mapping_set = {pdcch_processor::cce_to_reg_mapping_type::CORESET0};
       pdcch_aggregation_level_set  = {16};
       pdcch_payload_size_set       = {pdcch_constants::MAX_DCI_PAYLOAD_SIZE};

       // Disable PDSCH.
       pdsch_freq_alloc_set = {};
     }},
};

static void usage(const char* prog)
{
  fmt::print("Usage: {} [-R repetitions] [-s silent]\n", prog);
  fmt::print("\t-R Repetitions [Default {}]\n", nof_repetitions);
  fmt::print("\t-E LDPC encoder type [Default {}]\n", ldpc_encoder_type);
  fmt::print("\t-P Benchmark profile. [Default {}]\n", profiles.front().name);
  for (const configuration_profile& profile : profiles) {
    fmt::print("\t\t {:<30}{}\n", profile.name, profile.description);
  }
  fmt::print("\t-s Toggle silent operation [Default {}]\n", silent);
  fmt::print("\t-h Show this message\n");
}

static int parse_args(int argc, char** argv)
{
  std::string profile_name;

  int opt = 0;
  while ((opt = getopt(argc, argv, "R:E:P:sh")) != -1) {
    switch (opt) {
      case 'R':
        nof_repetitions = std::strtol(optarg, nullptr, 10);
        break;
      case 'E':
        ldpc_encoder_type = std::string(optarg);
        break;
      case 'P':
        profile_name = std::string(optarg);
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

  // Search profile if set.
  if (!profile_name.empty()) {
    bool found = false;
    for (const auto& profile : profiles) {
      if (profile_name == profile.name) {
        profile.function();
        srslog::fetch_basic_logger("TEST").info("Loading profile: {}", profile.name);
        found = true;
        break;
      }
    }
    if (!found) {
      usage(argv[0]);
      srslog::fetch_basic_logger("TEST").error("Invalid profile: {}.", profile_name);
      return -1;
    }
  } else {
    const configuration_profile& default_profile = profiles.front();

    srslog::fetch_basic_logger("TEST").info("Loading default profile: {}", default_profile.name);
    default_profile.function();
  }

  return 0;
}

using namespace srsgnb;

class immediate_task_executor : public task_executor
{
public:
  void execute(unique_task task) override
  {
    srsgnb_assert(!task.is_empty(), "Invalid task.");
    task();
  }
  void defer(unique_task task) override
  {
    srsgnb_assert(!task.is_empty(), "Invalid task.");
    task();
  }
};

class upper_phy_rg_gateway_dummy : public upper_phy_rg_gateway
{
public:
  void send(const resource_grid_context& context, const resource_grid_reader& grid) override
  {
    // Do nothing.
  }
};

int main(int argc, char** argv)
{
  // Parse arguments.
  int ret = parse_args(argc, argv);
  if (ret < 0) {
    return ret;
  }

  // Create downlink processor dependencies.
  immediate_task_executor    executor;
  upper_phy_rg_gateway_dummy gateway;

  // Create downlink processor factory.
  downlink_processor_factory_sw_config dl_proc_factory_config;
  dl_proc_factory_config.ldpc_encoder_type            = ldpc_encoder_type;
  std::shared_ptr<downlink_processor_factory> factory = create_downlink_processor_factory_sw(dl_proc_factory_config);
  report_fatal_error_if_not(factory, "Invalid factory.");

  // Create downlink processor.
  downlink_processor_config dl_proc_config;
  dl_proc_config.id                           = 0;
  dl_proc_config.executor                     = &executor;
  dl_proc_config.gateway                      = &gateway;
  std::unique_ptr<downlink_processor> dl_proc = factory->create(dl_proc_config);
  report_fatal_error_if_not(dl_proc, "Invalid DL processor.");

  // Create resource grid.
  std::unique_ptr<resource_grid> dl_rg = create_resource_grid(rg_nof_ports, rg_nof_symbols, rg_nof_prb * NRE);
  report_fatal_error_if_not(dl_rg, "Invalid resource grid.");

  // Benchmark PDSCH.
  benchmarker perf_meas_pdsch("DL processor - PDSCH", nof_repetitions);
  for (rb_allocation pdsch_freq_alloc : pdsch_freq_alloc_set) {
    for (std::pair<modulation_scheme, double> pdsch_mcs : pdsch_mcs_set) {
      // Extract MCS parameters.
      modulation_scheme pdsch_modulation = pdsch_mcs.first;
      double            pdsch_rate       = pdsch_mcs.second;

      // Calculate TBS.
      unsigned nof_dmrs_per_rb  = (pdsch_dmrs == dmrs_type::TYPE1) ? (6 * (2 - pdsch_nof_cdm_grps_wo_data))
                                                                   : (4 * (3 - pdsch_nof_cdm_grps_wo_data));
      unsigned nof_channel_bits = pdsch_freq_alloc.get_nof_rb() *
                                  (NRE * pdsch_nof_symbols - pdsch_dmrs_symbol_mask.size() * nof_dmrs_per_rb) *
                                  get_bits_per_symbol(pdsch_modulation);
      unsigned pdsch_tbs_nbytes = static_cast<unsigned>(static_cast<double>(nof_channel_bits) * pdsch_rate / 8);

      // Deduce LDPC base graph from rate and TBS.
      ldpc_base_graph_type pdsch_base_graph = get_ldpc_base_graph(pdsch_rate, pdsch_tbs_nbytes * 8);

      // Create transport block.
      std::vector<uint8_t> transport_block(pdsch_tbs_nbytes);
      std::generate(
          transport_block.begin(), transport_block.end(), [&]() { return static_cast<unsigned>(rgen() & 0xff); });
      static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data = {transport_block};

      // Prepare PDU.
      pdsch_processor::pdu_t pdu;
      pdu.slot         = slot_point(to_numerology_value(common_scs), 0);
      pdu.rnti         = rnti;
      pdu.bwp_size_rb  = bwp_size_rb;
      pdu.bwp_start_rb = bwp_start_rb;
      pdu.cp           = global_cp;
      pdu.codewords.emplace_back();
      pdu.codewords[0].modulation     = pdsch_modulation;
      pdu.codewords[0].rv             = pdsch_rv;
      pdu.n_id                        = pdsch_n_id;
      pdu.ports                       = {0};
      pdu.ref_point                   = pdsch_processor::pdu_t::CRB0;
      pdu.dmrs_symbol_mask            = pdsch_dmrs_symbol_mask;
      pdu.dmrs                        = pdsch_dmrs;
      pdu.scrambling_id               = pdsch_scrambling_id;
      pdu.n_scid                      = pdsch_nscid;
      pdu.nof_cdm_groups_without_data = pdsch_nof_cdm_grps_wo_data;
      pdu.freq_alloc                  = pdsch_freq_alloc;
      pdu.start_symbol_index          = pdsch_start_symbol;
      pdu.nof_symbols                 = pdsch_nof_symbols;
      pdu.ldpc_base_graph             = pdsch_base_graph;
      pdu.tbs_lbrm_bytes              = ldpc::MAX_CODEBLOCK_SIZE / 8;
      pdu.reserved                    = {};
      pdu.ratio_pdsch_dmrs_to_sss_dB  = 0.0F;
      pdu.ratio_pdsch_data_to_sss_dB  = 0.0F;

      // Prepare context.
      resource_grid_context slot_context;
      slot_context.slot   = pdu.slot;
      slot_context.sector = 0;

      // Configure resource grid.
      dl_proc->configure_resource_grid(slot_context, *dl_rg);

      // Calculate the peak throughput, considering that the number of bits is for a slot.
      double slot_duration_us     = 1e3 / static_cast<double>(pow2(to_numerology_value(common_scs)));
      double peak_throughput_Mbps = static_cast<double>(pdsch_tbs_nbytes * 8) / slot_duration_us;

      fmt::memory_buffer description_buf;
      fmt::format_to(description_buf,
                     "PDSCH BW={:<3} Mod={:<6} R={:<5.3f} - {:>5.1f} Mbps",
                     pdsch_freq_alloc.get_nof_rb(),
                     to_string(pdsch_modulation),
                     pdsch_rate,
                     peak_throughput_Mbps);

      // Process PDU multiple times.
      perf_meas_pdsch.new_measure(
          to_string(description_buf), pdsch_tbs_nbytes * 8, [&]() { dl_proc->process_pdsch(data, pdu); });

      dl_proc->finish_processing_pdus();
    }
  }

  // Benchmark PDCCH.
  benchmarker perf_meas_pdcch("DL processor - PDCCH", nof_repetitions);
  for (unsigned pdcch_duration : pdcch_duration_set) {
    for (pdcch_processor::cce_to_reg_mapping_type pdcch_cce_to_reg_mapping : pdcch_cce_to_reg_mapping_set) {
      for (unsigned pdcch_aggregation_level : pdcch_aggregation_level_set) {
        for (unsigned pdcch_payload_size : pdcch_payload_size_set) {
          // Prepare PDU.
          pdcch_processor::pdu_t pdu;
          pdu.slot                       = slot_point(to_numerology_value(common_scs), 0);
          pdu.cp                         = global_cp;
          pdu.coreset.bwp_size_rb        = (bwp_size_rb / 12) * 12;
          pdu.coreset.bwp_start_rb       = bwp_start_rb;
          pdu.coreset.start_symbol_index = pdcch_start_symbol_index;
          pdu.coreset.duration           = pdcch_duration;
          pdu.coreset.frequency_resources.resize(pdcch_constants::MAX_NOF_FREQ_RESOURCES);
          pdu.coreset.frequency_resources.fill(0, pdu.coreset.bwp_size_rb / 6);
          pdu.coreset.cce_to_reg_mapping = pdcch_cce_to_reg_mapping;
          pdu.coreset.reg_bundle_size    = 6;
          pdu.coreset.interleaver_size   = 2;
          pdu.coreset.shift_index        = 0;
          pdu.dci_list.emplace_back();
          pdu.dci_list.back().rnti                 = rnti;
          pdu.dci_list.back().n_id_pdcch_dmrs      = 0;
          pdu.dci_list.back().n_id_pdcch_data      = 0;
          pdu.dci_list.back().n_rnti               = rnti;
          pdu.dci_list.back().cce_index            = 0;
          pdu.dci_list.back().aggregation_level    = pdcch_aggregation_level;
          pdu.dci_list.back().dmrs_power_offset_dB = 0.0;
          pdu.dci_list.back().data_power_offset_dB = 0.0;
          pdu.dci_list.back().payload.resize(pdcch_payload_size);
          pdu.dci_list.back().ports = {0};

          // Skip case if the number of CORESET available CCE is not sufficient to accommodate the aggregation level.
          unsigned nof_cce_coreset = pdu.coreset.frequency_resources.count() * pdu.coreset.duration;
          if (nof_cce_coreset < pdcch_aggregation_level) {
            continue;
          }

          // Skip case if the payload size exceeds half of the number of channel bits.
          unsigned nof_channel_bits =
              pdcch_aggregation_level * pdcch_constants::NOF_REG_PER_CCE * pdcch_constants::NOF_RE_PDCCH_PER_RB * 2;
          if (nof_channel_bits / 2 < pdcch_payload_size) {
            continue;
          }

          // Prepare context.
          resource_grid_context slot_context;
          slot_context.slot   = pdu.slot;
          slot_context.sector = 0;

          // Configure resource grid.
          dl_proc->configure_resource_grid(slot_context, *dl_rg);

          fmt::memory_buffer description_buf;
          fmt::format_to(description_buf,
                         "PDCCH duration={} reg-to-cce={:<15} AL={:<2} PS={}",
                         pdcch_duration,
                         pdcch_cce_to_reg_mapping == pdcch_processor::cce_to_reg_mapping_type::CORESET0 ? "CORESET0"
                         : pdcch_cce_to_reg_mapping == pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED
                             ? "interleaved"
                             : "non-interleaved",
                         pdcch_aggregation_level,
                         pdcch_payload_size);

          // Process PDU multiple times.
          perf_meas_pdcch.new_measure(
              to_string(description_buf), pdu.dci_list.back().payload.size(), [&]() { dl_proc->process_pdcch(pdu); });

          dl_proc->finish_processing_pdus();
        }
      }
    }
  }

  if (!silent) {
    perf_meas_pdsch.print_percentiles_throughput("bits");
    perf_meas_pdcch.print_percentiles_time();
  }
}