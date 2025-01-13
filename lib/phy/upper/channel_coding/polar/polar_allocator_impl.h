/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
