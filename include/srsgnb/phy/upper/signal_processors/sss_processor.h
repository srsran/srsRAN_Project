#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SSS_PROCESSOR_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SSS_PROCESSOR_H

#include "srsgnb/phy/resource_grid.h"
#include <memory>

namespace srsgnb {

class sss_processor
{
public:
  struct args_t {
    unsigned phys_cell_id;         ///< Physical cell identifier
    unsigned ssb_first_subcarrier; ///< First subcarrier in the resource grid
    unsigned ssb_first_symbol;     ///< First symbol in slot resource grid
    float    amplitude;            ///< SSS linear signal amplitude
  };

  virtual ~sss_processor() = default;

  virtual void map(resource_grid& grid, const args_t& args) = 0;
};

std::unique_ptr<sss_processor> create_sss_processor();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SSS_PROCESSOR_H
