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

#pragma once

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

namespace srsran {

/// \brief Describes a Discrete Fourier Transform (DFT) processor interface for 16-bit complex integer values.
///
/// The DFT in 16-bit complex integer output is divided by the DFT size independently of the direction.
class dft_processor_ci16
{
public:
  /// DFT algorithm direction.
  enum class direction {
    /// Converts a time-domain signal into a frequency-domain signal.
    direct = 0,
    /// Converts a frequency-domain signal into a time-domain signal.
    inverse
  };

  /// Collects the DFT parameters.
  struct configuration {
    /// DFT size.
    unsigned size;
    /// DFT direction (direct or inverse).
    direction dir;
  };

  /// Converts a DFT direction to string.
  static std::string direction_to_string(direction dir) { return dir == direction::direct ? "direct" : "inverse"; }

  /// Default destructor.
  virtual ~dft_processor_ci16() = default;

  /// Gets the DFT direction.
  virtual direction get_direction() const = 0;

  /// Gets the DFT number of points.
  virtual unsigned get_size() const = 0;

  /// \brief Executes the DFT from the internal input data.
  /// \return A view of the internal output DFT buffer.
  /// \remark An assertion is triggered if the input and output sizes do not match the DFT size.
  virtual void run(span<ci16_t> out, span<const ci16_t> in) = 0;
};

} // namespace srsran
