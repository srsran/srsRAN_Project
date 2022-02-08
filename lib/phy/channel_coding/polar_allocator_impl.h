/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CHANNEL_CODING_POLAR_ALLOCATOR_IMPL_H_
#define SRSGNB_CHANNEL_CODING_POLAR_ALLOCATOR_IMPL_H_

#include "srsgnb/phy/channel_coding/polar_allocator.h"

namespace srsgnb {

class polar_allocator_impl : public polar_allocator
{
public:
  polar_allocator_impl()  = default;
  ~polar_allocator_impl() = default;
  void allocate(span<const uint8_t> message, span<uint8_t> input_encoder, const polar_code& code) override;
};

} // namespace srsgnb
#endif // SRSGNB_CHANNEL_CODING_POLAR_ALLOCATOR_IMPL_H_
