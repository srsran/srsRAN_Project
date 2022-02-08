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

#ifndef SRSGNB_CHANNEL_CODING_POLAR_DEALLOCATOR_IMPL_H_
#define SRSGNB_CHANNEL_CODING_POLAR_DEALLOCATOR_IMPL_H_

#include "srsgnb/phy/channel_coding/polar_deallocator.h"

namespace srsgnb {

class polar_deallocator_impl : public polar_deallocator
{
public:
  polar_deallocator_impl()  = default;
  ~polar_deallocator_impl() = default;
  void deallocate(span<const uint8_t> output_encoder, span<uint8_t> message, const polar_code& code) override;
};

} // namespace srsgnb
#endif // SRSGNB_CHANNEL_CODING_POLAR_DEALLOCATOR_IMPL_H_
