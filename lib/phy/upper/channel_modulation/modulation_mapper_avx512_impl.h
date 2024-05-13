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

#include "modulation_mapper_lut_impl.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/support/math_utils.h"
#include <map>

namespace srsran {

/// \brief Modulation mapper based on AVX512 instruction sets.
///
/// It requires the following AVX512 instruction sets: AVX512F, AVX512BW and AVX512VBMI.
class modulation_mapper_avx512_impl : public modulation_mapper
{
public:
  // See interface for the documentation.
  void modulate(span<cf_t> symbols, const bit_buffer& input, modulation_scheme scheme) override;

  // See interface for the documentation.
  float modulate(span<ci8_t> symbols, const bit_buffer& input, modulation_scheme scheme) override;

private:
  /// Implements the modulation algorithm for 64-QAM.
  float modulate_qam64(span<ci8_t> symbols, const bit_buffer& input);

  /// Implements the modulation algorithm for 256-QAM.
  float modulate_qam256(span<ci8_t> symbols, const bit_buffer& input);

  /// Modulation mapper based on LUT for modulations that are not implemented with the AVX512 instruction set.
  modulation_mapper_lut_impl lut_modulator;
};

} // namespace srsran
