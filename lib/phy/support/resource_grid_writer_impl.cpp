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
#include "resource_grid_writer_impl.h"
#include "srsran/adt/interval.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/srsvec/copy.h"

using namespace srsran;

unsigned resource_grid_writer_impl::get_nof_ports() const
{
  return data.get_dimension_size(resource_grid_dimensions::port);
}

unsigned resource_grid_writer_impl::get_nof_subc() const
{
  return data.get_dimension_size(resource_grid_dimensions::subc);
}

unsigned resource_grid_writer_impl::get_nof_symbols() const
{
  return data.get_dimension_size(resource_grid_dimensions::symbol);
}

void resource_grid_writer_impl::put(unsigned                             port,
                                    span<const resource_grid_coordinate> coordinates,
                                    span<const cf_t>                     symbols)
{
  srsran_assert(coordinates.size() == symbols.size(),
                "The number of coordinates {} is not equal to the number of symbols {}.",
                coordinates.size(),
                symbols.size());

  srsran_assert(port < get_nof_ports(), "The port index {} is out of range (max {}).", port, get_nof_ports() - 1);

  unsigned count = 0;
  for (const resource_grid_coordinate& coordinate : coordinates) {
    srsran_assert(coordinate.symbol < get_nof_symbols(),
                  "Symbol index {} is out of range (max {}).",
                  coordinate.symbol,
                  get_nof_symbols());
    srsran_assert(coordinate.subcarrier < get_nof_subc(),
                  "Subcarrier index {} is out of range (max {}).",
                  coordinate.subcarrier,
                  get_nof_subc());

    // Select destination OFDM symbol from the resource grid.
    span<cf_t> rg_symbol = data.get_view({coordinate.symbol, port});

    // Write into the desired resource element.
    rg_symbol[coordinate.subcarrier] = symbols[count++];
  }
  empty[port] = false;
}

span<const cf_t> resource_grid_writer_impl::put(unsigned         port,
                                                unsigned         l,
                                                unsigned         k_init,
                                                span<const bool> mask,
                                                span<const cf_t> symbol_buffer)
{
  interval<unsigned>       i_symbol_range(0, get_nof_symbols());
  interval<unsigned, true> mask_size_range(1, get_nof_subc());
  interval<unsigned>       i_port_range(0, get_nof_ports());
  srsran_assert(i_symbol_range.contains(l), "Symbol index (i.e., {}) is out-of-range range {}.", l, i_symbol_range);
  srsran_assert(mask_size_range.contains(mask.size()),
                "The mask size (i.e., {}) is out-of-range range {}.",
                mask.size(),
                i_symbol_range);
  srsran_assert(
      i_port_range.contains(port), "The port identifier (i.e., {}) is out-of-range range {}.", port, i_port_range);

  // Select destination OFDM symbol from the resource grid.
  span<cf_t> rg_symbol = data.get_view({l, port});

  // Iterate mask using AVX2 intrinsics and preset groups of 4 subcarriers.
  for (unsigned i_subc = 0, i_subc_end = mask.size(); i_subc < i_subc_end; ++i_subc) {
    if (mask[i_subc]) {
      rg_symbol[i_subc + k_init] = symbol_buffer.front();
      symbol_buffer              = symbol_buffer.last(symbol_buffer.size() - 1);
    }
  }
  empty[port] = false;

  // Update symbol buffer
  return symbol_buffer;
}

span<const cf_t> resource_grid_writer_impl::put(unsigned                            port,
                                                unsigned                            l,
                                                unsigned                            k_init,
                                                const bounded_bitset<NRE * MAX_RB>& mask,
                                                span<const cf_t>                    symbols)
{
  interval<unsigned, false> l_range(0, get_nof_symbols());
  interval<unsigned, true>  mask_size_range(1, get_nof_subc());
  interval<unsigned, false> port_range(0, get_nof_ports());
  srsran_assert(l_range.contains(l), "Symbol index (i.e., {}) is out of the range {}.", l_range, l_range);
  srsran_assert(
      mask_size_range.contains(mask.size()), "Mask size (i.e., {}) is out of range {}.", mask.size(), mask_size_range);
  srsran_assert(port_range.contains(port), "Port identifier (i.e., {}) is out of range {}.", port, port_range);

  // Get view of the OFDM symbol subcarriers.
  span<cf_t> symb = data.get_view({l, port});

  empty[port] = false;

  unsigned mask_count = mask.count();
  srsran_assert(mask_count <= symbols.size(),
                "The number of active subcarriers (i.e., {}) exceeds the number of symbols (i.e., {}).",
                mask_count,
                symbols.size());

  // Do a straight copy if the elements of the mask are all contiguous.
  if (mask_count and mask.is_contiguous()) {
    srsvec::copy(symb.subspan(mask.find_lowest(), mask_count), symbols.first(mask_count));
    return symbols.last(symbols.size() - mask_count);
  }

  mask.for_each(0, mask.size(), [&](unsigned i_subc) {
    symb[i_subc + k_init] = symbols.front();
    symbols               = symbols.last(symbols.size() - 1);
  });

  return symbols;
}

void resource_grid_writer_impl::put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols)
{
  srsran_assert(
      k_init + symbols.size() <= get_nof_subc(),
      "The initial subcarrier index (i.e., {}) plus the number of symbols (i.e., {}) exceeds the maximum number of "
      "subcarriers (i.e., {})",
      k_init,
      symbols.size(),
      get_nof_subc());
  srsran_assert(l < get_nof_symbols(),
                "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})",
                l,
                get_nof_symbols());
  srsran_assert(port < get_nof_ports(),
                "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})",
                port,
                get_nof_ports());

  // Select destination OFDM symbol from the resource grid.
  span<cf_t> rg_symbol = data.get_view({l, port});

  // Copy resource elements.
  srsvec::copy(rg_symbol.subspan(k_init, symbols.size()), symbols);
  empty[port] = false;
}
