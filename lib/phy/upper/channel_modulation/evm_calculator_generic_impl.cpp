/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "evm_calculator_generic_impl.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/srsvec/dot_prod.h"
#include "srsgnb/srsvec/subtract.h"

using namespace srsran;

float evm_calculator_generic_impl::calculate(span<const log_likelihood_ratio> soft_bits,
                                             span<const cf_t>                 symbols,
                                             modulation_scheme                modulation)
{
  // Perform hard-decision.
  bit_buffer hard_bits = temp_hard_bits.first(soft_bits.size());
  hard_decision(hard_bits, soft_bits);

  // Modulate.
  span<cf_t> modulated = temp_modulated.first(symbols.size());
  modulator->modulate(modulated, hard_bits, modulation);

  // Calculate EVM.
  srsvec::subtract(modulated, symbols, modulated);
  return std::sqrt(srsvec::average_power(modulated));
}
