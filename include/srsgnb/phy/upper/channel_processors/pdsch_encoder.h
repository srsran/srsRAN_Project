#ifndef  SRSGNB_PHY_UPPER_PDSCH_ENCODER_H
#define  SRSGNB_PHY_UPPER_PDSCH_ENCODER_H

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

/// :TODO: decide if this interface should accept two TB's as input or not.
class pdsch_encoder
{
public:
  // Return value scheme TBD! (for all computational blocks)
  virtual std::vector<byte_buffer> encode(const byte_buffer& input) = 0;
};

} // namespace srsgnb

#endif //  SRSGNB_PHY_UPPER_PDSCH_ENCODER_H
