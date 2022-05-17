/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDCCH_PROCESSOR_TEST_DOUBLES_H
#define UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDCCH_PROCESSOR_TEST_DOUBLES_H

#include "srsgnb/phy/upper/signal_processors/dmrs_pdcch_processor.h"

namespace srsgnb {

class dmrs_pdcch_processor_spy : public dmrs_pdcch_processor
{
private:
  struct entry_t {
    config_t              config;
    resource_grid_writer* grid;
  };
  std::vector<entry_t> entries;

public:
  void map(resource_grid_writer& grid, const config_t& config) override
  {
    entry_t entry = {};
    entry.config  = config;
    entry.grid    = &grid;
    entries.emplace_back(entry);
  }
  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

} // namespace srsgnb

#endif // UNITTESTS_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PDCCH_PROCESSOR_TEST_DOUBLES_H
