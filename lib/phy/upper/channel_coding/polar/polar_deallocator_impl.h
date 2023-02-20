/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_coding/polar/polar_deallocator.h"

namespace srsgnb {

class polar_deallocator_impl : public polar_deallocator
{
public:
  polar_deallocator_impl()  = default;
  ~polar_deallocator_impl() = default;
  void deallocate(span<uint8_t> message, span<const uint8_t> output_decoder, const polar_code& code) override;
};

} // namespace srsgnb
