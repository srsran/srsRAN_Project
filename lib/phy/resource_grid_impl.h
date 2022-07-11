/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RESOURCE_GRID_IMPL_H
#define SRSGNB_RESOURCE_GRID_IMPL_H

#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/srsvec/aligned_vec.h"

namespace srsgnb {

/// Describes a generic resource grid implementation
class resource_grid_impl : public resource_grid
{
private:
  std::vector<bool> empty;
  unsigned          nof_ports;
  unsigned          nof_symb;
  unsigned          nof_subc;

  /// \brief Stores the resource grid buffers
  ///
  /// Each \c srsvec::aligned_vec<cf_t> represents a flattened resource grid for each port of \c nof_ports. The resource
  /// elements are organised in increasing subcarrier index and then symbol where the resource element of symbol \c l
  /// and subcarrier \c k access is \c port_buffers[port][nof_subc * l + k].
  std::vector<srsvec::aligned_vec<cf_t>> port_buffers;

public:
  resource_grid_impl(unsigned nof_ports, unsigned nof_symb, unsigned nof_subc);
  void put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols) override;
  span<const cf_t>
       put(unsigned port, unsigned l, unsigned k_init, span<const bool> mask, span<const cf_t> symbols) override;
  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override;

  bool       is_empty(unsigned int port) const override;
  void       get(span<cf_t> symbols, unsigned port, span<const resource_grid_coordinate> coordinates) const override;
  span<cf_t> get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const override;
  void       get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const override;

  void set_all_zero() override;
};

} // namespace srsgnb

#endif // SRSGNB_RESOURCE_GRID_IMPL_H
