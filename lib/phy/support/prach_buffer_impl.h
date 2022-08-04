/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/support/prach_buffer.h"
#include "srsgnb/phy/support/support_factories.h"

using namespace srsgnb;

namespace srsgnb {

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
    srsgnb_assert(symbol_index < MAX_NOF_SYMB,
                  "The symbol index {} exceeds the number of symbols {}.",
                  symbol_index,
                  MAX_NOF_SYMB);
    return span<cf_t>(symbols).subspan(symbol_index * SEQ_LEN, SEQ_LEN);
  }

  // See interface for documentation.
  span<const cf_t> get_symbol(unsigned symbol_index) const override
  {
    srsgnb_assert(symbol_index < MAX_NOF_SYMB,
                  "The symbol index {} exceeds the number of symbols {}.",
                  symbol_index,
                  MAX_NOF_SYMB);
    return span<const cf_t>(symbols).subspan(symbol_index * SEQ_LEN, SEQ_LEN);
  }
};

} // namespace srsgnb
