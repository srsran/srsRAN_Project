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

#include "../../../support/resource_grid_test_doubles.h"
#include "../../rx_buffer_test_doubles.h"
#include "../../signal_processors/dmrs_pusch_estimator_test_doubles.h"
#include "../uci/uci_decoder_test_doubles.h"
#include "pusch_decoder_test_doubles.h"
#include "pusch_demodulator_test_doubles.h"
#include "pusch_processor_result_test_doubles.h"
#include "ulsch_demultiplex_test_doubles.h"
#include "srsran/phy/upper/channel_processors/pusch/factories.h"
#include "srsran/ran/pusch/ulsch_info.h"
#include "srsran/ran/sch_dmrs_power.h"
#include "srsran/srsvec/compare.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace srsran {
std::ostream& operator<<(std::ostream& os, subcarrier_spacing value)
{
  fmt::print(os, "{}", to_string(value));
  return os;
}

std::ostream& operator<<(std::ostream& os, span<const log_likelihood_ratio> data)
{
  fmt::print(os, "{}", data);
  return os;
}

std::ostream& operator<<(std::ostream& os, modulation_scheme value)
{
  fmt::print(os, "{}", to_string(value));
  return os;
}

std::ostream& operator<<(std::ostream& os, cyclic_prefix value)
{
  fmt::print(os, "{}", value.to_string());
  return os;
}

std::ostream& operator<<(std::ostream& os, uci_status value)
{
  fmt::print(os, "{}", to_string(value));
  return os;
}

static bool operator==(span<const log_likelihood_ratio> left, span<const log_likelihood_ratio> right)
{
  return srsvec::equal(left, right);
}

} // namespace srsran

namespace {

using PuschProcessorParams =
    std::tuple<subcarrier_spacing, cyclic_prefix, modulation_scheme, bool, unsigned, unsigned, unsigned>;

class PuschProcessorFixture : public ::testing::TestWithParam<PuschProcessorParams>
{
protected:
  static void SetUpTestSuite()
  {
    // Skip setup if processor has already been created.
    if (pusch_proc) {
      return;
    }

    std::string filename(log_filename);

    // Set up logging.
    srslog::sink* log_sink = (filename == "stdout") ? srslog::create_stdout_sink() : srslog::create_file_sink(filename);

    if (log_sink == nullptr) {
      report_error("Could not create application main log sink.\n");
    }

    // Create spy factories.
    std::shared_ptr<dmrs_pusch_estimator_factory_spy> estimator_factory_spy =
        std::make_shared<dmrs_pusch_estimator_factory_spy>();
    std::shared_ptr<pusch_demodulator_factory_spy> demodulator_factory_spy =
        std::make_shared<pusch_demodulator_factory_spy>();
    std::shared_ptr<ulsch_demultiplex_factory_spy> demux_factory_spy =
        std::make_shared<ulsch_demultiplex_factory_spy>();
    std::shared_ptr<pusch_decoder_factory_spy> decoder_factory_spy = std::make_shared<pusch_decoder_factory_spy>();
    std::shared_ptr<uci_decoder_factory_spy>   uci_dec_factory_spy = std::make_shared<uci_decoder_factory_spy>();

    // Create PUSCH processor factory.
    pusch_processor_factory_sw_configuration proc_factory_config;
    proc_factory_config.estimator_factory                    = estimator_factory_spy;
    proc_factory_config.demodulator_factory                  = demodulator_factory_spy;
    proc_factory_config.demux_factory                        = demux_factory_spy;
    proc_factory_config.decoder_factory                      = decoder_factory_spy;
    proc_factory_config.uci_dec_factory                      = uci_dec_factory_spy;
    proc_factory_config.ch_estimate_dimensions.nof_prb       = MAX_RB;
    proc_factory_config.ch_estimate_dimensions.nof_symbols   = MAX_NSYMB_PER_SLOT;
    proc_factory_config.ch_estimate_dimensions.nof_rx_ports  = MAX_PORTS;
    proc_factory_config.ch_estimate_dimensions.nof_tx_layers = 1;
    proc_factory_config.max_nof_concurrent_threads           = 1;
    std::shared_ptr<pusch_processor_factory> pusch_proc_factory =
        create_pusch_processor_factory_sw(proc_factory_config);

    // Initialise logging.
    srslog::set_default_sink(*log_sink);
    srslog::init();

    srslog::basic_logger& debug_logger = srslog::fetch_basic_logger(debug_logger_id, false);
    debug_logger.set_level(srslog::basic_levels::debug);
    debug_logger.set_hex_dump_max_size(log_max_hex_dump);

    srslog::basic_logger& info_logger = srslog::fetch_basic_logger(info_logger_id, false);
    info_logger.set_level(srslog::basic_levels::info);
    info_logger.set_hex_dump_max_size(log_max_hex_dump);

    // Create actual PUSCH processors.
    pusch_proc       = pusch_proc_factory->create();
    pusch_proc_debug = pusch_proc_factory->create(debug_logger);
    pusch_proc_info  = pusch_proc_factory->create(info_logger);
    validator        = pusch_proc_factory->create_validator();

    // Select spies.
    estimator_spy     = estimator_factory_spy->get_entries().front();
    demodulator_spy   = demodulator_factory_spy->get_entries().front();
    demux_spy         = demux_factory_spy->get_entries().front();
    decoder_spy       = decoder_factory_spy->get_entries()[0];
    decoder_spy_debug = decoder_factory_spy->get_entries()[1];
    decoder_spy_info  = decoder_factory_spy->get_entries()[2];
    uci_dec_spy       = uci_dec_factory_spy->get_entries().front();
  }

  void SetUp() override
  {
    // Extract test parameters.
    subcarrier_spacing scs                = std::get<0>(GetParam());
    cyclic_prefix      cyclic_prefix_conf = std::get<1>(GetParam());
    modulation_scheme  modulation         = std::get<2>(GetParam());
    bool               codeword_present   = std::get<3>(GetParam());
    unsigned           nof_harq_ack_bits  = std::get<4>(GetParam());
    unsigned           nof_csi_part1_bits = std::get<5>(GetParam());
    unsigned           nof_rx_ports       = std::get<6>(GetParam());

    unsigned numerology             = to_numerology_value(scs);
    unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(scs);

    // Generate PDU.
    pdu.slot                       = slot_point(numerology, slot_count_dist(rgen) % (10240 * nof_slots_per_subframe));
    pdu.rnti                       = rnti_dist(rgen);
    pdu.bwp_start_rb               = bwp_start_dist(rgen);
    pdu.bwp_size_rb                = std::min(MAX_RB - pdu.bwp_start_rb, bwp_size_dist(rgen));
    pdu.cp                         = cyclic_prefix_conf;
    pdu.mcs_descr.modulation       = modulation;
    pdu.mcs_descr.target_code_rate = target_code_rate_dist(rgen);

    if (codeword_present) {
      pdu.codeword.emplace();
      pdu.codeword.value().rv              = rv_dist(rgen);
      pdu.codeword.value().ldpc_base_graph = static_cast<ldpc_base_graph_type>(ldpc_base_graph_dist(rgen));
      pdu.codeword.value().new_data        = bool_dist(rgen) == 0;
    }

    // Fill UCI configuration.
    pdu.uci.nof_harq_ack          = nof_harq_ack_bits;
    pdu.uci.nof_csi_part1         = nof_csi_part1_bits;
    pdu.uci.csi_part2_size        = {};
    pdu.uci.alpha_scaling         = 1.0F;
    pdu.uci.beta_offset_harq_ack  = 20.0F;
    pdu.uci.beta_offset_csi_part1 = 20.0F;
    pdu.uci.beta_offset_csi_part2 = 20.0F;

    unsigned rb_alloc_start   = bwp_start_dist(rgen) % pdu.bwp_size_rb;
    unsigned rb_alloc_count   = std::min(pdu.bwp_size_rb - rb_alloc_start, bwp_size_dist(rgen));
    unsigned nof_symbols_slot = get_nsymb_per_slot(pdu.cp);

    pdu.n_id          = n_id_dist(rgen);
    pdu.nof_tx_layers = 1;
    pdu.rx_ports.resize(nof_rx_ports);
    std::generate(pdu.rx_ports.begin(), pdu.rx_ports.end(), []() { return rx_port_dist(rgen); });
    pdu.dmrs                        = dmrs_type::TYPE1;
    pdu.scrambling_id               = scrambling_id_dist(rgen);
    pdu.n_scid                      = n_scid_dist(rgen) == 0;
    pdu.nof_cdm_groups_without_data = 2;
    pdu.freq_alloc                  = rb_allocation::make_type1(rb_alloc_start, rb_alloc_count);
    pdu.start_symbol_index          = start_symbol_index_dist(rgen);
    pdu.nof_symbols                 = nof_symbols_slot - pdu.start_symbol_index;
    pdu.tbs_lbrm_bytes              = tbs_lbrm_bytes_dist(rgen);
    pdu.dmrs_symbol_mask            = symbol_slot_mask(nof_symbols_slot);

    for (unsigned i_symbol = pdu.start_symbol_index, i_symbol_end = pdu.start_symbol_index + pdu.nof_symbols;
         i_symbol != i_symbol_end;
         ++i_symbol) {
      pdu.dmrs_symbol_mask.set(i_symbol, bool_dist(rgen) == 0);
    }
    if (pdu.dmrs_symbol_mask.none()) {
      pdu.dmrs_symbol_mask.set(pdu.start_symbol_index);
    }

    // Reset spies.
    estimator_spy->clear();
    demodulator_spy->clear();
    demux_spy->clear();
    decoder_spy->clear();
    uci_dec_spy->clear();
  }

  void TearDown() override
  {
    // Make sure the log is flushed before the next test starts.
    srslog::flush();
  }

  static constexpr const char* log_filename     = "/dev/null";
  static constexpr const char* info_logger_id   = "info";
  static constexpr const char* debug_logger_id  = "debug";
  static constexpr unsigned    log_max_hex_dump = 16;

  static dmrs_pusch_estimator_spy* estimator_spy;
  static pusch_demodulator_spy*    demodulator_spy;
  static ulsch_demultiplex_spy*    demux_spy;
  static pusch_decoder_spy*        decoder_spy;
  static pusch_decoder_spy*        decoder_spy_info;
  static pusch_decoder_spy*        decoder_spy_debug;
  static uci_decoder_spy*          uci_dec_spy;

  static std::unique_ptr<pusch_processor>     pusch_proc;
  static std::unique_ptr<pusch_processor>     pusch_proc_info;
  static std::unique_ptr<pusch_processor>     pusch_proc_debug;
  static std::unique_ptr<pusch_pdu_validator> validator;

  static std::mt19937                            rgen;
  static std::uniform_int_distribution<unsigned> slot_count_dist;
  static std::uniform_int_distribution<uint16_t> rnti_dist;
  static std::uniform_int_distribution<unsigned> bwp_size_dist;
  static std::uniform_int_distribution<unsigned> bwp_start_dist;
  static std::uniform_int_distribution<unsigned> rv_dist;
  static std::uniform_int_distribution<unsigned> bool_dist;
  static std::uniform_int_distribution<uint8_t>  ldpc_base_graph_dist;
  static std::uniform_int_distribution<unsigned> n_id_dist;
  static std::uniform_int_distribution<unsigned> rx_port_dist;
  static std::uniform_int_distribution<unsigned> scrambling_id_dist;
  static std::uniform_int_distribution<unsigned> n_scid_dist;
  static std::uniform_int_distribution<unsigned> start_symbol_index_dist;
  static std::uniform_int_distribution<unsigned> tbs_lbrm_bytes_dist;
  static std::uniform_int_distribution<unsigned> tbs_dist;
  static std::uniform_real_distribution<float>   target_code_rate_dist;

  pusch_processor::pdu_t pdu;
};

dmrs_pusch_estimator_spy* PuschProcessorFixture::estimator_spy     = nullptr;
pusch_demodulator_spy*    PuschProcessorFixture::demodulator_spy   = nullptr;
ulsch_demultiplex_spy*    PuschProcessorFixture::demux_spy         = nullptr;
pusch_decoder_spy*        PuschProcessorFixture::decoder_spy       = nullptr;
pusch_decoder_spy*        PuschProcessorFixture::decoder_spy_info  = nullptr;
pusch_decoder_spy*        PuschProcessorFixture::decoder_spy_debug = nullptr;
uci_decoder_spy*          PuschProcessorFixture::uci_dec_spy       = nullptr;

std::unique_ptr<pusch_processor>     PuschProcessorFixture::pusch_proc       = nullptr;
std::unique_ptr<pusch_processor>     PuschProcessorFixture::pusch_proc_info  = nullptr;
std::unique_ptr<pusch_processor>     PuschProcessorFixture::pusch_proc_debug = nullptr;
std::unique_ptr<pusch_pdu_validator> PuschProcessorFixture::validator        = nullptr;

std::mt19937                            PuschProcessorFixture::rgen;
std::uniform_int_distribution<unsigned> PuschProcessorFixture::slot_count_dist(0, 10240 * 8);
std::uniform_int_distribution<uint16_t> PuschProcessorFixture::rnti_dist(1, std::numeric_limits<uint16_t>::max());
std::uniform_int_distribution<unsigned> PuschProcessorFixture::bwp_size_dist(1, MAX_RB);
std::uniform_int_distribution<unsigned> PuschProcessorFixture::bwp_start_dist(0, MAX_RB - 1);
std::uniform_int_distribution<unsigned> PuschProcessorFixture::rv_dist(0, 3);
std::uniform_int_distribution<unsigned> PuschProcessorFixture::bool_dist(0, 1);
std::uniform_int_distribution<uint8_t>  PuschProcessorFixture::ldpc_base_graph_dist(0, 1);
std::uniform_int_distribution<unsigned> PuschProcessorFixture::n_id_dist(0, 1023);
std::uniform_int_distribution<unsigned> PuschProcessorFixture::rx_port_dist(1, 4);
std::uniform_int_distribution<unsigned> PuschProcessorFixture::scrambling_id_dist(0, 65535);
std::uniform_int_distribution<unsigned> PuschProcessorFixture::n_scid_dist(0, 1);
std::uniform_int_distribution<unsigned> PuschProcessorFixture::start_symbol_index_dist(0, MAX_NSYMB_PER_SLOT / 2);
std::uniform_int_distribution<unsigned> PuschProcessorFixture::tbs_lbrm_bytes_dist(1, ldpc::MAX_CODEBLOCK_SIZE / 8);
std::uniform_int_distribution<unsigned> PuschProcessorFixture::tbs_dist(1, 1024);
std::uniform_real_distribution<float>   PuschProcessorFixture::target_code_rate_dist(30.F, 772.F);

TEST_P(PuschProcessorFixture, PuschProcessorUnittest)
{
  using namespace units::literals;

  // Make sure PUSCH processor is created.
  ASSERT_TRUE(pusch_proc);
  ASSERT_TRUE(validator);

  ASSERT_NE(estimator_spy, nullptr);
  ASSERT_NE(demodulator_spy, nullptr);
  ASSERT_NE(demux_spy, nullptr);
  ASSERT_NE(decoder_spy, nullptr);
  ASSERT_NE(uci_dec_spy, nullptr);

  // Calculate number of symbols carrying DM-RS.
  unsigned nof_dmrs_symbols = pdu.dmrs_symbol_mask.count();

  // Calculate the total number of DM-RS per PRB.
  unsigned nof_dmrs_per_prb = pdu.dmrs.nof_dmrs_per_rb() * pdu.nof_cdm_groups_without_data * nof_dmrs_symbols;

  // Calculate the mnumber of data RE per PRB.
  unsigned nof_re_per_prb = NRE * pdu.nof_symbols - nof_dmrs_per_prb;

  // Calculate the number of RE used for the PUSCH transmission.
  unsigned nof_re = pdu.freq_alloc.get_nof_rb() * nof_re_per_prb;

  // Generate resource block mask.
  bounded_bitset<MAX_RB> rb_mask = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Generate transport block.
  std::vector<uint8_t> transport_block(tbs_dist(rgen));
  std::generate(transport_block.begin(), transport_block.end(), [&]() { return static_cast<uint8_t>(rgen()); });

  // Get UL-SCH information.
  ulsch_configuration ulsch_config;
  ulsch_config.tbs                   = units::bytes(transport_block.size()).to_bits();
  ulsch_config.mcs_descr             = pdu.mcs_descr;
  ulsch_config.nof_harq_ack_bits     = units::bits(pdu.uci.nof_harq_ack);
  ulsch_config.nof_csi_part1_bits    = units::bits(pdu.uci.nof_csi_part1);
  ulsch_config.nof_csi_part2_bits    = 0_bits;
  ulsch_config.alpha_scaling         = pdu.uci.alpha_scaling;
  ulsch_config.beta_offset_harq_ack  = pdu.uci.beta_offset_harq_ack;
  ulsch_config.beta_offset_csi_part1 = pdu.uci.beta_offset_csi_part1;
  ulsch_config.beta_offset_csi_part2 = pdu.uci.beta_offset_csi_part2;
  ulsch_config.nof_rb                = rb_mask.count();
  ulsch_config.start_symbol_index    = pdu.start_symbol_index;
  ulsch_config.nof_symbols           = pdu.nof_symbols;
  ulsch_config.dmrs_type             = pdu.dmrs == dmrs_type::TYPE1 ? dmrs_config_type::type1 : dmrs_config_type::type2;
  ulsch_config.dmrs_symbol_mask      = pdu.dmrs_symbol_mask;
  ulsch_config.nof_cdm_groups_without_data = pdu.nof_cdm_groups_without_data;
  ulsch_config.nof_layers                  = pdu.nof_tx_layers;
  ulsch_information ulsch_info             = get_ulsch_information(ulsch_config);

  // Calculate the number of LLR.
  unsigned nof_codeword_llr = nof_re * get_bits_per_symbol(pdu.mcs_descr.modulation) * pdu.nof_tx_layers;
  demodulator_spy->set_codeword_size(nof_codeword_llr);
  demux_spy->set_ulsch_config(nof_codeword_llr,
                              ulsch_info.nof_ul_sch_bits.value(),
                              ulsch_info.nof_harq_ack_bits.value(),
                              ulsch_info.nof_csi_part1_bits.value(),
                              ulsch_info.nof_csi_part2_bits.value());

  // Create receive buffer.
  rx_buffer_spy    rm_buffer_spy;
  unique_rx_buffer rm_buffer(rm_buffer_spy);

  // Resource grid spy.
  resource_grid_reader_spy rg_spy;

  // Process PDU.
  pusch_processor_result_notifier_spy result_notifier;
  pusch_proc->process(transport_block, std::move(rm_buffer), result_notifier, rg_spy, pdu);

  // Extract results.
  const auto& sch_entries = result_notifier.get_sch_entries();
  const auto& uci_entries = result_notifier.get_uci_entries();

  // Make sure PDU is valid.
  ASSERT_TRUE(validator->is_valid(pdu));

  // Calling resource grid and buffer methods are not permitted.
  ASSERT_EQ(0, rg_spy.get_count());
  ASSERT_EQ(0, rm_buffer_spy.get_total_count());

  // Assert channel estimator inputs.
  ASSERT_EQ(1, estimator_spy->get_entries().size());
  const dmrs_pusch_estimator_spy::entry_t& estimator_entry = estimator_spy->get_entries().front();
  ASSERT_EQ(&rg_spy, estimator_entry.grid);
  ASSERT_EQ(pdu.slot, estimator_entry.config.slot);
  ASSERT_EQ(pdu.dmrs, estimator_entry.config.type);
  ASSERT_EQ(pdu.scrambling_id, estimator_entry.config.scrambling_id);
  ASSERT_EQ(pdu.n_scid, estimator_entry.config.n_scid);
  ASSERT_EQ(convert_dB_to_amplitude(-get_sch_to_dmrs_ratio_dB(pdu.nof_cdm_groups_without_data)),
            estimator_entry.config.scaling);
  ASSERT_EQ(pdu.cp, estimator_entry.config.c_prefix);
  ASSERT_EQ(pdu.dmrs_symbol_mask, estimator_entry.config.symbols_mask);
  ASSERT_EQ(rb_mask, estimator_entry.config.rb_mask);
  ASSERT_EQ(pdu.start_symbol_index, estimator_entry.config.first_symbol);
  ASSERT_EQ(pdu.nof_symbols, estimator_entry.config.nof_symbols);
  ASSERT_EQ(pdu.nof_tx_layers, estimator_entry.config.nof_tx_layers);
  ASSERT_EQ(span<const uint8_t>(pdu.rx_ports), span<const uint8_t>(estimator_entry.config.rx_ports));

  // Assert demodulator inputs.
  ASSERT_EQ(1, demodulator_spy->get_entries().size());
  const pusch_demodulator_spy::entry_t& demodulator_entry = demodulator_spy->get_entries().front();
  ASSERT_EQ(&rg_spy, demodulator_entry.grid);
  ASSERT_EQ(estimator_entry.estimate, demodulator_entry.estimates);
  ASSERT_EQ(pdu.rnti, demodulator_entry.config.rnti);
  ASSERT_EQ(rb_mask, demodulator_entry.config.rb_mask);
  ASSERT_EQ(pdu.mcs_descr.modulation, demodulator_entry.config.modulation);
  ASSERT_EQ(pdu.start_symbol_index, demodulator_entry.config.start_symbol_index);
  ASSERT_EQ(pdu.nof_symbols, demodulator_entry.config.nof_symbols);
  ASSERT_EQ(pdu.dmrs_symbol_mask, demodulator_entry.config.dmrs_symb_pos);
  ASSERT_EQ(pdu.dmrs, demodulator_entry.config.dmrs_config_type);
  ASSERT_EQ(pdu.nof_cdm_groups_without_data, demodulator_entry.config.nof_cdm_groups_without_data);
  ASSERT_EQ(pdu.n_id, demodulator_entry.config.n_id);
  ASSERT_EQ(pdu.nof_tx_layers, demodulator_entry.config.nof_tx_layers);
  ASSERT_EQ(span<const uint8_t>(pdu.rx_ports), span<const uint8_t>(demodulator_entry.config.rx_ports));

  // Assert demux if UCI is multiplexed.
  ASSERT_EQ(1, demux_spy->get_demultiplex_entries().size());
  const ulsch_demultiplex_spy::demultiplex_entry& demux_entry = demux_spy->get_demultiplex_entries().front();
  ASSERT_EQ(demodulator_entry.codeword, demux_entry.input.get_data());
  ASSERT_EQ(demodulator_entry.scrambling_seq, demux_entry.input.get_scrambling_seq());
  ASSERT_EQ(pdu.mcs_descr.modulation, demux_entry.config.modulation);
  ASSERT_EQ(pdu.nof_tx_layers, demux_entry.config.nof_layers);
  ASSERT_EQ(pdu.freq_alloc.get_nof_rb(), demux_entry.config.nof_prb);
  ASSERT_EQ(pdu.start_symbol_index, demux_entry.config.start_symbol_index);
  ASSERT_EQ(pdu.nof_symbols, demux_entry.config.nof_symbols);
  ASSERT_EQ(ulsch_info.nof_harq_ack_rvd.value(), demux_entry.config.nof_harq_ack_rvd);
  ASSERT_EQ(pdu.dmrs, demux_entry.config.dmrs);
  ASSERT_EQ(pdu.dmrs_symbol_mask, demux_entry.config.dmrs_symbol_mask);
  ASSERT_EQ(pdu.nof_cdm_groups_without_data, demux_entry.config.nof_cdm_groups_without_data);

  // Assert decoder inputs only if the codeword is present.
  if (pdu.codeword.has_value()) {
    ASSERT_EQ(1, decoder_spy->get_entries().size());
    const pusch_decoder_spy::entry_t& decoder_entry = decoder_spy->get_entries().front();
    ASSERT_EQ(decoder_entry.transport_block.data(), transport_block.data());
    ASSERT_EQ(decoder_entry.transport_block.size(), transport_block.size());
    ASSERT_EQ(&rm_buffer_spy, &decoder_entry.rm_buffer.get());
    ASSERT_EQ(span<const log_likelihood_ratio>(demux_entry.sch_data), decoder_entry.input.get_data());
    ASSERT_EQ(pdu.codeword.value().ldpc_base_graph, decoder_entry.config.base_graph);
    ASSERT_EQ(pdu.mcs_descr.modulation, decoder_entry.config.mod);
    ASSERT_EQ(pdu.tbs_lbrm_bytes * 8, decoder_entry.config.Nref);
    ASSERT_EQ(pdu.nof_tx_layers, decoder_entry.config.nof_layers);
    ASSERT_EQ(10, decoder_entry.config.nof_ldpc_iterations);
    ASSERT_EQ(true, decoder_entry.config.use_early_stop);
    ASSERT_EQ(pdu.codeword.value().new_data, decoder_entry.config.new_data);

    ASSERT_EQ(sch_entries.size(), 1);
    const auto& sch_entry = sch_entries.back();
    ASSERT_EQ(decoder_entry.stats.tb_crc_ok, sch_entry.data.tb_crc_ok);
    ASSERT_EQ(decoder_entry.stats.nof_codeblocks_total, sch_entry.data.nof_codeblocks_total);

    // Clear decoder spy entries - It makes sure the soft buffer is unlocked before destroying the pool.
    decoder_spy->clear();
  } else {
    ASSERT_EQ(0, decoder_spy->get_entries().size());
    ASSERT_TRUE(sch_entries.empty());
  }

  // Calculate the number of UCI decoded fields.
  unsigned nof_uci_fields  = ((pdu.uci.nof_harq_ack > 0) ? 1 : 0) + ((pdu.uci.nof_csi_part1 > 0) ? 1 : 0);
  unsigned uci_decoder_idx = 0;
  ASSERT_EQ(nof_uci_fields, uci_dec_spy->get_entries().size());

  // Assert HARQ-ACK decoder inputs.
  if (pdu.uci.nof_harq_ack > 0) {
    const uci_decoder_spy::entry_t& uci_dec_entry = uci_dec_spy->get_entries()[uci_decoder_idx++];
    ASSERT_EQ(span<const log_likelihood_ratio>(demux_entry.harq_ack),
              span<const log_likelihood_ratio>(uci_dec_entry.llr));
    ASSERT_EQ(pdu.mcs_descr.modulation, uci_dec_entry.config.modulation);

    uci_payload_type packed_harq_ack(uci_dec_entry.message.begin(), uci_dec_entry.message.end());

    ASSERT_EQ(uci_entries.size(), 1);
    const auto& uci_entry = uci_entries.back();
    ASSERT_EQ(uci_entry.harq_ack.payload, packed_harq_ack);
    ASSERT_EQ(uci_entry.harq_ack.status, uci_dec_entry.status);
  } else if (!uci_entries.empty()) {
    const auto& uci_entry = uci_entries.back();
    ASSERT_EQ(uci_status::unknown, uci_entry.harq_ack.status);
    ASSERT_TRUE(uci_entry.harq_ack.payload.empty());
  }

  // Assert CSI Part 1 decoder inputs.
  if (pdu.uci.nof_csi_part1 > 0) {
    const uci_decoder_spy::entry_t& uci_dec_entry = uci_dec_spy->get_entries()[uci_decoder_idx++];
    ASSERT_EQ(span<const log_likelihood_ratio>(demux_entry.csi_part1),
              span<const log_likelihood_ratio>(uci_dec_entry.llr));
    ASSERT_EQ(pdu.mcs_descr.modulation, uci_dec_entry.config.modulation);

    uci_payload_type packed_csi_part1(uci_dec_entry.message.begin(), uci_dec_entry.message.end());

    ASSERT_EQ(uci_entries.size(), 1);
    const auto& uci_entry = uci_entries.back();
    ASSERT_EQ(uci_entry.csi_part1.payload, packed_csi_part1);
    ASSERT_EQ(uci_entry.csi_part1.status, uci_dec_entry.status);
  } else if (!uci_entries.empty()) {
    const auto& uci_entry = uci_entries.back();
    ASSERT_EQ(uci_status::unknown, uci_entry.csi_part1.status);
    ASSERT_TRUE(uci_entry.csi_part1.payload.empty());
  }
}

TEST_P(PuschProcessorFixture, HealthTestFormatterInfo)
{
  // Generate transport block.
  std::vector<uint8_t> transport_block(tbs_dist(rgen));
  std::generate(transport_block.begin(), transport_block.end(), [&]() { return static_cast<uint8_t>(rgen()); });

  // Create receive buffer.
  rx_buffer_spy    rm_buffer_spy;
  unique_rx_buffer rm_buffer(rm_buffer_spy);

  // Resource grid spy.
  resource_grid_reader_spy rg_spy;

  // Process PDU.
  pusch_processor_result_notifier_spy result_notifier;
  pusch_proc_info->process(transport_block, std::move(rm_buffer), result_notifier, rg_spy, pdu);

  // Clear decoder spy entries - It makes sure the soft buffer is unlocked before destroying the pool.
  decoder_spy_info->clear();
}

TEST_P(PuschProcessorFixture, HealthTestFormatterDebug)
{
  // Generate transport block.
  std::vector<uint8_t> transport_block(tbs_dist(rgen));
  std::generate(transport_block.begin(), transport_block.end(), [&]() { return static_cast<uint8_t>(rgen()); });

  // Create receive buffer.
  rx_buffer_spy    rm_buffer_spy;
  unique_rx_buffer rm_buffer(rm_buffer_spy);

  // Resource grid spy.
  resource_grid_reader_spy rg_spy;

  // Process PDU.
  pusch_processor_result_notifier_spy result_notifier;
  pusch_proc_debug->process(transport_block, std::move(rm_buffer), result_notifier, rg_spy, pdu);

  // Clear decoder spy entries - It makes sure the soft buffer is unlocked before destroying the pool.
  decoder_spy_debug->clear();
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(
    PuschProcessor,
    PuschProcessorFixture,
    ::testing::Combine(
        // Subcarrier spacing.
        ::testing::Values(subcarrier_spacing::kHz15, subcarrier_spacing::kHz30, subcarrier_spacing::kHz60),
        // Cyclic Prefix.
        ::testing::Values(cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED),
        // Modulation scheme.
        ::testing::Values(modulation_scheme::QPSK,
                          modulation_scheme::QAM16,
                          modulation_scheme::QAM64,
                          modulation_scheme::QAM256),
        // PUSCH codeword present.
        ::testing::Values(true, false),
        // Number of HARQ-ACK bits.
        ::testing::Values(0, 1, 2, 10),
        // Number of CSI Part1 bits.
        ::testing::Values(0, 4),
        // Number of receive antenna ports.
        ::testing::Values(1, 2, 4)));

} // namespace
