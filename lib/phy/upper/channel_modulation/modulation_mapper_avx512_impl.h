/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "modulation_mapper_lut_impl.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/support/math/math_utils.h"
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
