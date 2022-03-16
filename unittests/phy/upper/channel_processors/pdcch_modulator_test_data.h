#ifndef SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_MODULATOR_TEST_DATA_H
#define SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_MODULATOR_TEST_DATA_H

#include "../../resource_grid_test_doubles.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_modulator.h"
#include <vector>

namespace srsgnb {

struct test_case_t {
  pdcch_modulator::config_t                               config;
  std::vector<uint8_t>                                    data;
  std::vector<resource_grid_writer_spy::expected_entry_t> symbols;
};

static const std::vector<test_case_t> pdcch_modulator_test_data = {
    // No data to test.
};

} // namespace srsgnb

#endif // SRSGNB_UNITTESTS_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_MODULATOR_TEST_DATA_H
