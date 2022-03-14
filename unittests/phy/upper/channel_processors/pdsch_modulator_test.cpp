

#include "pdsch_modulator_test_data.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsgnb {
struct pdsch_modulator_config_t {
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;
};

std::unique_ptr<pdsch_modulator> create_pdsch_modulator(pdsch_modulator_config_t& config);

} // namespace srsgnb

using namespace srsgnb;

static constexpr float assert_max_error = 1e-6;

int main()
{
  // Create PDSCH modulator configuration.
  pdsch_modulator_config_t config = {create_modulation_mapper(), create_pseudo_random()};

  // Create PDSCH modulator.
  std::unique_ptr<pdsch_modulator> pdsch = create_pdsch_modulator(config);

  for (const test_case_t& test_case : pdsch_modulator_test_data) {
    // Create resource grid spy.
    resource_grid_spy grid;

    // Prepare codewords
    static_vector<span<const uint8_t>, pdsch_modulator::MAX_NOF_CODEWORDS> codewords(1);
    codewords[0] = test_case.codeword;

    // Modulate.
    pdsch->modulate(grid, codewords, test_case.config);

    // Make sure the number of modulated RE match.
    srsran_assert(test_case.symbols.size() == grid.get_nof_put_entries(),
                  "Mismatched number of entries %d vs %d",
                  test_case.symbols.size(),
                  grid.get_nof_put_entries());

    // Assert encoded data
    std::vector<resource_grid_spy::entry_t> symbols = grid.get_put_entries();
    for (unsigned i = 0; i != symbols.size(); ++i) {
      float err = std::abs(test_case.symbols[i].value - symbols[i].value);
      srsran_assert(test_case.symbols[i].port == symbols[i].port, "Mismatched port");
      srsran_assert(test_case.symbols[i].l == symbols[i].l, "Mismatched symbol");
      srsran_assert(test_case.symbols[i].k == symbols[i].k, "Mismatched subcarrier");
      srsran_assert(err < assert_max_error, "Mismatched value");
    }
  }

  return 0;
}