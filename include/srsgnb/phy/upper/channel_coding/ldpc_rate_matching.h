#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_MATCHING
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_MATCHING

#include "srsgnb/adt/byte_buffer.h"

namespace srsgnb {

class ldpc_rate_matching
{
public:
  virtual byte_buffer rate_match(const byte_buffer& input) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_MATCHING
