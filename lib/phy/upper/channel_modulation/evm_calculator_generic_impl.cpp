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

#include "evm_calculator_generic_impl.h"
#include "srsran/srsvec/bit.h"
#include "srsran/srsvec/dot_prod.h"
#include "srsran/srsvec/subtract.h"

using namespace srsran;

float evm_calculator_generic_impl::calculate(span<const log_likelihood_ratio> soft_bits,
                                             span<const cf_t>                 symbols,
                                             modulation_scheme                modulation)
{
  // Get modulation order.
  unsigned bits_per_symbol = get_bits_per_symbol(modulation);

  // Verify that soft bits and symbols dimensions are consistent.
  srsran_assert(soft_bits.size() == symbols.size() * bits_per_symbol,
                "The number of soft bits (i.e., {}) is not consistent with the number of symbols (i.e., {}) and "
                "modulation (i.e., {}).",
                soft_bits.size(),
                symbols.size(),
                to_string(modulation));

  unsigned nof_symbols = symbols.size();
  float    avg_power   = 0.0;

  while (!soft_bits.empty()) {
    unsigned block_nof_symbols =
        std::min(static_cast<unsigned>(symbols.size()), static_cast<unsigned>(MAX_NOF_SYMBOLS));
    unsigned block_nof_bits = block_nof_symbols * bits_per_symbol;

    // Perform hard-decision.
    bit_buffer hard_bits = temp_hard_bits.first(block_nof_bits);
    hard_decision(hard_bits, soft_bits.first(block_nof_bits));

    // Modulate.
    span<cf_t> modulated = temp_modulated.first(block_nof_symbols);
    modulator->modulate(modulated, hard_bits, modulation);

    // Calculate EVM.
    srsvec::subtract(modulated, symbols.first(block_nof_symbols), modulated);

    // Accumulate power.
    avg_power += std::real(srsvec::dot_prod(modulated, modulated));

    // Pop bits and symbols.
    symbols   = symbols.last(symbols.size() - block_nof_symbols);
    soft_bits = soft_bits.last(soft_bits.size() - block_nof_bits);
  }

  return std::sqrt(avg_power / static_cast<float>(nof_symbols));
}
