
#include "../../resource_grid_test_doubles.h"
#include "ofdm_modulator_test_data.h"
#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/phy/lower/modulation/ofdm_modulator.h"
#include "srsgnb/srsvec/compare.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/support/test_utils.h"

// Necessary prototipes to create the DFT processor instance.
namespace srsgnb {
struct dft_processor_factory_fftw_config {
  bool        avoid_wisdom;
  std::string wisdom_filename;
};

std::unique_ptr<dft_processor_factory>
create_dft_processor_factory_fftw(const dft_processor_factory_fftw_config& factory_config);

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
  // Create a DFT factory.
  dft_processor_factory_fftw_config      factory_config = {};
  std::unique_ptr<dft_processor_factory> dft_factory    = create_dft_processor_factory_fftw(factory_config);

  // Prepare OFDM modulator factory configuration.
  ofdm_modulator_factory_config ofdm_factory_config = {*dft_factory};

  // Create OFDM modulator factory.
  std::unique_ptr<ofdm_modulator_factory> ofdm_factory = create_ofdm_modulator_factory(ofdm_factory_config);

  // Run all defined tests
  for (const test_case_t& test_case : ofdm_modulator_test_data) {
    // Reset factory.
    // dft_factory.reset();

    // Create FFTW configuration;
    dft_processor::configuration config;
    config.size = test_case.test_config.config.dft_size;
    config.dir  = dft_processor::direction::DIRECT;

    // Create DFT processor
    std::unique_ptr<dft_processor> dft = dft_factory->create(config);

    unsigned nsubc = test_case.test_config.config.bw_rb * NRE;

    // Create OFDM modulator.
    std::unique_ptr<ofdm_slot_modulator> ofdm = ofdm_factory->create_ofdm_slot_modulator(test_case.test_config.config);
    TESTASSERT(ofdm != nullptr);

    // Load the input data.
    const std::vector<resource_grid_writer_spy::expected_entry_t> data = test_case.data.read();

    // Load the golden data.
    const std::vector<cf_t> modulated = test_case.modulated.read();

    // Define empty guard band.
    std::vector<cf_t> zero_guard(test_case.test_config.config.dft_size - nsubc);

    // Iterate all slots within a subframe.
    unsigned input_offset = 0;
    for (unsigned slot_idx = 0, nslot = pow2(test_case.test_config.config.numerology); slot_idx != nslot; ++slot_idx) {
      // Map the read data into a reader_spy structure.
      resource_grid_reader_spy rg;
      unsigned                 nsymb = get_nsymb_per_slot(test_case.test_config.config.cp);
      for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
        for (unsigned subc_idx = 0; subc_idx != nsubc; ++subc_idx) {
          resource_grid_reader_spy::expected_entry_t entry = {};
          entry.port                                       = data[input_offset + subc_idx].port;
          entry.symbol                                     = data[input_offset + subc_idx].symbol;
          entry.subcarrier                                 = data[input_offset + subc_idx].subcarrier;
          entry.value                                      = data[input_offset + subc_idx].value;
          rg.write(entry);
        }
        input_offset += nsubc;
      }

      // Reset DFT entries.
      // dft.reset();

      // Modulate signal.
      std::vector<cf_t> output(ofdm->get_slot_size(test_case.test_config.slot_idx));
      ofdm->modulate(output, rg, test_case.test_config.port_idx, test_case.test_config.slot_idx);

      // Iterate all symbols.
      unsigned offset = 0;
      for (unsigned symbol_idx = 0; symbol_idx != nsymb; ++symbol_idx) {
        // Map the read golden data into a symbol-wise vector.
        unsigned cp_len =
            test_case.test_config.config.cp.get_length(nsymb * test_case.test_config.slot_idx + symbol_idx,
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

        // Assert generated symbol matches ideal.
        for (unsigned idx = 0, nsamples = test_case.test_config.config.dft_size + cp_len; idx != nsamples; ++idx) {
          float err = std::abs(expected_output[idx] - output_symbol[idx]);
          TESTASSERT(err < ASSERT_MAX_ERROR);
        }

        // Increment OFDM symbol offset.
        offset += test_case.test_config.config.dft_size + cp_len;
      }
    }
  }

  return 0;
}
