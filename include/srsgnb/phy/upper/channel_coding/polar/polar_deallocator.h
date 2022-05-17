/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_DEALLOCATOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_DEALLOCATOR_H

#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"

namespace srsgnb {

class polar_deallocator
{
public:
  virtual ~polar_deallocator() = default;

  virtual void deallocate(span<const uint8_t> output_decoder, span<uint8_t> message, const polar_code& code) = 0;
};

std::unique_ptr<polar_deallocator> create_polar_deallocator();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_DEALLOCATOR_H