/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Short-block encoder implementation.

#pragma once

#include "srsgnb/phy/upper/channel_coding/short/short_block_encoder.h"

namespace srsgnb {

/// Generic implementation of the short-block encoder.
class short_block_encoder_impl : public short_block_encoder
{
public:
  // See interface for the documentation.
  void encode(span<uint8_t> output, span<const uint8_t> input, modulation_scheme mod) override;
};

} // namespace srsgnb
