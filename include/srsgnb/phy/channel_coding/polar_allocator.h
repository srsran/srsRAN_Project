#ifndef SRSGNB_PHY_CHANNEL_CODING_POLAR_ALLOCATOR_H
#define SRSGNB_PHY_CHANNEL_CODING_POLAR_ALLOCATOR_H

#include "srsgnb/phy/channel_coding/polar_code.h"

namespace srsgnb {

class polar_allocator
{
public:
  virtual ~polar_allocator() = default;

  virtual void allocate(bit_buffer& message, bit_buffer& input_encoder, const polar_code& code) = 0;
};

std::unique_ptr<polar_allocator> create_polar_allocator();

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_POLAR_ALLOCATOR_H