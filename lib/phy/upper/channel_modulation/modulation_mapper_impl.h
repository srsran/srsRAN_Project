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

#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsgnb/support/math_utils.h"
#include <map>

namespace srsgnb {
/// Modulation mapper based on look-up tables.
class modulation_mapper_impl : public modulation_mapper
{
public:
  // See interface for the documentation.
  void modulate(span<cf_t> symbols, const bit_buffer& input, modulation_scheme scheme) override;
};

} // namespace srsgnb
