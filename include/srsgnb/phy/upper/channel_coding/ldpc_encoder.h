#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_ENCODER
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_ENCODER

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/phy/upper/channel_coding/ldpc.h"

namespace srsgnb {

class ldpc_encoder
{
public:
  virtual byte_buffer encode(const byte_buffer& input, ldpc_base_graph base_graph) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_ENCODER
