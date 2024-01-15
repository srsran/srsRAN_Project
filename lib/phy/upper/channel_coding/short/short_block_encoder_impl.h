/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/// \file
/// \brief Short-block encoder implementation.

#pragma once

#include "srsran/phy/upper/channel_coding/short/short_block_encoder.h"

namespace srsran {

/// Generic implementation of the short-block encoder.
class short_block_encoder_impl : public short_block_encoder
{
public:
  // See interface for the documentation.
  void encode(span<uint8_t> output, span<const uint8_t> input, modulation_scheme mod) override;
};

} // namespace srsran
