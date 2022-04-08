
#include "../../generic_functions/dft_processor_test_doubles.h"
#include "../../resource_grid_test_doubles.h"
#include "ofdm_modulator_test_data.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/support/test_utils.h"

namespace srsgnb {
struct ofdm_modulator_factory_config {
  dft_processor_factory& dft_factory;
};

std::unique_ptr<ofdm_modulator_factory> create_ofdm_modulator_factory(ofdm_modulator_factory_config& config);

} // namespace srsgnb

using namespace srsgnb;

int main()
{
  // Create DFT factory spy.
  dft_processor_factory_spy dft_factory;

  // Prepare OFDM modulator factory configuration.
  ofdm_modulator_factory_config ofdm_factory_config = {dft_factory};

  // Create OFDM modulator factory.
  std::unique_ptr<ofdm_modulator_factory> ofdm_factory = create_ofdm_modulator_factory(ofdm_factory_config);

  // Run all defined tests
  for (const test_case_t& test_case : ofdm_modulator_test_data) {
    // Reset spies.
    dft_factory.reset();

    unsigned nsubc = test_case.test_config.config.bw_rb * NRE;

    // Create OFDM modulator.
    std::unique_ptr<ofdm_slot_modulator> ofdm = ofdm_factory->create_ofdm_slot_modulator(test_case.test_config.config);
    TESTASSERT(ofdm != nullptr);

    // Check if a DFT processor is created and not used.
    auto& dft_processor_factory_entry = dft_factory.get_entries();
    TESTASSERT(dft_processor_factory_entry.size() == 1);
    dft_processor_spy& dft = *dft_processor_factory_entry[0].dft;
    TESTASSERT(dft.get_entries().empty());

    // Load the input data.
    const std::vector<resource_grid_writer_spy::expected_entry_t> data = test_case.data.read();

    // Map the read data into a reader_spy structure.
    resource_grid_reader_spy rg;
    int                      index = 0;
    for (unsigned symbol_idx = 0, nsymb = get_nsymb_per_slot(test_case.test_config.config.cp); symbol_idx != nsymb;
         ++symbol_idx) {
      for (unsigned subc_idx = 0; subc_idx != nsubc; ++subc_idx) {
        resource_grid_reader_spy::expected_entry_t entry = {};
        entry.port                                       = data[index].port;
        entry.symbol                                     = data[index].symbol;
        entry.subcarrier                                 = data[index].subcarrier;
        entry.value                                      = data[index].value;
        rg.write(entry);
        index = index + 1;
      }
    }

    // Modulate signal.
    std::vector<cf_t> output(ofdm->get_slot_size(test_case.test_config.slot_idx));
    ofdm->modulate(output, rg, test_case.test_config.port_idx, test_case.test_config.slot_idx);

    // Load the golden data.
    const std::vector<cf_t> modulated = test_case.modulated.read();

    // Check the number of calls to DFT processor match with the number of symbols.
    unsigned nsymb = get_nsymb_per_slot(test_case.test_config.config.cp);
    TESTASSERT(dft.get_entries().size() == nsymb);

    // Define empty guard band.
    std::vector<cf_t> zero_guard(test_case.test_config.config.dft_size - nsubc);

    unsigned offset      = 0;
    auto     dft_entries = dft.get_entries();
    for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
      // Get expected data from RG.
      std::vector<cf_t> rg_data_symbol(nsubc);
      rg.get(rg_data_symbol, test_case.test_config.port_idx, symbol_idx, 0);
      span<cf_t> rg_data = rg_data_symbol;

      // Get DFT input.
      span<const cf_t> dft_input = dft_entries[symbol_idx].input;

      // Verify DFT input.
      TESTASSERT(srsvec::equal(rg_data.first(nsubc / 2), dft_input.last(nsubc / 2)));
      TESTASSERT(srsvec::equal(rg_data.last(nsubc / 2), dft_input.first(nsubc / 2)));
      TESTASSERT(srsvec::equal(zero_guard, dft_input.subspan(nsubc / 2, zero_guard.size())));

      // Map the read golden data into a symbol-wise vector.
      unsigned cp_len = test_case.test_config.config.cp.get_length(nsymb * test_case.test_config.slot_idx + symbol_idx,
                                                                   test_case.test_config.config.numerology,
                                                                   test_case.test_config.config.dft_size);
      std::vector<cf_t> expected_output_data(test_case.test_config.config.dft_size + cp_len);
      for (unsigned idx = 0, nsamples = test_case.test_config.config.dft_size + cp_len; idx != nsamples; ++idx) {
        expected_output_data[idx] = modulated[offset + idx];
      }
      span<cf_t> expected_output = expected_output_data;

      // Select a view of the OFDM modulator output.
      span<const cf_t> output_symbol(output);
      output_symbol = output_symbol.subspan(offset, test_case.test_config.config.dft_size + cp_len);

      for (unsigned idx = 0, nsamples = test_case.test_config.config.dft_size + cp_len; idx != nsamples; ++idx) {
        float err = std::abs(expected_output[idx] - output_symbol[idx]);
        TESTASSERT(err < 1e-6);
      }

      offset += test_case.test_config.config.dft_size + cp_len;
    }
  }

  return 0;
}
