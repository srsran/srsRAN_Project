
#ifndef SRSGNB_PSS_PROCESSOR_DOUBLES_H
#define SRSGNB_PSS_PROCESSOR_DOUBLES_H

#include "srsgnb/phy/upper/signal_processors/pss_processor.h"

namespace srsgnb {

class pss_processor_spy : public pss_processor
{
private:
  std::vector<config_t> entries;

public:
  void map(resource_grid_writer& grid, const config_t& args) override { entries.emplace_back(args); }
  const std::vector<config_t>& get_entries() const { return entries; }
};

} // namespace srsgnb

#endif // SRSGNB_PSS_PROCESSOR_DOUBLES_H
