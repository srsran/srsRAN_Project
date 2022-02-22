#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_PSS_PROCESSOR_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_PSS_PROCESSOR_H

#include "srsgnb/phy/resource_grid.h"
#include <memory>

namespace srsgnb {

class pss_processor
{
public:
  struct config_t {
    /// Physical cell identifier
    unsigned phys_cell_id;
    /// First subcarrier in the resource grid
    unsigned ssb_first_subcarrier;
    /// First symbol in slot resource grid
    unsigned ssb_first_symbol;
    /// PSS linear signal amplitude
    float amplitude;
  };

  virtual ~pss_processor() = default;

  virtual void map(resource_grid_writer& grid, const config_t& args) = 0;
};

std::unique_ptr<pss_processor> create_pss_processor();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_PSS_PROCESSOR_H
