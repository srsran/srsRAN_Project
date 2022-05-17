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
#include "srsgnb/srsvec/compare.h"
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
  static void compute_rb_mask(span<bool>                                  mask,
                              const pdcch_processor::coreset_description& coreset,
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
              // Skip if the

              // Skip if it cannot fit a candidate.
              if (duration * (bwp_size_rb / 6) < aggregation_level) {
                continue;
              }

              unsigned numerology = dist_numerology(rgen);

              pdcch_processor::pdu_t pdu = {};
              pdu.slot = {numerology, dist_sfn(rgen), dist_subframe(rgen), dist_slot(rgen) % (1 << numerology)};
              pdu.cp   = cp;

              pdcch_processor::coreset_description& coreset = pdu.coreset;
              coreset.bwp_size_rb                           = bwp_size_rb;
              coreset.bwp_start_rb                          = bwp_start_rb;
              coreset.start_symbol_index                    = start_symbol_index;
              coreset.duration                              = duration;
              std::fill(coreset.frequency_resources.begin() + bwp_start_rb / 6,
                        coreset.frequency_resources.begin() + (bwp_start_rb + bwp_size_rb) / 6,
                        true);
              coreset.cce_to_reg_mapping_type = pdcch_processor::coreset_description::NON_INTERLEAVED;

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

              // Set untested parameters.
              coreset.reg_bundle_size      = 0;
              coreset.interleaver_size     = 0;
              coreset.type                 = pdcch_processor::coreset_description::OTHER;
              coreset.shift_index          = 0;
              coreset.precoder_granularity = pdcch_processor::coreset_description::SAME_AS_REG_BUNDLE;

              // Reset spy classes.
              encoder->reset();
              modulator->reset();
              dmrs->reset();

              // Process PDU.
              pdcch->process(grid, pdu);

              // Calculate ideal allocation mask.
              std::array<bool, MAX_RB> rb_mask;
              pdcch_processor_impl::compute_rb_mask(rb_mask, pdu.coreset, pdu.dci_list[0]);

              // Check PDCCH encoder inputs.
              TESTASSERT(encoder->get_nof_entries() == 1, "Invalid number of entries.");
              const auto& encoder_entry = encoder->get_entries().front();
              TESTASSERT(srsvec::equal(encoder_entry.data, pdu.dci_list[0].payload));

              // Check PDCCH modulator inputs.
              TESTASSERT(modulator->get_nof_entries() == 1, "Invalid number of entries.");
              const auto& modulator_entry = modulator->get_entries().front();
              TESTASSERT(srsvec::equal(rb_mask, modulator_entry.config.rb_mask));
              TESTASSERT(modulator_entry.config.start_symbol_index == start_symbol_index);
              TESTASSERT(modulator_entry.config.duration == duration);
              TESTASSERT(modulator_entry.config.n_id == dci.n_id_pdcch_data);
              TESTASSERT(modulator_entry.config.n_rnti == dci.n_rnti);
              TESTASSERT(modulator_entry.config.scaling == convert_dB_to_amplitude(dci.data_power_offset_dB),
                         "Detected mismatch.");
              TESTASSERT(srsvec::equal(pdu.dci_list[0].ports, modulator_entry.config.ports));
              TESTASSERT(srsvec::equal(modulator_entry.bits, encoder_entry.encoded));
              TESTASSERT(modulator_entry.grid_ptr == &grid);

              // Check PDCCH DMRS inputs.
              TESTASSERT(dmrs->get_nof_entries() == 1, "Invalid number of entries.");
              const auto& dmrs_entry = dmrs->get_entries().front();
              TESTASSERT(dmrs_entry.config.slot == pdu.slot);
              TESTASSERT(dmrs_entry.config.cp == cp);
              TESTASSERT(dmrs_entry.config.reference_point_k_rb == 0);
              TESTASSERT(srsvec::equal(dmrs_entry.config.rb_mask, rb_mask));
              TESTASSERT(dmrs_entry.config.start_symbol_index == coreset.start_symbol_index);
              TESTASSERT(dmrs_entry.config.duration == coreset.duration);
              TESTASSERT(dmrs_entry.config.n_id == dci.n_id_pdcch_dmrs);
              TESTASSERT(dmrs_entry.config.amplitude == convert_dB_to_amplitude(dci.dmrs_power_offset_dB),
                         "Detected mismatch.");
              TESTASSERT(srsvec::equal(pdu.dci_list[0].ports, dmrs_entry.config.ports));
            }
          }
        }
      }
    }
  }
  return 0;
}
