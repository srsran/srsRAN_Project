#ifndef SRSGNB_PHY_CHANNEL_CODING_LDPC_CODEBLOCK_SEGMENTATION
#define SRSGNB_PHY_CHANNEL_CODING_LDPC_CODEBLOCK_SEGMENTATION

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/phy/upper/channel_coding/ldpc.h"

namespace srsgnb {

class ldpc_codeblock_segmentation
{
public:
  virtual std::vector<byte_buffer> segment(const byte_buffer& input, ldpc_base_graph bg) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_LDPC_CODEBLOCK_SEGMENTATION
