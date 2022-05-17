/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_ALLOCATOR_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_ALLOCATOR_IMPL_H

#include "srsgnb/phy/upper/channel_coding/polar/polar_allocator.h"

namespace srsgnb {

class polar_allocator_impl : public polar_allocator
{
public:
  polar_allocator_impl()  = default;
  ~polar_allocator_impl() = default;
  void allocate(span<const uint8_t> message, span<uint8_t> input_encoder, const polar_code& code) override;
};

} // namespace srsgnb
#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_ALLOCATOR_IMPL_H
