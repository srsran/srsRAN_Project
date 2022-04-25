
#ifndef SRSGNB_OFDM_SYMBOL_RANGE_H
#define SRSGNB_OFDM_SYMBOL_RANGE_H

#include "srsgnb/adt/interval.h"

namespace srsgnb {

/// Range [start,stop) of OFDM symbols.
struct ofdm_symbol_range : public interval<uint8_t> {
  using interval<uint8_t>::interval;
};

} // namespace srsgnb

#endif // SRSGNB_OFDM_SYMBOL_RANGE_H
