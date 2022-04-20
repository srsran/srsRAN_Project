
#include "../../generic_functions/dft_processor_test_doubles.h"
#include "../../resource_grid_test_doubles.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/support/test_utils.h"
#include <random>

namespace srsgnb {
struct ofdm_modulator_factory_config {
  dft_processor_factory& dft_factory;
};

std::unique_ptr<ofdm_modulator_factory> create_ofdm_modulator_factory(ofdm_modulator_factory_config& config);

} // namespace srsgnb

/// Defines the maximum allowed error at the OFDM modulator output.
static constexpr float ASSERT_MAX_ERROR = 1e-6;

using namespace srsgnb;

int main()
{
  std::mt19937                            rgen(0);
  std::uniform_real_distribution<float>   dist_rg(-1, 1);
  std::uniform_int_distribution<unsigned> dist_port(0, MAX_PORTS - 1);

  // Create DFT factory spy.
  dft_processor_factory_spy dft_factory;

  // Prepare OFDM modulator factory configuration.
  ofdm_modulator_factory_config ofdm_factory_config = {dft_factory};

  // Create OFDM modulator factory.
  std::unique_ptr<ofdm_modulator_factory> ofdm_factory = create_ofdm_modulator_factory(ofdm_factory_config);

  // Iterate all possible numerologies.
  for (unsigned numerology : {0}) {
    // Iterate all possible FFT sizes.
    for (unsigned dft_size : {256}) {
      // Skip combinations of SCS>15kHz with large DFT sizes.
      if (numerology > 0 && dft_size > 4096) {
        continue;
      }

      // Iterate all possible cyclic prefix.
      for (cyclic_prefix cp : {cyclic_prefix::NORMAL}) {
        // Skip invalid CP, numerology and DFT size combinations that invalid.
        if (!cp.is_valid(numerology, dft_size)) {
          printf("Unsupported cyclic prefix (%s), numerology (%d) and DFT size (%d) combination. Skipping.\n",
                 cp.to_string().c_str(),
                 numerology,
                 dft_size);
          continue;
        }

        // Reset spies.
        dft_factory.reset();

        // Create OFDM modulator configuration. Use minimum number of RB.
        ofdm_modulator_configuration ofdm_config = {};
        ofdm_config.numerology                   = numerology;
        ofdm_config.bw_rb                        = 11;
        ofdm_config.dft_size                     = dft_size;
        ofdm_config.cp                           = cp;
        ofdm_config.scale                        = dist_rg(rgen);

        unsigned nsubc = ofdm_config.bw_rb * NRE;

        // Create OFDM modulator.
        std::unique_ptr<ofdm_slot_modulator> ofdm = ofdm_factory->create_ofdm_slot_modulator(ofdm_config);
        TESTASSERT(ofdm != nullptr);

        // Check is a DFT processor is created and not used.
        auto& dft_processor_factory_entry = dft_factory.get_entries();
        TESTASSERT(dft_processor_factory_entry.size() == 1);
        dft_processor_spy& dft = *dft_processor_factory_entry[0].dft;
        TESTASSERT(dft.get_entries().empty());

        // Define empty guard band.
        std::vector<cf_t> zero_guard(dft_size - nsubc);

        // Iterate all slots within a subframe.
        for (unsigned slot_idx = 0, nslot = pow2(numerology); slot_idx != nslot; ++slot_idx) {
          // Select a random port.
          unsigned port_idx = dist_port(rgen);

          // Generate random data in the resource grid.
          resource_grid_reader_spy rg;
          for (unsigned symbol_idx = 0, nsymb = get_nsymb_per_slot(cp); symbol_idx != nsymb; ++symbol_idx) {
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
          dft.reset();

          // Modulate signal.
          std::vector<cf_t> output(ofdm->get_slot_size(slot_idx));
          ofdm->modulate(output, rg, port_idx, slot_idx);

          // Check the number of calls to DFT processor match with the number of symbols.
          unsigned nsymb = get_nsymb_per_slot(cp);
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
            unsigned          cp_len = cp.get_length(nsymb * slot_idx + symbol_idx, numerology, dft_size);
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
