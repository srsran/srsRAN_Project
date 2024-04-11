/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/srsvec/zero.h"

namespace srsran {

/// Defines an empty resource grid reader that behaves as it is filled with zeros.
class resource_grid_reader_empty : public resource_grid_reader
{
public:
  /// Creates an empty resource grid reader with the given dimensions.
  resource_grid_reader_empty(unsigned nof_ports_, unsigned nof_symbols_, unsigned nof_prb_) :
    nof_ports(nof_ports_), nof_symbols(nof_symbols_), nof_prb(nof_prb_)
  {
  }

  // See interface for documentation.
  unsigned get_nof_ports() const override { return nof_ports; }

  // See interface for documentation.
  unsigned get_nof_subc() const override { return nof_prb * NRE; }

  // See interface for documentation.
  unsigned get_nof_symbols() const override { return nof_symbols; }

  // See interface for documentation.
  bool is_empty(unsigned /**/) const override { return true; }

  // See interface for documentation.
  bool is_empty() const override { return true; }

  // See interface for documentation.
  span<cf_t> get(span<cf_t> symbols,
                 unsigned /**/,
                 unsigned /**/,
                 unsigned /**/,
                 const bounded_bitset<MAX_RB * NRE>& /**/) const override
  {
    srsvec::zero(symbols);
    return {};
  }

  // See interface for documentation.
  void get(span<cf_t> symbols, unsigned /**/, unsigned /**/, unsigned /**/, unsigned /**/) const override
  {
    srsvec::zero(symbols);
  }

  // See interface for documentation.
  span<const cf_t> get_view(unsigned /**/, unsigned /**/) const override { return {}; }

private:
  unsigned nof_ports;
  unsigned nof_symbols;
  unsigned nof_prb;
};

} // namespace srsran
