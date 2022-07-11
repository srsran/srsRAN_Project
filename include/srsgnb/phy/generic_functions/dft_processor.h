/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"

namespace srsgnb {

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

} // namespace srsgnb
