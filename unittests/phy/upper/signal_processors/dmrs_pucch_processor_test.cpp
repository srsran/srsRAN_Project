
#include "dmrs_pucch_processor_test_data.h"
#include "../../resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsgnb/phy/upper/signal_processors/pucch_orthogonal_sequence.h"
#include "srsgnb/phy/upper/sequence_generators/low_papr_sequence_collection.h"

using namespace srsgnb;

// forward-declare
namespace srsgnb {
std::unique_ptr<low_papr_sequence_generator> create_low_papr_sequence_generator();
}

int main()
{
  // Fixed BW
  const unsigned nof_prb = 52;

  // Create low PAPR sequence generator.
  std::unique_ptr<low_papr_sequence_generator> generator = create_low_papr_sequence_generator();

  // Low PAPR sequence parameters, assumes M_zc = 12
  unsigned m = 1;
  unsigned delta = 0;
  // Generate alphas for 1PRB (see TS38.211 clause 6.3.2.2.2).
  std::vector<float> alphas_1prb;
  for (unsigned i = 0; i != NRE; ++i) {
    alphas_1prb.emplace_back(2U * M_PI * static_cast<float>(i) / static_cast<float>(NRE));
  }

  // Pre-generate a low PAPR sequence collection.
  low_papr_sequence_collection collection(*generator, m, delta, alphas_1prb);

  // Pre-generate orthogonal cover sequences.
  pucch_orthogonal_sequence orthogonal_sequences;

  // Iterate over test cases.
  for (const test_case_t& test_case : dmrs_pucch_processor_test_data) {
    // Create DMRS-PUCCH processor.
    std::unique_ptr<dmrs_pucch_processor> dmrs_pucch = create_dmrs_pucch_processor(test_case.config.format, &collection, &orthogonal_sequences);

    // Load Matlab generated symbols.
    const std::vector<resource_grid_reader_spy::expected_entry_t> testvector_symbols = test_case.symbols.read();

    // Create resource grid
    resource_grid_reader_spy grid;
    grid.write(testvector_symbols);

    // Object to store channel estimation results
    channel_estimate estimate(nof_prb);

    // Estimate channel
    dmrs_pucch->estimate(estimate, grid, test_case.config);

    srsran_assert(fabsf(estimate.noise_estimate) < 1e-6, "Expected ideal channel!");
  }

  return 0;
}
