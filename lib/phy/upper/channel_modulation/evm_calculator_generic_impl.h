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

#include "srsran/phy/upper/channel_modulation/evm_calculator.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"

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
  std::vector<cf_t> temp_modulated;
};

} // namespace srsran
