
#include "../../generic_functions/dft_processor_test_doubles.h"
#include "../../resource_grid_test_doubles.h"
#include "srsgnb/phy/lower/modulation/ofdm_demodulator.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/support/test_utils.h"
#include <random>

namespace srsgnb {
struct ofdm_demodulator_factory_config {
  dft_processor_factory& dft_factory;
};

std::unique_ptr<ofdm_demodulator_factory> create_ofdm_demodulator_factory(ofdm_demodulator_factory_config& config);

} // namespace srsgnb

/// Defines the maximum allowed error at the OFDM demodulator output.
static constexpr float ASSERT_MAX_ERROR = 1e-6;

using namespace srsgnb;

int main()
{
  std::mt19937                            rgen(0);
  std::uniform_real_distribution<float>   dist_rg(-1, 1);
  std::uniform_int_distribution<unsigned> dist_port(0, MAX_PORTS - 1);

  // Create DFT factory spy.
  dft_processor_factory_spy dft_factory;

  // Prepare OFDM demodulator factory configuration.
  ofdm_demodulator_factory_config ofdm_factory_config = {dft_factory};

  // Create OFDM demodulator factory.
  std::unique_ptr<ofdm_demodulator_factory> ofdm_factory = create_ofdm_demodulator_factory(ofdm_factory_config);

  // Iterate all possible numerologies.
  for (unsigned numerology : {0, 1, 2, 3, 4}) {
    // Iterate all possible FFT sizes.
    for (unsigned dft_size : {256, 384, 512, 768, 1024, 1536, 2048, 3072, 4096, 6144, 8192, 12288}) {
      // Skip combinations of SCS>15kHz with large DFT sizes.
      if (numerology > 0 && dft_size > 4096) {
        continue;
      }

      // Iterate all possible cyclic prefix.
      for (cyclic_prefix cp : {cyclic_prefix::NORMAL, cyclic_prefix::EXTENDED}) {
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

        // Create OFDM demodulator configuration. Use minimum number of RB.
        ofdm_demodulator_configuration ofdm_config = {};
        ofdm_config.numerology                     = numerology;
        ofdm_config.bw_rb                          = 11;
        ofdm_config.dft_size                       = dft_size;
        ofdm_config.cp                             = cp;
        ofdm_config.scale                          = dist_rg(rgen);

        unsigned nsubc = ofdm_config.bw_rb * NRE;

        // Create OFDM demodulator.
        std::unique_ptr<ofdm_slot_demodulator> ofdm = ofdm_factory->create_ofdm_slot_demodulator(ofdm_config);
        TESTASSERT(ofdm != nullptr);

        // Check is a DFT processor is created and not used.
        auto& dft_processor_factory_entry = dft_factory.get_entries();
        TESTASSERT(dft_processor_factory_entry.size() == 1);
        dft_processor_spy& dft = *dft_processor_factory_entry[0].dft;
        TESTASSERT(dft.get_entries().empty());

        // Iterate all slots within a subframe.
        for (unsigned slot_idx = 0, nslot = pow2(numerology); slot_idx != nslot; ++slot_idx) {
          // Select a random port.
          unsigned port_idx = dist_port(rgen);

          // Generate random time domain data.
          unsigned          nsymb = get_nsymb_per_slot(cp);
          std::vector<cf_t> time_data;
          // Iterate all symbols in the slot.
          for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
            // Get the size of the current time-domain symbol.
            unsigned nsamples = cp.get_length(nsymb * slot_idx + symbol_idx, numerology, dft_size) + dft_size;
            for (unsigned sample_idx = 0; sample_idx != nsamples; ++sample_idx) {
              cf_t random_value = {dist_rg(rgen), dist_rg(rgen)};
              time_data.push_back(random_value);
            }
          }

          // Reset DFT spy entries.
          dft.reset();

          // Demodulate signal.
          resource_grid_writer_spy rg;
          ofdm->demodulate(rg, time_data, port_idx, slot_idx);

          // Check the number of calls to DFT processor match with the number of symbols.
          TESTASSERT(dft.get_entries().size() == nsymb);

          // Iterate all symbols.
          std::vector<resource_grid_writer_spy::expected_entry_t> expected_rg;
          unsigned                                                offset      = 0;
          auto                                                    dft_entries = dft.get_entries();
          for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
            // Get the size of the current time-domain symbol.
            unsigned symb_size = cp.get_length(nsymb * slot_idx + symbol_idx, numerology, dft_size) + dft_size;

            // Get input time data.
            span<cf_t> time_data_symbol(&time_data[offset], symb_size);

            // Get DFT input.
            span<const cf_t> dft_input = dft_entries[symbol_idx].input;

            // Verify DFT input.
            TESTASSERT(srsvec::equal(time_data_symbol.last(dft_size), dft_input.first(dft_size)));

            // Generate ideal frequency domain outputs.
            for (unsigned subc_idx = 0; subc_idx != nsubc; ++subc_idx) {
              resource_grid_writer_spy::expected_entry_t entry = {};
              entry.port                                       = port_idx;
              entry.symbol                                     = symbol_idx;
              entry.subcarrier                                 = subc_idx;
              if (subc_idx < nsubc / 2) {
                entry.value = dft_entries[symbol_idx].output[dft_size - (nsubc / 2) + subc_idx] * ofdm_config.scale;
              } else {
                entry.value = dft_entries[symbol_idx].output[subc_idx - (nsubc / 2)] * ofdm_config.scale;
              }
              expected_rg.push_back(entry);
            }

            // Increment OFDM symbol offset.
            offset += symb_size;
          }

          // Assert resource grid entries.
          rg.assert_entries(expected_rg, ASSERT_MAX_ERROR);
        }
      }
    }
  }

  return 0;
}
