/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/srsvec/conversion.h"
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
  span<cbf16_t> symb = data.get_view({l, port}).subspan(k_init, mask.size());

  clear_empty(port);

  unsigned mask_count = mask.count();
  srsran_assert(mask_count <= symbols.size(),
                "The number of active subcarriers (i.e., {}) exceeds the number of symbols (i.e., {}).",
                mask_count,
                symbols.size());

  // Do a straight copy if the elements of the mask are all contiguous.
  if (mask_count and mask.is_contiguous()) {
    srsvec::convert(symb.subspan(mask.find_lowest(), mask_count), symbols.first(mask_count));
    return symbols.last(symbols.size() - mask_count);
  }

  mask.for_each(0, mask.size(), [&symb, &symbols](unsigned i_subc) {
    symb[i_subc] = to_cbf16(symbols.front());
    symbols      = symbols.last(symbols.size() - 1);
  });

  return symbols;
}

span<const cbf16_t> resource_grid_writer_impl::put(unsigned                            port,
                                                   unsigned                            l,
                                                   unsigned                            k_init,
                                                   const bounded_bitset<NRE * MAX_RB>& mask,
                                                   span<const cbf16_t>                 symbols)
{
  interval<unsigned, false> l_range(0, get_nof_symbols());
  interval<unsigned, true>  mask_size_range(1, get_nof_subc());
  interval<unsigned, false> port_range(0, get_nof_ports());
  srsran_assert(l_range.contains(l), "Symbol index (i.e., {}) is out of the range {}.", l_range, l_range);
  srsran_assert(
      mask_size_range.contains(mask.size()), "Mask size (i.e., {}) is out of range {}.", mask.size(), mask_size_range);
  srsran_assert(port_range.contains(port), "Port identifier (i.e., {}) is out of range {}.", port, port_range);

  // Get view of the OFDM symbol subcarriers.
  span<cbf16_t> symb = data.get_view({l, port}).subspan(k_init, mask.size());

  clear_empty(port);

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

  mask.for_each(0, mask.size(), [&symb, &symbols](unsigned i_subc) {
    symb[i_subc] = symbols.front();
    symbols      = symbols.last(symbols.size() - 1);
  });

  return symbols;
}

void resource_grid_writer_impl::put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols)
{
  srsran_assert(k_init + symbols.size() <= get_nof_subc(),
                "The initial subcarrier index (i.e., {}) plus the number of symbols (i.e., {}) exceeds the maximum "
                "number of subcarriers (i.e., {})",
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
  span<cbf16_t> rg_symbol = data.get_view({l, port});

  // Copy resource elements.
  srsvec::convert(rg_symbol.subspan(k_init, symbols.size()), symbols);
  clear_empty(port);
}

void resource_grid_writer_impl::put(unsigned            port,
                                    unsigned            l,
                                    unsigned            k_init,
                                    unsigned            stride,
                                    span<const cbf16_t> symbols)
{
  unsigned nof_symbols = symbols.size();
  srsran_assert(
      k_init + ((nof_symbols - 1) * stride) < get_nof_subc(),
      "The initial subcarrier index (i.e., {}) plus the number of RE (i.e., {}) exceeds the maximum number of "
      "subcarriers (i.e., {})",
      k_init,
      (((nof_symbols - 1) * stride) + 1),
      get_nof_subc());
  srsran_assert(l < get_nof_symbols(),
                "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})",
                l,
                get_nof_symbols());
  srsran_assert(port < get_nof_ports(),
                "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})",
                port,
                get_nof_ports());

  // Insert symbols.
  span<cbf16_t> rg_symbol = data.get_view({l, port});
  for (unsigned i_symbol = 0, i_re = k_init; i_symbol != nof_symbols; ++i_symbol) {
    rg_symbol[i_re] = symbols[i_symbol];
    i_re += stride;
  }

  clear_empty(port);
}

span<cbf16_t> resource_grid_writer_impl::get_view(unsigned port, unsigned l)
{
  srsran_assert(l < get_nof_symbols(),
                "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})",
                l,
                get_nof_symbols());
  srsran_assert(port < get_nof_ports(),
                "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})",
                port,
                get_nof_ports());
  clear_empty(port);
  return data.get_view({l, port});
}
