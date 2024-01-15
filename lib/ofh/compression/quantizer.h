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

#include "srsran/srsvec/conversion.h"
#include <cmath>
#include <cstdint>

namespace srsran {
namespace ofh {

/// Describes quantization of floating point number in the range [-1; +1] to a fixed point number of a given bit width.
class quantizer
{
public:
  /// Constructor receives width in bits of a resulting fixed point number.
  explicit quantizer(unsigned bit_width_) :
    bit_width(bit_width_), nbits_shift_sign(16U - bit_width), gain((1 << (bit_width - 1)) - 1.0f)
  {
  }

  /// \brief Quantizes a floating point value into 16bit integer.
  ///
  /// \param[in] value A floating point value.
  /// \return Resultant quantized value.
  int16_t to_fixed_point(float value) const
  {
    float clipped = value;
    if (std::abs(value) > 1.0) {
      clipped = std::copysign(1.0, value);
    }
    return static_cast<int16_t>(std::round(clipped * gain));
  }

  /// \brief Quantizes a sequence of floating point values into 16bit integers.
  ///
  /// \param[out] z       Resultant quantized values.
  /// \param[in] x        Sequence of floating point values.
  /// \param[in] in_scale Scaling factor applied to the input prior conversion.
  ///
  /// \remark \c z and \c x must have the same size.
  void to_fixed_point(span<int16_t> z, span<const float> x, float in_scale) const
  {
    float scale = gain * in_scale;
    srsvec::convert_round(x, scale, z);
  }

  /// \brief Converts fixed point value to floating point.
  ///
  /// \param[in] p 16bit integer value.
  /// \return Resultant floating point value.
  float to_float(int p) const { return (p / gain); }

  /// \brief Converts a sequence of fixed point values to complex floating point values.
  ///
  /// \param[out] z       Resultant complex floating point values.
  /// \param[in] x        Sequence of 16bit integer values.
  /// \param[in] in_scale Scaling factor (expected to be bigger or equal to 1) applied to the input prior conversion.
  ///
  /// \remark The size of \c x must be twice the size of \c z as \x is comprised by the quantized pairs of real and
  /// imaginary parts of complex values.
  void to_float(span<cf_t> z, span<const int16_t> x, int16_t in_scale) const
  {
    float scale = gain / in_scale;
    srsvec::convert(x, scale, z);
  }

  /// Sign extends the input value.
  int16_t sign_extend(int16_t value) const
  {
    int16_t shifted = value << nbits_shift_sign;
    return shifted >> nbits_shift_sign;
  }

private:
  /// Width of fixed point number in bits.
  const unsigned bit_width;
  /// Shift for sign extention.
  const unsigned nbits_shift_sign;
  /// Quantization gain.
  const float gain;
};

} // namespace ofh
} // namespace srsran
