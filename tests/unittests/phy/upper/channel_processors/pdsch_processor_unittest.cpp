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

#include "../../support/resource_grid_test_doubles.h"
#include "../signal_processors/dmrs_pdsch_processor_test_doubles.h"
#include "../tx_buffer_test_doubles.h"
#include "pdsch_encoder_test_doubles.h"
#include "pdsch_modulator_test_doubles.h"
#include "pdsch_processor_test_doubles.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/srsvec/compare.h"
#include <fmt/ostream.h>
#include <gtest/gtest.h>
#include <random>

using namespace srsran;

namespace srsran {
static std::ostream& operator<<(std::ostream& os, cyclic_prefix value)
{
  fmt::print(os, value.to_string());
  return os;
}

static std::ostream& operator<<(std::ostream& os, const dmrs_type& value)
{
  fmt::print(os, "{}", value.to_string());
  return os;
}

static std::ostream& operator<<(std::ostream& os, span<const uint8_t> value)
{
  fmt::print(os, "[{}]", value);
  return os;
}

static bool operator==(const span<const uint8_t> lhs, const span<const uint8_t> rhs)
{
  return srsvec::equal(lhs, rhs);
}

} // namespace srsran

namespace {

using PdschProcessorParams = std::tuple<unsigned, cyclic_prefix, dmrs_type>;

class PdschProcessorFixture : public testing::TestWithParam<PdschProcessorParams>
{
protected:
  void SetUp() override
  {
    // Extract parameters.
    nof_layers      = std::get<0>(GetParam());
    cp              = std::get<1>(GetParam());
    dmrs_type_value = std::get<2>(GetParam());
  }

  unsigned      nof_layers;
  cyclic_prefix cp;
  dmrs_type     dmrs_type_value;

  static float get_power()
  {
    static std::uniform_real_distribution<float> dist_power(-6.0f, 6.0f);
    float                                        result = 0.0f;
    while (!std::isnormal(result)) {
      result = dist_power(rgen);
    }
    return result;
  }

  static constexpr float                         amplitude_max_error = 1e-6;
  static std::mt19937                            rgen;
  static std::uniform_int_distribution<uint8_t>  dist_payload;
  static std::uniform_int_distribution<uint8_t>  dist_bool;
  static std::uniform_int_distribution<unsigned> dist_u16;
  static std::uniform_int_distribution<unsigned> dist_start_symb;
  static std::uniform_int_distribution<unsigned> dist_rv;
  static std::uniform_int_distribution<unsigned> dist_mod;
};

static const std::array<modulation_scheme, 5> modulations = {modulation_scheme::QPSK,
                                                             modulation_scheme::QAM16,
                                                             modulation_scheme::QAM64,
                                                             modulation_scheme::QAM256};

std::mt19937                            PdschProcessorFixture::rgen;
std::uniform_int_distribution<uint8_t>  PdschProcessorFixture::dist_payload(0, UINT8_MAX);
std::uniform_int_distribution<uint8_t>  PdschProcessorFixture::dist_bool(0, 1);
std::uniform_int_distribution<unsigned> PdschProcessorFixture::dist_u16(0, UINT16_MAX);
std::uniform_int_distribution<unsigned> PdschProcessorFixture::dist_start_symb(0, 1);
std::uniform_int_distribution<unsigned> PdschProcessorFixture::dist_rv(0, 3);
std::uniform_int_distribution<unsigned> PdschProcessorFixture::dist_mod(0, modulations.size() - 1);

TEST_P(PdschProcessorFixture, UnitTest)
{
  std::shared_ptr<pdsch_encoder_factory_spy>   encoder_factory_spy   = std::make_shared<pdsch_encoder_factory_spy>();
  std::shared_ptr<pdsch_modulator_factory_spy> modulator_factory_spy = std::make_shared<pdsch_modulator_factory_spy>();
  std::shared_ptr<dmrs_pdsch_processor_factory_spy> dmrs_factory_spy =
      std::make_shared<dmrs_pdsch_processor_factory_spy>();

  std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory =
      create_pdsch_processor_factory_sw(std::shared_ptr<pdsch_encoder_factory>(encoder_factory_spy),
                                        std::shared_ptr<pdsch_modulator_factory>(modulator_factory_spy),
                                        std::shared_ptr<dmrs_pdsch_processor_factory>(dmrs_factory_spy));
  ASSERT_NE(pdsch_proc_factory, nullptr);

  std::unique_ptr<pdsch_processor> pdsch = pdsch_proc_factory->create();
  ASSERT_NE(pdsch, nullptr);

  pdsch_encoder_spy*           encoder_spy   = encoder_factory_spy->get_entries().front();
  pdsch_modulator_spy*         modulator_spy = modulator_factory_spy->get_entries().front();
  dmrs_pdsch_processor_spy*    dmrs_spy      = dmrs_factory_spy->get_entries().front();
  pdsch_processor_notifier_spy notifier;

  unsigned nof_codewords    = (nof_layers > 4) ? 2 : 1;
  unsigned nof_symbols_slot = get_nsymb_per_slot(cp);

  // Prepare PDSCH PDU.
  pdsch_processor::pdu_t pdu;
  pdu.slot         = {0, 0};
  pdu.rnti         = 0x1234;
  pdu.bwp_size_rb  = 52;
  pdu.bwp_start_rb = 1;
  pdu.cp           = cp;
  for (unsigned codeword = 0; codeword != nof_codewords; ++codeword) {
    pdu.codewords.emplace_back();
    pdu.codewords.back().modulation = modulations[dist_mod(rgen)];
    pdu.codewords.back().rv         = dist_rv(rgen);
  }
  pdu.n_id             = 0;
  pdu.precoding        = precoding_configuration::make_wideband(make_identity(nof_layers));
  pdu.ref_point        = dist_bool(rgen) ? pdsch_processor::pdu_t::PRB0 : pdsch_processor::pdu_t::CRB0;
  pdu.dmrs_symbol_mask = symbol_slot_mask(nof_symbols_slot);
  while (pdu.dmrs_symbol_mask.none()) {
    pdu.dmrs_symbol_mask.set(2, static_cast<bool>(dist_bool(rgen)));
    pdu.dmrs_symbol_mask.set(7, static_cast<bool>(dist_bool(rgen)));
    pdu.dmrs_symbol_mask.set(11, static_cast<bool>(dist_bool(rgen)));
  }
  pdu.dmrs                        = dmrs_type_value;
  pdu.scrambling_id               = dist_u16(rgen);
  pdu.n_scid                      = static_cast<bool>(dist_bool(rgen));
  pdu.nof_cdm_groups_without_data = 1;
  pdu.freq_alloc                  = rb_allocation::make_custom({1, 2, 3, 4});
  pdu.start_symbol_index          = dist_start_symb(rgen);
  pdu.nof_symbols                 = get_nsymb_per_slot(cp) - pdu.start_symbol_index;
  pdu.ldpc_base_graph             = static_cast<ldpc_base_graph_type>(dist_bool(rgen));
  pdu.tbs_lbrm_bytes              = ldpc::MAX_CODEBLOCK_SIZE / 8;
  pdu.reserved                    = {};
  pdu.ratio_pdsch_dmrs_to_sss_dB  = get_power();
  pdu.ratio_pdsch_data_to_sss_dB  = get_power();

  // Generate reserved element pattern for DM-RS.
  re_pattern dmrs_reserved_pattern = pdu.dmrs.get_dmrs_pattern(
      pdu.bwp_start_rb, pdu.bwp_size_rb, pdu.nof_cdm_groups_without_data, pdu.dmrs_symbol_mask);

  // Create overall reserved pattern.
  re_pattern_list reserved = pdu.reserved;
  reserved.merge(dmrs_reserved_pattern);

  // Get physical RB allocation mask.
  bounded_bitset<MAX_RB> rb_mask = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Count number of resource elements.
  unsigned Nre = pdu.freq_alloc.get_nof_rb() * NRE * pdu.nof_symbols -
                 reserved.get_inclusion_count(pdu.start_symbol_index, pdu.nof_symbols, rb_mask);

  static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS> data;

  // Generate random data and prepare TB.
  std::vector<uint8_t> data0(16);
  for (uint8_t& byte : data0) {
    byte = dist_payload(rgen);
  }
  data.emplace_back(data0);

  std::vector<uint8_t> data1(32);
  if (nof_codewords > 1) {
    for (uint8_t& byte : data1) {
      byte = dist_payload(rgen);
    }
    data.emplace_back(data1);
  }

  // Use resource grid dummy, the PDSCH processor must not call any method.
  resource_grid_dummy   rg_dummy;
  resource_grid_mapper& mapper_dummy = rg_dummy.get_mapper();

  // Reset spies.
  encoder_spy->reset();
  modulator_spy->reset();
  dmrs_spy->reset();
  notifier.reset();

  tx_buffer_spy    rm_buffer_spy(0, 0);
  unique_tx_buffer rm_buffer(rm_buffer_spy);

  // Process PDU.
  pdsch->process(mapper_dummy, std::move(rm_buffer), notifier, data, pdu);

  // Wait for the processor to finish.
  notifier.wait_for_finished();

  unsigned nof_layers_cw0 = nof_layers / nof_codewords;
  unsigned nof_layers_cw1 = nof_layers - nof_layers_cw0;

  // Validate encoder.
  {
    ASSERT_EQ(encoder_spy->get_nof_entries(), nof_codewords);
    const auto& entries = encoder_spy->get_entries();
    for (unsigned codeword = 0; codeword != nof_codewords; ++codeword) {
      const auto& entry = entries[codeword];
      ASSERT_EQ(entry.config.base_graph, pdu.ldpc_base_graph);
      ASSERT_EQ(entry.config.rv, pdu.codewords[codeword].rv);
      ASSERT_EQ(entry.config.mod, pdu.codewords[codeword].modulation);
      ASSERT_EQ(entry.config.Nref, pdu.tbs_lbrm_bytes * 8);
      ASSERT_EQ(entry.config.nof_layers, codeword == 0 ? nof_layers_cw0 : nof_layers_cw1);
      ASSERT_EQ(entry.config.nof_ch_symbols, Nre * entry.config.nof_layers);
      ASSERT_EQ(span<const uint8_t>(entry.transport_block), span<const uint8_t>(data[codeword]));
    }
  }

  // Validate modulator.
  {
    ASSERT_EQ(modulator_spy->get_nof_entries(), 1);
    const auto& entries = modulator_spy->get_entries();
    const auto& entry   = entries.front();
    ASSERT_EQ(entry.config.rnti, pdu.rnti);
    ASSERT_EQ(entry.config.bwp_size_rb, pdu.bwp_size_rb);
    ASSERT_EQ(entry.config.bwp_start_rb, pdu.bwp_start_rb);
    ASSERT_EQ(entry.config.modulation1, pdu.codewords[0].modulation);
    if (nof_codewords > 1) {
      ASSERT_EQ(entry.config.modulation2, pdu.codewords[1].modulation);
    }
    ASSERT_EQ(entry.config.freq_allocation, pdu.freq_alloc);
    ASSERT_EQ(entry.config.start_symbol_index, pdu.start_symbol_index);
    ASSERT_EQ(entry.config.nof_symbols, pdu.nof_symbols);
    ASSERT_EQ(entry.config.n_id, pdu.n_id);
    ASSERT_NEAR(entry.config.scaling, convert_dB_to_amplitude(-pdu.ratio_pdsch_data_to_sss_dB), amplitude_max_error);
    ASSERT_EQ(entry.config.reserved, pdu.reserved);
    ASSERT_EQ(entry.config.precoding, pdu.precoding);
    ASSERT_EQ(entry.mapper, &mapper_dummy);
    for (unsigned codeword = 0; codeword != nof_codewords; ++codeword) {
      span<const uint8_t> codeword_encoder   = encoder_spy->get_entries()[codeword].codeword;
      const bit_buffer    codeword_modulator = entry.codewords[codeword];
      for (unsigned i_bit = 0, i_bit_end = codeword_encoder.size(); i_bit != i_bit_end; ++i_bit) {
        ASSERT_EQ(codeword_encoder[i_bit], codeword_modulator.extract(i_bit, 1));
      }
    }
  }

  // Validate DM-RS processor.
  {
    ASSERT_EQ(dmrs_spy->get_nof_entries(), 1);
    const auto& entries = dmrs_spy->get_entries();
    auto&       entry   = entries.front();
    ASSERT_EQ(entry.config.slot, pdu.slot);
    ASSERT_EQ(entry.config.reference_point_k_rb,
              (pdu.ref_point == pdsch_processor::pdu_t::PRB0) ? pdu.bwp_start_rb : 0);
    ASSERT_EQ(entry.config.type, pdu.dmrs);
    ASSERT_EQ(entry.config.scrambling_id, pdu.scrambling_id);
    ASSERT_EQ(entry.config.n_scid, pdu.n_scid);
    ASSERT_NEAR(entry.config.amplitude, convert_dB_to_amplitude(-pdu.ratio_pdsch_dmrs_to_sss_dB), amplitude_max_error);
    ASSERT_EQ(entry.config.symbols_mask, pdu.dmrs_symbol_mask);
    ASSERT_EQ(entry.config.rb_mask, rb_mask);
    ASSERT_EQ(entry.config.precoding, pdu.precoding);
    ASSERT_EQ(entry.mapper, &mapper_dummy);
  }
}

INSTANTIATE_TEST_SUITE_P(PdschProcessorUnittest,
                         PdschProcessorFixture,
                         testing::Combine(testing::Values(1, 2),
                                          testing::Values(cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED),
                                          testing::Values(dmrs_type::options::TYPE1)));

} // namespace
