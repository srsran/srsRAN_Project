

#include "pdcch_modulator_test_data.h"
#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"

namespace srsgnb {
struct pdcch_modulator_config_t {
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;
};

std::unique_ptr<pdcch_modulator> create_pdcch_modulator(pdcch_modulator_config_t& config);

} // namespace srsgnb

using namespace srsgnb;

static constexpr float assert_max_error = 1e-6;

int main()
{
  // Create PDCCH modulator configuration.
  pdcch_modulator_config_t config = {create_modulation_mapper(), create_pseudo_random()};

  // Create PDCCH modulator.
  std::unique_ptr<pdcch_modulator> pdcch = create_pdcch_modulator(config);

  for (const test_case_t& test_case : pdcch_modulator_test_data) {
    // Create resource grid spy.
    resource_grid_spy grid;

    // Modulate.
    pdcch->modulate(grid, test_case.data, test_case.config);

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