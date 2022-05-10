
#ifndef SRSGNB_SYMBOL_PRB_RES_GRID_H
#define SRSGNB_SYMBOL_PRB_RES_GRID_H

#include "srsgnb/ran/frame_types.h"
#include "srsgnb/ran/prb_grant.h"

namespace srsgnb {

/// Represents a bit matrix of dimensions #NofOfdmSymbols x #PRBs.
class symbol_x_prb_res_grid
{
public:
  explicit symbol_x_prb_res_grid(unsigned nof_prbs_ = 0, bool cp_extended = false) :
    nof_prbs(nof_prbs_),
    resources(nof_prbs * (cp_extended ? NOF_OFDM_SYM_PER_SLOT_EXTENDED_CP : NOF_OFDM_SYM_PER_SLOT_NORMAL_CP))
  {}

  void clear() { resources.reset(); }

  bool collides(prb_interval prbs, ofdm_symbol_range symbols) const
  {
    for (unsigned i = symbols.start(); i < symbols.stop(); ++i) {
      if (resources.any(prbs.start() + i * nof_prbs, prbs.stop() + i * nof_prbs)) {
        return true;
      }
    }
    return false;
  }

  void fill(prb_interval prbs, ofdm_symbol_range symbols)
  {
    for (unsigned i = symbols.start(); i < symbols.stop(); ++i) {
      resources.fill(prbs.start() + i * nof_prbs, prbs.stop() + i * nof_prbs);
    }
  }

private:
  using prb_symbol_bitmap = bounded_bitset<MAX_NOF_PRBS * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, true>;

  unsigned          nof_prbs;
  prb_symbol_bitmap resources;
};

} // namespace srsgnb

#endif // SRSGNB_SYMBOL_PRB_RES_GRID_H
