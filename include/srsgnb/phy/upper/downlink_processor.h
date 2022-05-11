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

#ifndef SRSGNB_PHY_UPPER_DL_PROCESSOR_H
#define SRSGNB_PHY_UPPER_DL_PROCESSOR_H

#include "srsgnb/adt/byte_buffer.h"
#include <complex>

namespace srsgnb {

// IMPORTANT: this struct is defined at this level, not in the MAC.
struct dl_sched {
  //: TODO:
  byte_buffer pdu;
};

/// Low level UC.
class downlink_processor
{
public:
  // Receives the dl sched struct, dispatches it to each channel encoder+modulation_mapper, then sends it to the output gateway.
  virtual bool process(const dl_sched& sched) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_DL_PROCESSOR_H
