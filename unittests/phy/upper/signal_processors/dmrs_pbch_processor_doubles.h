
#ifndef SRSGNB_DMRS_PBCH_PROCESSOR_DOUBLES_H
#define SRSGNB_DMRS_PBCH_PROCESSOR_DOUBLES_H

#include "srsgnb/phy/upper/signal_processors/dmrs_pbch_processor.h"

namespace srsgnb {

class dmrs_pbch_processor_spy : public dmrs_pbch_processor
{
private:
  std::vector<config_t> entries;

public:
  void map(resource_grid_writer& grid, const config_t& args) override { entries.emplace_back(args); }
  const std::vector<config_t>& get_entries() const { return entries; }
};

} // namespace srsgnb

#endif // SRSGNB_DMRS_PBCH_PROCESSOR_DOUBLES_H
