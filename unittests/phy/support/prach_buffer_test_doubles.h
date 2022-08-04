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

class prach_buffer_dummy : public prach_buffer
{
public:
  unsigned         get_sequence_length() const override { return 0; }
  unsigned         get_max_nof_symbols() const override { return 0; }
  span<cf_t>       get_symbol(unsigned symbol_index) override { return {}; }
  span<const cf_t> get_symbol(unsigned symbol_index) const override { return {}; }
};

} // namespace srsgnb
