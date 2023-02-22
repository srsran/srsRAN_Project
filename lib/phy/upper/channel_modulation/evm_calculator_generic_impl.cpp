/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "evm_calculator_generic_impl.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/subtract.h"

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
