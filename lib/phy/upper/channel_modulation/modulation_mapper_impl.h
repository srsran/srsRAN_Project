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

#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/support/math_utils.h"
#include <map>

namespace srsran {
/// Modulation mapper based on look-up tables.
class modulation_mapper_impl : public modulation_mapper
{
public:
  // See interface for the documentation.
  void modulate(span<cf_t> symbols, const bit_buffer& input, modulation_scheme scheme) override;

  // See interface for the documentation.
  float modulate(span<ci8_t> symbols, const bit_buffer& input, modulation_scheme scheme) override;
};

} // namespace srsran
