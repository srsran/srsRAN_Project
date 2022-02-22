#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_H

#include "srsgnb/phy/resource_grid.h"
#include <memory>

namespace srsgnb {

class dmrs_pbch_processor
{
public:
  static const unsigned NOF_RE = 144; ///< DMRS PBCH Sequence length in the SSB

  struct config_t {
    /// Physical cell identifier
    unsigned phys_cell_id;
    /// SS/PBCH block index in the burst
    unsigned ssb_idx;
    /// Maximum number of SS/PBCH transmissions in a burst (5ms)
    unsigned L_max;
    /// First subcarrier in the resource grid
    unsigned ssb_first_subcarrier;
    /// First symbol in slot resource grid
    unsigned ssb_first_symbol;
    /// Half frame flag
    bool n_hf;
    /// Linear signal amplitude
    float amplitude;
  };

  virtual ~dmrs_pbch_processor() = default;

  virtual void map(resource_grid_writer& grid, const config_t& args) = 0;
};

std::unique_ptr<dmrs_pbch_processor> create_dmrs_pbch_processor();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_H
