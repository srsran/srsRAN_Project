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

#include "srsgnb/phy/support/prach_buffer.h"

namespace srsgnb {

class prach_buffer_spy : public prach_buffer
{
private:
  unsigned         count_get_sequence_length = 0;
  unsigned         count_get_max_nof_symbols = 0;
  unsigned         count_get_symbol          = 0;
  mutable unsigned count_get_symbol_const    = 0;

public:
  unsigned   get_sequence_length() const override { return 0; }
  unsigned   get_max_nof_symbols() const override { return 0; }
  span<cf_t> get_symbol(unsigned symbol_index) override
  {
    ++count_get_symbol;
    return span<cf_t>();
  }
  span<const cf_t> get_symbol(unsigned symbol_index) const override
  {
    ++count_get_symbol_const;
    return span<const cf_t>();
  }

  unsigned get_total_count() const
  {
    return count_get_sequence_length + count_get_max_nof_symbols + count_get_symbol + count_get_symbol_const;
  }

  void clear()
  {
    count_get_sequence_length = 0;
    count_get_max_nof_symbols = 0;
    count_get_symbol          = 0;
    count_get_symbol_const    = 0;
  }
};

} // namespace srsgnb
