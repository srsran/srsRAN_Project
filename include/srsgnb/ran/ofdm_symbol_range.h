/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

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
