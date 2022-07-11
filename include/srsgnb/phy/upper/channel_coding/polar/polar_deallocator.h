/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"

namespace srsgnb {

class polar_deallocator
{
public:
  virtual ~polar_deallocator() = default;

  virtual void deallocate(span<uint8_t> message, span<const uint8_t> output_decoder, const polar_code& code) = 0;
};

std::unique_ptr<polar_deallocator> create_polar_deallocator();

} // namespace srsgnb
