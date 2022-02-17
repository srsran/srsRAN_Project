#ifndef SRSGNB_PHY_CHANNEL_CODING_POLAR_DEALLOCATOR_H
#define SRSGNB_PHY_CHANNEL_CODING_POLAR_DEALLOCATOR_H

#include "srsgnb/phy/upper/channel_coding/polar_code.h"

namespace srsgnb {

class polar_deallocator
{
public:
  virtual ~polar_deallocator() = default;

  virtual void deallocate(span<const uint8_t> output_decoder, span<uint8_t> message, const polar_code& code) = 0;
};

std::unique_ptr<polar_deallocator> create_polar_deallocator();

} // namespace srsgnb

#endif // SRSGNB_PHY_CHANNEL_CODING_POLAR_DEALLOCATOR_H