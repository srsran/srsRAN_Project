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

#include "resource_grid_reader_impl.h"
#include "srsran/srsvec/copy.h"

using namespace srsran;

unsigned resource_grid_reader_impl::get_nof_ports() const
{
  return data.get_dimension_size(resource_grid_dimensions::port);
}

unsigned resource_grid_reader_impl::get_nof_subc() const
{
  return data.get_dimension_size(resource_grid_dimensions::subc);
}

unsigned resource_grid_reader_impl::get_nof_symbols() const
{
  return data.get_dimension_size(resource_grid_dimensions::symbol);
}

bool resource_grid_reader_impl::is_empty(unsigned port) const
{
  srsran_assert(port < get_nof_ports(), "Port index {} is out of range (max {})", port, get_nof_ports());
  return is_port_empty(port);
}

span<cf_t> resource_grid_reader_impl::get(span<cf_t>       symbols,
                                          unsigned         port,
                                          unsigned         l,
                                          unsigned         k_init,
                                          span<const bool> mask) const
{
  unsigned mask_size = mask.size();

  srsran_assert(k_init + mask_size <= get_nof_subc(),
                "The initial subcarrier index (i.e., {}) plus the mask size (i.e., {}) exceeds the maximum number of "
                "subcarriers (i.e., {})",
                k_init,
                mask.size(),
                get_nof_subc());
  srsran_assert(l < get_nof_symbols(),
                "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})",
                l,
                get_nof_symbols());
  srsran_assert(port < get_nof_ports(),
                "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})",
                port,
                get_nof_ports());

  // Access the OFDM symbol from the resource grid.
  span<const cf_t> rg_symbol = data.get_view({l, port});

  // Iterate mask.
  unsigned count = 0;
  for (unsigned k = 0; k != mask_size; ++k) {
    if (mask[k]) {
      symbols[count] = rg_symbol[k + k_init];
      count++;
    }
  }

  // Update symbol buffer.
  return symbols.last(symbols.size() - count);
}

span<cf_t> resource_grid_reader_impl::get(span<cf_t>                          symbols,
                                          unsigned                            port,
                                          unsigned                            l,
                                          unsigned                            k_init,
                                          const bounded_bitset<MAX_RB * NRE>& mask) const
{
  srsran_assert(k_init + mask.size() <= get_nof_subc(),
                "The initial subcarrier index (i.e., {}) plus the mask size (i.e., {}) exceeds the maximum number of "
                "subcarriers (i.e., {})",
                k_init,
                mask.size(),
                get_nof_subc());
  srsran_assert(l < get_nof_symbols(),
                "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})",
                l,
                get_nof_symbols());
  srsran_assert(port < get_nof_ports(),
                "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})",
                port,
                get_nof_ports());

  // Get view of the OFDM symbol subcarriers.
  span<const cf_t> symb = data.get_view({l, port}).subspan(k_init, mask.size());

  srsran_assert(mask.count() <= symbols.size(),
                "The number ones in mask {} exceeds the number of symbols {}.",
                mask.count(),
                symbols.size());

  unsigned mask_count = mask.count();
  srsran_assert(mask_count <= symbols.size(),
                "The number of active subcarriers (i.e., {}) exceeds the number of symbols (i.e., {}).",
                mask_count,
                symbols.size());

  // Do a straight copy if the elements of the mask are all contiguous.
  if (mask_count and mask.is_contiguous()) {
    srsvec::copy(symbols.first(mask_count), symb.subspan(mask.find_lowest(), mask_count));
    return symbols.last(symbols.size() - mask_count);
  }

  mask.for_each(0, mask.size(), [symb, &symbols](unsigned i_subc) {
    symbols.front() = symb[i_subc];
    symbols         = symbols.last(symbols.size() - 1);
  });

  return symbols;
}

void resource_grid_reader_impl::get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const
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

  // Access the OFDM symbol from the resource grid.
  span<const cf_t> rg_symbol = data.get_view({l, port});

  // Copy resource elements.
  srsvec::copy(symbols, rg_symbol.subspan(k_init, symbols.size()));
}

span<const cf_t> resource_grid_reader_impl::get_view(unsigned port, unsigned l) const
{
  srsran_assert(l < get_nof_symbols(),
                "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})",
                l,
                get_nof_symbols());
  srsran_assert(port < get_nof_ports(),
                "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})",
                port,
                get_nof_ports());

  // Access the OFDM symbol from the resource grid.
  return data.get_view({l, port});
}
