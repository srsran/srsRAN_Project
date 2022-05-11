/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SSS_PROCESSOR_DOUBLES_H
#define SRSGNB_SSS_PROCESSOR_DOUBLES_H

#include "srsgnb/phy/upper/signal_processors/sss_processor.h"

namespace srsgnb {

class sss_processor_spy : public sss_processor
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

#endif // SRSGNB_SSS_PROCESSOR_DOUBLES_H
