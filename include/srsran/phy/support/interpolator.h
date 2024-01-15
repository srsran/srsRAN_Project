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
/// \brief Interpolator interface.

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

namespace srsran {

/// \brief Interpolator interface.
///
/// Describes a generic interpolator, agnostic to the interpolation algorithm.
class interpolator
{
public:
  /// Interpolator configuration parameters.
  struct configuration {
    /// Index of the first known value (i.e., number of unknown values at the beginning of the sequence).
    unsigned offset = 0;
    /// Distance between two consecutive known values (a value of one means that there are no gaps).
    unsigned stride = 1;
  };

  /// Defalut destructor.
  virtual ~interpolator() = default;

  /// \brief Interpolates the missing data in a sequence.
  ///
  /// \param[out] output Interpolated data sequence.
  /// \param[in]  input  Known data.
  /// \param[in]  cfg    Configuration: describes the position of the known values in the output data sequence.
  /// \remark The output length should be sufficient to process all input data according to the configured pattern.
  /// \remark The function extrapolates all values at the beginning and at the end of the output sequence (that is,
  /// before the first and after the last known values, respectively).
  virtual void interpolate(span<cf_t> output, span<const cf_t> input, const configuration& cfg) = 0;
};

} // namespace srsran
