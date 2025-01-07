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

/// Modulation mapper based on NEON instruction sets.
class modulation_mapper_neon_impl : public modulation_mapper
{
public:
  // See interface for the documentation.
  void modulate(span<cf_t> symbols, const bit_buffer& input, modulation_scheme scheme) override;

  // See interface for the documentation.
  float modulate(span<ci8_t> symbols, const bit_buffer& input, modulation_scheme scheme) override;

private:
  /// Implements the modulation algorithm for QPSK.
  float modulate_qpsk(span<ci8_t> symbols, const bit_buffer& input);

  /// Implements the modulation algorithm for 16-QAM.
  float modulate_qam16(span<ci8_t> symbols, const bit_buffer& input);

  /// Implements the modulation algorithm for 64-QAM.
  float modulate_qam64(span<ci8_t> symbols, const bit_buffer& input);

  /// Implements the modulation algorithm for 256-QAM.
  float modulate_qam256(span<ci8_t> symbols, const bit_buffer& input);

  /// Modulation mapper based on LUT for modulations that are not implemented with the NEON instruction set.
  modulation_mapper_lut_impl lut_modulator;
};

} // namespace srsran
