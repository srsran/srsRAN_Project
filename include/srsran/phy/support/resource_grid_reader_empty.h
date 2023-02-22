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
#include "srsran/phy/support/resource_grid.h"
#include "srsran/srsvec/zero.h"

namespace srsran {

/// Defines an empty resource grid reader that behaves as it is filled with zeros.
class resource_grid_reader_empty : public resource_grid_reader
{
public:
  bool is_empty(unsigned /**/) const override { return true; }
  void get(span<cf_t> symbols, unsigned /**/, span<const resource_grid_coordinate> /**/) const override
  {
    srsvec::zero(symbols);
  }
  span<cf_t> get(span<cf_t> symbols, unsigned /**/, unsigned /**/, unsigned /**/, span<const bool> /**/) const override
  {
    srsvec::zero(symbols);
    return {};
  }

  span<cf_t> get(span<cf_t> symbols,
                 unsigned /**/,
                 unsigned /**/,
                 unsigned /**/,
                 const bounded_bitset<MAX_RB * NRE>& /**/) const override
  {
    srsvec::zero(symbols);
    return {};
  }
  void get(span<cf_t> symbols, unsigned /**/, unsigned /**/, unsigned /**/) const override { srsvec::zero(symbols); }
};

} // namespace srsran