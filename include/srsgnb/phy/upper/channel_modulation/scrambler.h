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
