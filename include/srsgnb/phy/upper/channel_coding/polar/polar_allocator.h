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

#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"

namespace srsran {

class polar_allocator
{
public:
  virtual ~polar_allocator() = default;

  virtual void allocate(span<uint8_t> input_encoder, span<const uint8_t> message, const polar_code& code) = 0;
};

} // namespace srsran
