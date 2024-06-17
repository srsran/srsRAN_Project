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

#include "../../phy/support/resource_grid_dimensions.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/srsvec/conversion.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/complex_normal_random.h"

namespace srsran {

/// Implements a resource grid reader that contains always the same random data.
class ru_dummy_rx_resource_grid : public resource_grid_reader
{
public:
  ru_dummy_rx_resource_grid(unsigned seed, unsigned nof_subc, unsigned nof_symbols, unsigned nof_ports) :
    rg_buffer({nof_subc, nof_symbols, nof_ports})
  {
    std::mt19937                      rgen(seed);
    complex_normal_distribution<cf_t> dist(0, 1);

    span<cbf16_t> data = rg_buffer.get_data();
    std::generate(data.begin(), data.end(), [&rgen, &dist]() { return dist(rgen); });
  }

  // See resource_grid_base for documentation.
  unsigned get_nof_ports() const override { return rg_buffer.get_dimension_size(resource_grid_dimensions::port); }

  // See resource_grid_base for documentation.
  unsigned get_nof_subc() const override { return rg_buffer.get_dimension_size(resource_grid_dimensions::subc); }

  // See resource_grid_base for documentation.
  unsigned get_nof_symbols() const override { return rg_buffer.get_dimension_size(resource_grid_dimensions::symbol); }

  // See resource_grid_reader for documentation.
  bool is_empty(unsigned port) const override
  {
    // This grid is never empty.
    return false;
  }

  // See resource_grid_reader for documentation.
  bool is_empty() const override
  {
    // This grid is never empty.
    return false;
  }

  // See resource_grid_reader for documentation.
  span<cf_t> get(span<cf_t>                          symbols,
                 unsigned                            port,
                 unsigned                            l,
                 unsigned                            k_init,
                 const bounded_bitset<MAX_RB * NRE>& mask) const override
  {
    // Count number of elements.
    unsigned nof_symbols = mask.count();

    return get(symbols, nof_symbols);
  }

  // See resource_grid_reader for documentation.
  span<cbf16_t> get(span<cbf16_t>                       symbols,
                    unsigned                            port,
                    unsigned                            l,
                    unsigned                            k_init,
                    const bounded_bitset<MAX_RB * NRE>& mask) const override
  {
    // Count number of elements.
    unsigned nof_symbols = mask.count();

    return get(symbols, nof_symbols);
  }

  // See resource_grid_reader for documentation.
  void get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, unsigned stride) const override
  {
    srsvec::convert(symbols, rg_buffer.get_data().first(symbols.size()));
  }

  // See resource_grid_reader for documentation.
  void get(span<cbf16_t> symbols, unsigned port, unsigned l, unsigned k_init) const override
  {
    srsvec::copy(symbols, rg_buffer.get_data().first(symbols.size()));
  }

  // See resource_grid_reader for documentation.
  span<const cbf16_t> get_view(unsigned port, unsigned l) const override { return rg_buffer.get_view({l, port}); }

private:
  /// Gets a number of symbols.
  span<cf_t> get(span<cf_t> symbols, unsigned nof_symbols) const
  {
    srsvec::convert(symbols.first(nof_symbols), rg_buffer.get_data().first(nof_symbols));

    return symbols.last(symbols.size() - nof_symbols);
  }
  /// Gets a number of symbols.
  span<cbf16_t> get(span<cbf16_t> symbols, unsigned nof_symbols) const
  {
    srsvec::copy(symbols.first(nof_symbols), rg_buffer.get_data().first(nof_symbols));

    return symbols.last(symbols.size() - nof_symbols);
  }

  /// \brief Stores the resource grid data.
  ///
  /// The resource grid buffer is a three-dimensional array with the dimensions representing, in order, subcarriers,
  /// OFDM symbols and antenna ports.
  dynamic_tensor<static_cast<unsigned>(resource_grid_dimensions::all), cbf16_t, resource_grid_dimensions> rg_buffer;
};

} // namespace srsran
