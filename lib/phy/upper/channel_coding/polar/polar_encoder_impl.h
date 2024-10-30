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
/// \brief Polar encoder - Declaration of implementation class.
#pragma once

#include "srsran/phy/upper/channel_coding/polar/polar_encoder.h"

namespace srsran {

/// Polar encoder implementation.
class polar_encoder_impl : public polar_encoder
{
public:
  // See interface for documentation.
  void encode(span<uint8_t> output, span<const uint8_t> input, unsigned code_size_log) override;
};

} // namespace srsran
