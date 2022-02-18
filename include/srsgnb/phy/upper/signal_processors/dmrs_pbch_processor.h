#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_H

#include "srsgnb/phy/resource_grid.h"
#include <memory>

namespace srsgnb {

class dmrs_pbch_processor
{
public:
  static const unsigned NOF_RE = 144; ///< DMRS PBCH Sequence length in the SSB

  struct args_t {
    unsigned phys_cell_id;         ///< Physical cell identifier
    unsigned ssb_idx;              ///< SS/PBCH block index in the burst
    unsigned L_max;                ///< Maximum number of SS/PBCH transmissions in a burst (5ms)
    unsigned ssb_first_subcarrier; ///< First subcarrier in the resource grid
    unsigned ssb_first_symbol;     ///< First symbol in slot resource grid
    bool     n_hf;                 ///< Half frame flag
    float    amplitude;            ///< PSS linear signal amplitude
  };

  virtual ~dmrs_pbch_processor() = default;

  virtual void map(resource_grid_writer& grid, const args_t& args) = 0;
};

std::unique_ptr<dmrs_pbch_processor> create_dmrs_pbch_processor();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_H
