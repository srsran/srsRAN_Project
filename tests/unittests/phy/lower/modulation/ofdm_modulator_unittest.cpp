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

#include "../../generic_functions/dft_processor_test_doubles.h"
#include "../../support/resource_grid_test_doubles.h"
#include "srsran/phy/lower/modulation/modulation_factories.h"
#include "srsran/srsvec/compare.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/sc_prod.h"
#include "srsran/support/test_utils.h"
#include <random>

/// Defines the maximum allowed error at the OFDM modulator output.
static constexpr float ASSERT_MAX_ERROR = 1e-6;

using namespace srsran;

int main()
{
  std::mt19937                            rgen(0);
  std::uniform_real_distribution<float>   dist_rg(-1, 1);
  std::uniform_int_distribution<unsigned> dist_port(0, MAX_PORTS - 1);

  // Create DFT factory spy.
  std::shared_ptr<dft_processor_factory_spy> dft_factory = std::make_shared<dft_processor_factory_spy>();
  TESTASSERT(dft_factory);

  // Prepare OFDM modulator factory configuration.
  ofdm_factory_generic_configuration ofdm_common_config;
  ofdm_common_config.dft_factory = dft_factory;

  // Create OFDM modulator factory.
  std::shared_ptr<ofdm_modulator_factory> ofdm_factory = create_ofdm_modulator_factory_generic(ofdm_common_config);
  TESTASSERT(ofdm_factory);

  // Iterate all possible numerologies.
  for (subcarrier_spacing scs : {subcarrier_spacing::kHz15,
                                 subcarrier_spacing::kHz30,
                                 subcarrier_spacing::kHz60,
                                 subcarrier_spacing::kHz120,
                                 subcarrier_spacing::kHz240}) {
    // Iterate all possible FFT sizes.
    for (unsigned dft_size : {256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192, 12288}) {
      // Skip combinations of SCS>15kHz with large DFT sizes.
      if (scs > subcarrier_spacing::kHz15 && dft_size > 4096) {
        continue;
      }

      // Iterate all possible cyclic prefix.
      for (cyclic_prefix cp : {cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED}) {
        // Skip invalid CP, numerology and DFT size combinations that invalid.
        if (!cp.is_valid(scs, dft_size)) {
          fmt::print("Unsupported cyclic prefix ({}), SCS ({} KHz) and DFT size ({}) combination. Skipping.\n",
                     cp.to_string().c_str(),
                     scs_to_khz(scs),
                     dft_size);
          continue;
        }

        // Reset spies.
        dft_factory->clear_entries();

        // Create OFDM modulator configuration. Use minimum number of RB.
        ofdm_modulator_configuration ofdm_config = {};
        ofdm_config.numerology                   = to_numerology_value(scs);
        ofdm_config.bw_rb                        = 11;
        ofdm_config.dft_size                     = dft_size;
        ofdm_config.cp                           = cp;
        ofdm_config.scale                        = dist_rg(rgen);

        unsigned nsubc = ofdm_config.bw_rb * NRE;

        // Create OFDM modulator.
        std::unique_ptr<ofdm_slot_modulator> ofdm = ofdm_factory->create_ofdm_slot_modulator(ofdm_config);
        TESTASSERT(ofdm != nullptr);

        // Check is a DFT processor is created and not used.
        auto& dft_processor_factory_entry = dft_factory->get_entries();
        TESTASSERT(dft_processor_factory_entry.size() == 1);
        dft_processor_spy& dft = *dft_processor_factory_entry[0].dft;
        TESTASSERT(dft.get_entries().empty());

        // Define empty guard band.
        std::vector<cf_t> zero_guard(dft_size - nsubc);

        // Iterate all slots within a subframe.
        for (unsigned slot_idx = 0, nslot = get_nof_slots_per_subframe(scs); slot_idx != nslot; ++slot_idx) {
          // Select a random port.
          unsigned port_idx = dist_port(rgen);

          // Generate random data in the resource grid.
          unsigned                 nsymb = get_nsymb_per_slot(cp);
          resource_grid_reader_spy rg(1, nsymb, nsubc);
          for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
            for (unsigned subc_idx = 0; subc_idx != nsubc; ++subc_idx) {
              resource_grid_reader_spy::expected_entry_t entry = {};
              entry.port                                       = port_idx;
              entry.symbol                                     = symbol_idx;
              entry.subcarrier                                 = subc_idx;
              entry.value                                      = {dist_rg(rgen), dist_rg(rgen)};
              rg.write(entry);
            }
          }

          // Reset DFT spy entries.
          dft.clear_entries();

          // Modulate signal.
          std::vector<cf_t> output(ofdm->get_slot_size(slot_idx));
          ofdm->modulate(output, rg, port_idx, slot_idx);

          // Check the number of calls to DFT processor match with the number of symbols.
          TESTASSERT(dft.get_entries().size() == nsymb);

          // Iterate all symbols.
          unsigned offset      = 0;
          auto     dft_entries = dft.get_entries();
          for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
            // Get expected data from RG.
            std::vector<cf_t> rg_data_symbol(nsubc);
            rg.get(rg_data_symbol, port_idx, symbol_idx, 0);
            span<cf_t> rg_data = rg_data_symbol;

            // Get DFT input.
            span<const cf_t> dft_input = dft_entries[symbol_idx].input;

            // Verify DFT input.
            TESTASSERT(srsvec::equal(rg_data.first(nsubc / 2), dft_input.last(nsubc / 2)));
            TESTASSERT(srsvec::equal(rg_data.last(nsubc / 2), dft_input.first(nsubc / 2)));
            TESTASSERT(srsvec::equal(zero_guard, dft_input.subspan(nsubc / 2, zero_guard.size())));

            // Generate ideal time domain output.
            unsigned cp_len =
                cp.get_length(nsymb * slot_idx + symbol_idx, scs).to_samples(to_sampling_rate_Hz(scs, dft_size));
            std::vector<cf_t> expected_output_data(dft_size + cp_len);
            span<cf_t>        expected_output = expected_output_data;
            srsvec::sc_prod(dft_entries[symbol_idx].output, ofdm_config.scale, expected_output.last(dft_size));
            srsvec::copy(expected_output.first(cp_len), expected_output.last(cp_len));

            // Select a view of the OFDM modulator output.
            span<const cf_t> output_symbol(output);
            output_symbol = output_symbol.subspan(offset, dft_size + cp_len);

            // Assert generated symbol matches ideal.
            for (unsigned idx = 0, nsamples = dft_size + cp_len; idx != nsamples; ++idx) {
              float err = std::abs(expected_output[idx] - output_symbol[idx]);
              TESTASSERT(err < ASSERT_MAX_ERROR);
            }

            // Increment OFDM symbol offset.
            offset += dft_size + cp_len;
          }
        }
      }
    }
  }

  return 0;
}
