/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/srsvec/zero.h"

namespace srsgnb {

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

} // namespace srsgnb