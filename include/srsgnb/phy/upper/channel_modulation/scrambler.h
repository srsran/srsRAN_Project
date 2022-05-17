/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_MODULATION_SCRAMBLER_H
#define SRSGNB_PHY_UPPER_CHANNEL_MODULATION_SCRAMBLER_H

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

class scrambler
{
public:
  virtual byte_buffer scramble(const byte_buffer& input, const byte_buffer& sequence) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_MODULATION_SCRAMBLER_H
