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

#include "srsgnb/adt/tensor.h"
#include "srsgnb/phy/support/resource_grid.h"

namespace srsgnb {

/// Describes a generic resource grid implementation
class resource_grid_impl : public resource_grid
{
private:
  /// Dimension, i.e. number of coordinates, of each indexing level of the resource grid.
  enum dimensions : unsigned { symbol = 1, port = 2, all = 3 };

  std::vector<bool> empty;
  unsigned          nof_ports;
  unsigned          nof_symb;
  unsigned          nof_subc;

  /// \brief Stores the resource grid data.
  ///
  /// The resource grid buffer is a three-dimensional array with the dimensions representing, in order, subcarriers,
  /// OFDM symbols and antenna ports.
  dynamic_tensor<dimensions::all, cf_t> rg_buffer;

public:
  resource_grid_impl(unsigned nof_ports, unsigned nof_symb, unsigned nof_subc);
  void put(unsigned port_, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override;
  span<const cf_t>
  put(unsigned port_, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) override;
  span<const cf_t> put(unsigned                            port_,
                       unsigned                            l,
                       unsigned                            k_init,
                       const bounded_bitset<NRE * MAX_RB>& mask,
                       span<const cf_t>                    symbols) override;
  void             put(unsigned port_, unsigned l, unsigned k_init, span<const cf_t> symbols) override;

  bool       is_empty(unsigned int port_) const override;
  void       get(span<cf_t> symbols, unsigned port_, span<const resource_grid_coordinate> coordinates) const override;
  span<cf_t> get(span<cf_t> symbols, unsigned port_, unsigned l, unsigned k_init, span<const bool> mask) const override;
  span<cf_t> get(span<cf_t>                          symbols,
                 unsigned                            port_,
                 unsigned                            l,
                 unsigned                            k_init,
                 const bounded_bitset<MAX_RB * NRE>& mask) const override;
  void       get(span<cf_t> symbols, unsigned port_, unsigned l, unsigned k_init) const override;

  void set_all_zero() override;
};

} // namespace srsgnb
