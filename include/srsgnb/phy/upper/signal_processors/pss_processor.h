#ifndef SRSGNB_PHY_SIGNAL_PROCESSORS_PSS_PROCESSOR_H
#define SRSGNB_PHY_SIGNAL_PROCESSORS_PSS_PROCESSOR_H

#include "srsgnb/phy/resource_grid.h"
#include <memory>

namespace srsgnb {

class pss_processor
{
public:
  struct args_t {
    unsigned phys_cell_id;         ///< Physical cell identifier
    unsigned ssb_first_subcarrier; ///< First subcarrier in the resource grid
    unsigned ssb_first_symbol;     ///< First symbol in slot resource grid
    float    amplitude;            ///< PSS linear signal amplitude
  };

  virtual ~pss_processor() = default;

  virtual void map(resource_grid_writer& grid, const args_t& args) = 0;
};

std::unique_ptr<pss_processor> create_pss_processor();

} // namespace srsgnb

#endif // SRSGNB_PHY_SIGNAL_PROCESSORS_PSS_PROCESSOR_H
