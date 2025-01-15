/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Polar deallocator implementation - Declaration.
#pragma once

#include "srsran/phy/upper/channel_coding/polar/polar_deallocator.h"

namespace srsran {

/// Polar deallocator implementation.
class polar_deallocator_impl : public polar_deallocator
{
public:
  // See interface for the documentation.
  void deallocate(span<uint8_t> message, span<const uint8_t> output_decoder, const polar_code& code) override;
};

} // namespace srsran
