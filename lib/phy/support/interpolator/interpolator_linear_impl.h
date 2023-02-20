/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Linear interpolator declaration.

#pragma once

#include "srsgnb/phy/support/interpolator.h"

namespace srsgnb {

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

} // namespace srsgnb
