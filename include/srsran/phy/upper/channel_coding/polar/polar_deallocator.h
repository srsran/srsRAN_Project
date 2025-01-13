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
/// \brief Polar deallocator - Interface.

#pragma once

#include "srsran/phy/upper/channel_coding/polar/polar_code.h"

namespace srsran {

/// Polar deallocator interface.
class polar_deallocator
{
public:
  /// Default destructor.
  virtual ~polar_deallocator() = default;

  /// \brief Extracts the information bits from the polar decoder output.
  ///
  /// \param[out] message         Information bits.
  /// \param[in]  output_decoder  Output bits of the polar decoder.
  /// \param[in]  code            Polar code description.
  virtual void deallocate(span<uint8_t> message, span<const uint8_t> output_decoder, const polar_code& code) = 0;
};

} // namespace srsran
