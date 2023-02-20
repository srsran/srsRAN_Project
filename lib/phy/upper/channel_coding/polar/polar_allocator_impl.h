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

#include "srsgnb/phy/upper/channel_coding/polar/polar_allocator.h"

namespace srsran {

class polar_allocator_impl : public polar_allocator
{
public:
  polar_allocator_impl()  = default;
  ~polar_allocator_impl() = default;
  void allocate(span<uint8_t> input_encoder, span<const uint8_t> message, const polar_code& code) override;
};

} // namespace srsran
