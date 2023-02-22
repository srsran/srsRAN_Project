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

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/phy/support/prach_buffer.h"

using namespace srsran;

namespace srsran {

template <unsigned SEQ_LEN, unsigned MAX_NOF_SYMB>
class prach_buffer_static : public prach_buffer
{
private:
  std::array<cf_t, SEQ_LEN * MAX_NOF_SYMB> symbols;

public:
  // See interface for documentation.
  unsigned get_sequence_length() const override { return SEQ_LEN; }

  // See interface for documentation.
  unsigned get_max_nof_symbols() const override { return MAX_NOF_SYMB; }

  // See interface for documentation.
  span<cf_t> get_symbol(unsigned symbol_index) override
  {
    srsran_assert(symbol_index < MAX_NOF_SYMB,
                  "The symbol index {} exceeds the number of symbols {}.",
                  symbol_index,
                  MAX_NOF_SYMB);
    return span<cf_t>(symbols).subspan(symbol_index * SEQ_LEN, SEQ_LEN);
  }

  // See interface for documentation.
  span<const cf_t> get_symbol(unsigned symbol_index) const override
  {
    srsran_assert(symbol_index < MAX_NOF_SYMB,
                  "The symbol index {} exceeds the number of symbols {}.",
                  symbol_index,
                  MAX_NOF_SYMB);
    return span<const cf_t>(symbols).subspan(symbol_index * SEQ_LEN, SEQ_LEN);
  }
};

} // namespace srsran
