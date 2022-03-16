

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
    resource_grid_writer_spy grid;

    // Modulate.
    pdcch->modulate(grid, test_case.data, test_case.config);

    // Assert resource grid entries.
    grid.assert_entries(test_case.symbols);
  }

  return 0;
}