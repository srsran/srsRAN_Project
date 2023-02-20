/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../phy_test_utils.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"

namespace srsran {

class pusch_demodulator_spy : public pusch_demodulator
{
public:
  struct entry_t {
    span<log_likelihood_ratio>  data;
    const resource_grid_reader* grid;
    const channel_estimate*     estimates;
    configuration               config;
  };

  demodulation_status demodulate(span<log_likelihood_ratio>  data,
                                 const resource_grid_reader& grid,
                                 const channel_estimate&     estimates,
                                 const configuration&        config) override
  {
    entries.emplace_back();
    entry_t& entry  = entries.back();
    entry.data      = data;
    entry.grid      = &grid;
    entry.estimates = &estimates;
    entry.config    = config;

    return demodulation_status();
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

private:
  std::vector<entry_t> entries;
};

PHY_SPY_FACTORY_TEMPLATE(pusch_demodulator);

} // namespace srsran
