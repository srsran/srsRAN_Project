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
/// \brief Linear interpolator declaration.

#pragma once

#include "srsran/phy/support/interpolator.h"

namespace srsran {

/// \brief Linear interpolator.
///
/// Missing data between two known values are interpolated linearly. Missing data at the beginning (end, respectively)
/// of the sequence are extrapolated by repeating the first (last, respectively) known value.
class interpolator_linear_impl : public interpolator
{
public:
  // See interface for the documentation.
  void interpolate(span<cf_t> output, span<const cf_t> input, const configuration& cfg) override;
};

} // namespace srsran
