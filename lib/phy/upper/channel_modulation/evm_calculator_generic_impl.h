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

#include "srsran/phy/upper/channel_modulation/evm_calculator.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/srsvec/aligned_vec.h"

namespace srsran {

/// Generic software implementation of the EVM calculator interface.
class evm_calculator_generic_impl : public evm_calculator
{
public:
  /// Default constructor.
  evm_calculator_generic_impl(std::unique_ptr<modulation_mapper> modulator_) :
    modulator(std::move(modulator_)), temp_hard_bits(MAX_NOF_BITS), temp_modulated(MAX_NOF_SYMBOLS)
  {
    srsran_assert(modulator, "Invalid modulator.");
  }

  // See interface for documentation.
  float calculate(span<const log_likelihood_ratio> soft_bits,
                  span<const cf_t>                 symbols,
                  modulation_scheme                modulation) override;

private:
  /// Maximum processing block size in number of symbols.
  static constexpr unsigned MAX_NOF_SYMBOLS = 4096;
  /// Maximum processing block size in number of bits.
  static constexpr unsigned MAX_NOF_BITS = MAX_NOF_SYMBOLS * 8;
  /// Internal modulator.
  std::unique_ptr<modulation_mapper> modulator;
  /// Data after hard-decision.
  dynamic_bit_buffer temp_hard_bits;
  /// Modulated data.
  srsvec::aligned_vec<cf_t> temp_modulated;
};

} // namespace srsran
