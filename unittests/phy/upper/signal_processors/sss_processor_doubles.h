
#ifndef SRSGNB_SSS_PROCESSOR_DOUBLES_H
#define SRSGNB_SSS_PROCESSOR_DOUBLES_H

#include "srsgnb/phy/upper/signal_processors/sss_processor.h"

namespace srsgnb {

class sss_processor_spy : public sss_processor
{
private:
  std::vector<config_t> entries;

public:
  void map(resource_grid_writer& grid, const config_t& args) override { entries.emplace_back(args); }
  const std::vector<config_t>& get_entries() const { return entries; }
};

} // namespace srsgnb

#endif // SRSGNB_SSS_PROCESSOR_DOUBLES_H
