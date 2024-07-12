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
#include "resource_grid_dimensions.h"
#include "srsran/phy/support/resource_grid_writer.h"

namespace srsran {

/// Implements the resource grid writer interface.
class resource_grid_writer_impl : public resource_grid_writer
{
public:
  using storage_type = tensor<static_cast<unsigned>(resource_grid_dimensions::all), cbf16_t, resource_grid_dimensions>;

  /// Constructs a resource grid writer implementation from a tensor.
  resource_grid_writer_impl(storage_type& data_, std::atomic<unsigned>& empty_) : data(data_), empty(empty_) {}

  // See interface for documentation.
  unsigned get_nof_ports() const override;

  // See interface for documentation.
  unsigned get_nof_subc() const override;

  // See interface for documentation.
  unsigned get_nof_symbols() const override;

  // See interface for documentation.
  span<const cf_t> put(unsigned                            port,
                       unsigned                            l,
                       unsigned                            k_init,
                       const bounded_bitset<NRE * MAX_RB>& mask,
                       span<const cf_t>                    symbols) override;

  // See interface for documentation.
  span<const cbf16_t> put(unsigned                            port,
                          unsigned                            l,
                          unsigned                            k_init,
                          const bounded_bitset<NRE * MAX_RB>& mask,
                          span<const cbf16_t>                 symbols) override;

  // See interface for documentation.
  void put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols) override;

  // See interface for documentation.
  void put(unsigned port, unsigned l, unsigned k_init, unsigned stride, span<const cf_t> symbols) override;

  // See interface for documentation.
  span<cbf16_t> get_view(unsigned port, unsigned l) override;

  /// Helper function to mark port as not empty.
  inline void clear_empty(unsigned i_port) { empty &= ~(1U << i_port); }

private:
  storage_type&          data;
  std::atomic<unsigned>& empty;
};

} // namespace srsran
