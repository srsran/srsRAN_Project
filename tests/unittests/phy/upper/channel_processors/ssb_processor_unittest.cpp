/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "../signal_processors/dmrs_pbch_processor_doubles.h"
#include "../signal_processors/pss_processor_doubles.h"
#include "../signal_processors/sss_processor_doubles.h"
#include "pbch_encoder_doubles.h"
#include "pbch_modulator_doubles.h"
#include "srsran/phy/upper/channel_processors/ssb_processor.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/srsvec/compare.h"
#include <random>

using namespace srsran;

static std::mt19937 rgen(0);

int main()
{
  std::uniform_int_distribution<unsigned> sfn_dist(0, 1023);
  std::uniform_int_distribution<unsigned> pci_dist(0, phys_cell_id::NOF_NID - 1);
  std::uniform_int_distribution<unsigned> bit_dist(0, 1);
  std::uniform_int_distribution<unsigned> port_dist(0, 63);
  std::uniform_int_distribution<unsigned> offset_to_pointA_dist(0, 2199);
  std::uniform_int_distribution<unsigned> ssb_subcarrier_offset_dist(0, 11);

  resource_grid_writer_spy grid(0, 0, 0);

  std::shared_ptr<pbch_encoder_factory_spy>   encoder_factory_spy   = std::make_shared<pbch_encoder_factory_spy>();
  std::shared_ptr<pbch_modulator_factory_spy> modulator_factory_spy = std::make_shared<pbch_modulator_factory_spy>();
  std::shared_ptr<dmrs_pbch_processor_factory_spy> dmrs_factory_spy =
      std::make_shared<dmrs_pbch_processor_factory_spy>();
  std::shared_ptr<pss_processor_factory_spy> pss_factory_spy = std::make_shared<pss_processor_factory_spy>();
  std::shared_ptr<sss_processor_factory_spy> sss_factory_spy = std::make_shared<sss_processor_factory_spy>();

  // Create channel processors - SSB
  ssb_processor_factory_sw_configuration ssb_factory_config;
  ssb_factory_config.encoder_factory                      = encoder_factory_spy;
  ssb_factory_config.modulator_factory                    = modulator_factory_spy;
  ssb_factory_config.dmrs_factory                         = dmrs_factory_spy;
  ssb_factory_config.pss_factory                          = pss_factory_spy;
  ssb_factory_config.sss_factory                          = sss_factory_spy;
  std::shared_ptr<ssb_processor_factory> ssb_proc_factory = create_ssb_processor_factory_sw(ssb_factory_config);
  TESTASSERT(ssb_proc_factory);

  std::unique_ptr<ssb_processor> pbch = ssb_proc_factory->create();
  TESTASSERT(pbch);

  pbch_encoder_spy*        encoder   = encoder_factory_spy->get_entries().front();
  pbch_modulator_spy*      modulator = modulator_factory_spy->get_entries().front();
  dmrs_pbch_processor_spy* dmrs      = dmrs_factory_spy->get_entries().front();
  pss_processor_spy*       pss       = pss_factory_spy->get_entries().front();
  sss_processor_spy*       sss       = sss_factory_spy->get_entries().front();

  // Iterate all possible SSB pattern cases.
  for (const ssb_pattern_case& pattern_case :
       {ssb_pattern_case::A, ssb_pattern_case::B, ssb_pattern_case::C, ssb_pattern_case::D, ssb_pattern_case::E}) {
    // Obtain the frequency range from the pattern case.
    frequency_range freq_range = to_frequency_range(pattern_case);

    // Iterate possible L_max
    for (unsigned L_max : {4, 8, 64}) {
      // Iterate possible beta PSS
      for (float beta_pss : {0.0F, -3.0F}) {
        // Iterate over possible values of Common SCS (TS38.331 subCarrierSpacingCommon in MIB).
        for (subcarrier_spacing common_scs : {subcarrier_spacing::kHz15,
                                              subcarrier_spacing::kHz30,
                                              subcarrier_spacing::kHz60,
                                              subcarrier_spacing::kHz120}) {
          // Select numerology from case.
          subcarrier_spacing ssb_scs = to_subcarrier_spacing(pattern_case);

          // Skip invalid pattern and L_max combinations.
          if (pattern_case != ssb_pattern_case::E && pattern_case != ssb_pattern_case::D && L_max == 64) {
            continue;
          }

          // Skip invalid frequency range and common SCS combinations.
          if (!is_scs_valid(common_scs, freq_range)) {
            continue;
          }

          // Iterate over all possible SS/PBCH block candidates.
          for (unsigned ssb_idx = 0; ssb_idx < L_max; ++ssb_idx) {
            ssb_offset_to_pointA  offset_pointA     = offset_to_pointA_dist(rgen);
            ssb_subcarrier_offset subcarrier_offset = ssb_subcarrier_offset_dist(rgen);

            // Round the SS/PBCH block subcarrier offset to avoid incompatible subcarrier offsets.
            if (freq_range == frequency_range::FR1 && ssb_scs != subcarrier_spacing::kHz15) {
              subcarrier_offset = 2 * (subcarrier_offset.to_uint() / 2);
            } else if (freq_range == frequency_range::FR2) {
              subcarrier_offset = 4 * (subcarrier_offset.to_uint() / 4);
            }

            // Compute secondary variables.
            unsigned ssb_first_symbol_burst = ssb_get_l_first(pattern_case, ssb_idx);
            unsigned nslots_in_subframe     = get_nof_slots_per_subframe(ssb_scs);
            unsigned slot_in_burst          = ssb_first_symbol_burst / get_nsymb_per_slot(cyclic_prefix::NORMAL);
            unsigned subframe_in_burst      = slot_in_burst / nslots_in_subframe;
            unsigned slot_in_subframe       = slot_in_burst % nslots_in_subframe;

            // Compute secondary assertion values.
            unsigned ssb_first_subcarrier =
                ssb_get_k_first(freq_range, ssb_scs, common_scs, offset_pointA, subcarrier_offset);
            unsigned ssb_first_symbol_slot = ssb_first_symbol_burst % get_nsymb_per_slot(cyclic_prefix::NORMAL);

            // Iterate half frames.
            for (unsigned subframe : {0 + subframe_in_burst, 5 + subframe_in_burst}) {
              // Generate PBCH PDU.
              ssb_processor::pdu_t pdu = {};
              pdu.slot                 = {to_numerology_value(ssb_scs), sfn_dist(rgen), subframe, slot_in_subframe};
              pdu.phys_cell_id         = pci_dist(rgen);
              pdu.beta_pss             = beta_pss;
              pdu.ssb_idx              = ssb_idx;
              pdu.L_max                = L_max;
              pdu.common_scs           = common_scs;
              pdu.subcarrier_offset    = subcarrier_offset;
              pdu.offset_to_pointA     = offset_pointA;
              pdu.pattern_case         = pattern_case;
              pdu.ports.emplace_back(port_dist(rgen));
              for (uint8_t& bit : pdu.bch_payload) {
                bit = bit_dist(rgen);
              }

              // Reset spies.
              encoder->reset();
              modulator->reset();
              dmrs->reset();
              pss->reset();
              sss->reset();

              // Process PDU.
              pbch->process(grid, pdu);

              // Assert modules number of entries.
              TESTASSERT_EQ(encoder->get_nof_entries(), 1);
              TESTASSERT_EQ(modulator->get_nof_entries(), 1);
              TESTASSERT_EQ(dmrs->get_nof_entries(), 1);
              TESTASSERT_EQ(pss->get_nof_entries(), 1);
              TESTASSERT_EQ(sss->get_nof_entries(), 1);

              // Assert encoder.
              const auto& encoder_entry = encoder->get_entries()[0];
              TESTASSERT(encoder_entry.msg.N_id == pdu.phys_cell_id);
              TESTASSERT(encoder_entry.msg.ssb_idx == pdu.ssb_idx);
              TESTASSERT(encoder_entry.msg.L_max == L_max);
              TESTASSERT(encoder_entry.msg.hrf == pdu.slot.is_odd_hrf());
              TESTASSERT(srsvec::equal(encoder_entry.msg.payload, encoder_entry.msg.payload));
              TESTASSERT(encoder_entry.msg.sfn == pdu.slot.sfn());
              TESTASSERT(encoder_entry.msg.k_ssb == pdu.subcarrier_offset);
              TESTASSERT(encoder_entry.encoded.size() == pbch_encoder::E);

              // Assert modulator.
              const auto& modulator_entry = modulator->get_entries()[0];
              TESTASSERT(modulator_entry.config.phys_cell_id == pdu.phys_cell_id);
              TESTASSERT(modulator_entry.config.ssb_idx == pdu.ssb_idx);
              TESTASSERT_EQ(ssb_first_subcarrier, modulator_entry.config.ssb_first_subcarrier);
              TESTASSERT(modulator_entry.config.ssb_first_symbol == ssb_first_symbol_slot);
              TESTASSERT(modulator_entry.config.amplitude == 1.0F);
              TESTASSERT(srsvec::equal(modulator_entry.config.ports, pdu.ports));
              TESTASSERT(srsvec::equal(modulator_entry.bits, encoder_entry.encoded));
              TESTASSERT(modulator_entry.grid_ptr == &grid);

              // Assert DMRS for PBCH.
              const auto& dmrs_entry = dmrs->get_entries()[0];
              TESTASSERT(dmrs_entry.config.phys_cell_id == pdu.phys_cell_id);
              TESTASSERT(dmrs_entry.config.ssb_idx == pdu.ssb_idx);
              TESTASSERT(dmrs_entry.config.L_max == pdu.L_max);
              TESTASSERT(dmrs_entry.config.ssb_first_subcarrier == ssb_first_subcarrier);
              TESTASSERT(dmrs_entry.config.ssb_first_symbol == ssb_first_symbol_slot);
              TESTASSERT(dmrs_entry.config.hrf == pdu.slot.is_odd_hrf());
              TESTASSERT(dmrs_entry.config.amplitude == 1.0F);
              TESTASSERT(srsvec::equal(dmrs_entry.config.ports, pdu.ports));

              // Assert PSS.
              const auto& pss_entry = pss->get_entries()[0];
              TESTASSERT(pss_entry.config.phys_cell_id == pdu.phys_cell_id);
              TESTASSERT(pss_entry.config.ssb_first_subcarrier == ssb_first_subcarrier);
              TESTASSERT(pss_entry.config.ssb_first_symbol == ssb_first_symbol_slot);
              TESTASSERT(pss_entry.config.amplitude == convert_dB_to_amplitude(beta_pss));
              TESTASSERT(srsvec::equal(pss_entry.config.ports, pdu.ports));

              // Assert SSS.
              const auto& sss_entry = sss->get_entries()[0];
              TESTASSERT(sss_entry.config.phys_cell_id == pdu.phys_cell_id);
              TESTASSERT(sss_entry.config.ssb_first_subcarrier == ssb_first_subcarrier);
              TESTASSERT(sss_entry.config.ssb_first_symbol == ssb_first_symbol_slot);
              TESTASSERT(sss_entry.config.amplitude == 1.0F);
              TESTASSERT(srsvec::equal(sss_entry.config.ports, pdu.ports));
            }
          }
        }
      }
    }
  }

  return 0;
}
