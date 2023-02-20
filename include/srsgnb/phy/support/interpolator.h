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
/// \brief Interpolator interface.

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"

namespace srsgnb {

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

} // namespace srsgnb
