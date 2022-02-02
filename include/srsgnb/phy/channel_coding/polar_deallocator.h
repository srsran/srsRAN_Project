#ifndef SRSGNB_PHY_CHANNEL_CODING_POLAR_DEALLOCATOR_H
#define SRSGNB_PHY_CHANNEL_CODING_POLAR_DEALLOCATOR_H

#include "srsgnb/phy/channel_coding/polar_code.h"

namespace srsgnb {

class polar_deallocator
{
public:
  virtual void deallocate(bit_buffer& output_decoder, bit_buffer& message, const polar_code& code) = 0;
};

std::unique_ptr<polar_deallocator> create_polar_deallocator();

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_POLAR_DEALLOCATOR_H