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

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

namespace srsran {

/// \brief Describes a Discrete Fourier Transform (DFT) processor.
/// \remark The DFT processor allocates the input and output buffers for optimization purposes.
/// \remark The input buffer is available using get_input().
/// \remark The output buffer is available using run().
class dft_processor
{
public:
  /// Indicates the DFT direction.
  enum class direction {
    /// \brief Converts a time domain signal into frequency domain.
    /// \remark For the FFTW based libraries, it is equivalent to \c FFTW_FORWARD sign.
    DIRECT = 0,
    /// \brief Converts a frequency domain signal into frequency (inverse DFT).
    /// \remark For the FFTW based libraries, it is equivalent to \c FFTW_BACKWARD sign.
    INVERSE
  };

  /// Describes the DFT parameters.
  struct configuration {
    /// Indicates the DFT size.
    unsigned size;
    /// Indicates if the DFT is direct or inverse.
    direction dir;
  };

  /// Converts a DFT direction to string.
  static std::string direction_to_string(direction dir) { return dir == direction::DIRECT ? "direct" : "inverse"; }

  /// Default destructor.
  virtual ~dft_processor() = default;

  /// \brief Gets the DFT direction.
  virtual direction get_direction() const = 0;

  /// \brief Gets the DFT number of points.
  virtual unsigned get_size() const = 0;

  /// \brief Gets a view of the internal input DFT buffer.
  virtual span<cf_t> get_input() = 0;

  /// \brief Executes the DFT from the internal input data.
  /// \return A view of the internal output DFT buffer.
  virtual span<const cf_t> run() = 0;
};

} // namespace srsran
