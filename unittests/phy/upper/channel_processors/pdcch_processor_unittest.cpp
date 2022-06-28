/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../resource_grid_test_doubles.h"
#include "../signal_processors/dmrs_pdcch_processor_test_doubles.h"
#include "pdcch_encoder_test_doubles.h"
#include "pdcch_modulator_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_processor.h"
#include <random>

static std::mt19937 rgen(0);

namespace srsgnb {
struct pdcch_processor_config_t {
  std::unique_ptr<pdcch_encoder>        encoder;
  std::unique_ptr<pdcch_modulator>      modulator;
  std::unique_ptr<dmrs_pdcch_processor> dmrs;
};
std::unique_ptr<pdcch_processor> create_pdcch_processor(pdcch_processor_config_t& config);

class pdcch_processor_impl
{
public:
  static bounded_bitset<MAX_RB> compute_rb_mask(const pdcch_processor::coreset_description& coreset,
                                                const pdcch_processor::dci_description&     dci);
};

} // namespace srsgnb

using namespace srsgnb;

int main()
{
  // Random distributions.
  std::uniform_int_distribution<unsigned> dist_numerology(0, 4);
  std::uniform_int_distribution<unsigned> dist_sfn(0, 1023);
  std::uniform_int_distribution<unsigned> dist_subframe(0, 9);
  std::uniform_int_distribution<unsigned> dist_slot(0, 15);
  std::uniform_int_distribution<unsigned> dist_n_id(0, 65535);
  std::uniform_real_distribution<float>   dist_power_offset(-6, +6);
  std::uniform_int_distribution<unsigned> dist_payload_sz(15, 127);
  std::uniform_int_distribution<uint8_t>  dist_payload(0, 1);
  std::uniform_int_distribution<unsigned> dist_port_idx(0, 15);

  // Create dummies and spies.
  resource_grid_dummy       grid;
  pdcch_modulator_spy*      modulator = new pdcch_modulator_spy;
  pdcch_encoder_spy*        encoder   = new pdcch_encoder_spy;
  dmrs_pdcch_processor_spy* dmrs      = new dmrs_pdcch_processor_spy;

  // Create PDCCH processor configuration.
  pdcch_processor_config_t config = {};
  config.encoder                  = std::unique_ptr<pdcch_encoder>(encoder);
  config.modulator                = std::unique_ptr<pdcch_modulator>(modulator);
  config.dmrs                     = std::unique_ptr<dmrs_pdcch_processor>(dmrs);

  // Create generic PDCCH processor.
  std::unique_ptr<pdcch_processor> pdcch = create_pdcch_processor(config);

  for (unsigned bwp_size_rb : {52, 104}) {
    for (unsigned bwp_start_rb : {0, 1}) {
      for (unsigned start_symbol_index : {0, 1}) {
        for (unsigned duration : {1, 2, 3}) {
          for (cyclic_prefix cp : {cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED}) {
            for (unsigned aggregation_level : {1, 2, 4, 8, 16}) {
              for (auto cce_to_reg_mapping_type : {pdcch_processor::coreset_description::CORESET0,
                                                   pdcch_processor::coreset_description::NON_INTERLEAVED,
                                                   pdcch_processor::coreset_description::INTERLEAVED}) {
                // Skip if it cannot fit a candidate.
                if (duration * (bwp_size_rb / 6) < aggregation_level) {
                  continue;
                }

                unsigned numerology  = dist_numerology(rgen);
                bool     is_coreset0 = (cce_to_reg_mapping_type == pdcch_processor::coreset_description::CORESET0);
                unsigned E =
                    aggregation_level * pdcch_constants::NOF_REG_PER_CCE * pdcch_constants::NOF_RE_PDCCH_PER_RB * 2;

                bounded_bitset<pdcch_constants::MAX_NOF_FREQ_RESOURCES> frequency_resources(
                    pdcch_constants::MAX_NOF_FREQ_RESOURCES);
                frequency_resources.fill(0, (12 * (bwp_size_rb / 12)) / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE);

                pdcch_processor::pdu_t pdu = {};
                pdu.slot = {numerology, dist_sfn(rgen), dist_subframe(rgen), dist_slot(rgen) % (1 << numerology)};
                pdu.cp   = cp;

                pdcch_processor::coreset_description& coreset = pdu.coreset;
                coreset.bwp_size_rb                           = is_coreset0 ? (12 * (bwp_size_rb / 12)) : bwp_size_rb;
                coreset.bwp_start_rb                          = bwp_start_rb;
                coreset.start_symbol_index                    = start_symbol_index;
                coreset.duration                              = duration;
                coreset.frequency_resources                   = frequency_resources;
                coreset.cce_to_reg_mapping_type               = cce_to_reg_mapping_type;
                coreset.reg_bundle_size                       = 6;
                coreset.interleaver_size                      = 2;
                coreset.shift_index                           = 0;

                pdu.dci_list.resize(1);
                pdcch_processor::dci_description& dci = pdu.dci_list[0];
                dci.n_id_pdcch_dmrs                   = dist_n_id(rgen);
                dci.n_id_pdcch_data                   = dist_n_id(rgen);
                dci.n_rnti                            = dist_n_id(rgen);
                dci.cce_index                         = 0;
                dci.aggregation_level                 = aggregation_level;
                dci.dmrs_power_offset_dB              = dist_power_offset(rgen);
                dci.data_power_offset_dB              = dist_power_offset(rgen);
                dci.payload.resize(dist_payload_sz(rgen));
                for (uint8_t& bit : dci.payload) {
                  bit = dist_payload(rgen);
                }
                dci.payload.resize(dist_payload_sz(rgen));
                dci.ports.emplace_back(dist_port_idx(rgen));

                // Reset spy classes.
                encoder->reset();
                modulator->reset();
                dmrs->reset();

                // Process PDU.
                pdcch->process(grid, pdu);

                // Calculate ideal allocation mask.
                bounded_bitset<MAX_RB> rb_mask = pdcch_processor_impl::compute_rb_mask(pdu.coreset, pdu.dci_list[0]);

                // Check PDCCH encoder inputs.
                TESTASSERT_EQ(encoder->get_nof_entries(), 1);
                const auto& encoder_entry = encoder->get_entries().front();
                TESTASSERT_EQ(encoder_entry.config.rnti, dci.rnti);
                TESTASSERT_EQ(encoder_entry.config.E, E);
                TESTASSERT_EQ(const_span<uint8_t>(encoder_entry.data), const_span<uint8_t>(pdu.dci_list[0].payload));

                // Check PDCCH modulator inputs.
                TESTASSERT_EQ(modulator->get_nof_entries(), 1);
                const auto& modulator_entry = modulator->get_entries().front();
                TESTASSERT_EQ(rb_mask, modulator_entry.config.rb_mask);
                TESTASSERT_EQ(modulator_entry.config.start_symbol_index, start_symbol_index);
                TESTASSERT_EQ(modulator_entry.config.duration, duration);
                TESTASSERT_EQ(modulator_entry.config.n_id, dci.n_id_pdcch_data);
                TESTASSERT_EQ(modulator_entry.config.n_rnti, dci.n_rnti);
                TESTASSERT_EQ(modulator_entry.config.scaling, convert_dB_to_amplitude(dci.data_power_offset_dB));
                TESTASSERT_EQ(const_span<uint8_t>(pdu.dci_list[0].ports),
                              const_span<uint8_t>(modulator_entry.config.ports));
                TESTASSERT_EQ(const_span<uint8_t>(modulator_entry.bits), const_span<uint8_t>(encoder_entry.encoded));
                TESTASSERT_EQ((void*)modulator_entry.grid_ptr, (void*)&grid);

                // Check PDCCH DMRS inputs.
                TESTASSERT_EQ(dmrs->get_nof_entries(), 1);
                const auto& dmrs_entry = dmrs->get_entries().front();
                TESTASSERT_EQ(dmrs_entry.config.slot, pdu.slot);
                TESTASSERT_EQ(dmrs_entry.config.cp, cp);
                TESTASSERT_EQ(dmrs_entry.config.reference_point_k_rb, is_coreset0 ? coreset.bwp_start_rb : 0);
                TESTASSERT_EQ(dmrs_entry.config.rb_mask, rb_mask);
                TESTASSERT_EQ(dmrs_entry.config.start_symbol_index, coreset.start_symbol_index);
                TESTASSERT_EQ(dmrs_entry.config.duration, coreset.duration);
                TESTASSERT_EQ(dmrs_entry.config.n_id, dci.n_id_pdcch_dmrs);
                TESTASSERT_EQ(dmrs_entry.config.amplitude, convert_dB_to_amplitude(dci.dmrs_power_offset_dB));
                TESTASSERT_EQ(const_span<uint8_t>(pdu.dci_list[0].ports), const_span<uint8_t>(dmrs_entry.config.ports));
              }
            }
          }
        }
      }
    }
  }
  return 0;
}
