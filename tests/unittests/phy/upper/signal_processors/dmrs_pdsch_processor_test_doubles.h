
#pragma once

#include "../../phy_test_utils.h"
#include "srsran/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsran/phy/upper/signal_processors/signal_processor_factories.h"

namespace srsran {

class dmrs_pdsch_processor_spy : public dmrs_pdsch_processor
{
private:
  struct entry_t {
    config_t              config;
    resource_grid_mapper* mapper;
  };
  std::vector<entry_t> entries;

public:
  void map(resource_grid_mapper& mapper, const config_t& config) override
  {
    entry_t entry = {};
    entry.config  = config;
    entry.mapper  = &mapper;
    entries.emplace_back(entry);
  }
  void                        reset() { entries.clear(); }
  unsigned                    get_nof_entries() const { return entries.size(); }
  const std::vector<entry_t>& get_entries() const { return entries; }
};

PHY_SPY_FACTORY_TEMPLATE(dmrs_pdsch_processor);

} // namespace srsran
