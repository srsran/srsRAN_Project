/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../phy_test_utils.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"

namespace srsgnb {

class pusch_demodulator_spy : public pusch_demodulator
{
public:
  struct entry_t {
    span<log_likelihood_ratio>        data;
    std::vector<log_likelihood_ratio> harq_ack;
    std::vector<log_likelihood_ratio> csi_part1;
    std::vector<log_likelihood_ratio> csi_part2;
    const resource_grid_reader*       grid;
    const channel_estimate*           estimates;
    configuration                     config;
  };

  void demodulate(span<log_likelihood_ratio>  data,
                  span<log_likelihood_ratio>  harq_ack,
                  span<log_likelihood_ratio>  csi_part1,
                  span<log_likelihood_ratio>  csi_part2,
                  const resource_grid_reader& grid,
                  const channel_estimate&     estimates,
                  const configuration&        config) override
  {
    entries.emplace_back();
    entry_t& entry = entries.back();
    entry.data     = data;
    entry.harq_ack.resize(harq_ack.size());
    entry.csi_part1.resize(csi_part1.size());
    entry.csi_part2.resize(csi_part2.size());
    entry.grid      = &grid;
    entry.estimates = &estimates;
    entry.config    = config;
  }

  const std::vector<entry_t>& get_entries() const { return entries; }

private:
  std::vector<entry_t> entries;
};

PHY_SPY_FACTORY_TEMPLATE(pusch_demodulator);

} // namespace srsgnb