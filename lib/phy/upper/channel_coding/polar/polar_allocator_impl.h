/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Polar allocator - Declaration of implementation class.

#pragma once

#include "srsran/phy/upper/channel_coding/polar/polar_allocator.h"

namespace srsran {

/// Polar allocator implementation.
class polar_allocator_impl : public polar_allocator
{
public:
  // See interface for the documentation.
  void allocate(span<uint8_t> input_encoder, span<const uint8_t> message, const polar_code& code) override;
};

} // namespace srsran
