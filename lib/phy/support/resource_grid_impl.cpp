/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "resource_grid_impl.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/zero.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <cassert>

using namespace srsgnb;

resource_grid_impl::resource_grid_impl(unsigned nof_ports_, unsigned nof_symb_, unsigned nof_subc_) :
  empty(nof_ports_), nof_ports(nof_ports_), nof_symb(nof_symb_), nof_subc(nof_subc_)
{
  // Reserve memory for the internal buffer.
  rg_buffer.reserve({nof_subc, nof_symb, nof_ports});
}

void resource_grid_impl::put(unsigned port_, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols)
{
  srsgnb_assert(coordinates.size() == symbols.size(),
                "The number of coordinates {} is not equal to the number of symbols {}.",
                coordinates.size(),
                symbols.size());

  srsgnb_assert(port_ < nof_ports, "The port index {} is out of range (max {}).", port_, nof_ports - 1);

  unsigned count = 0;
  for (const resource_grid_coordinate& coordinate : coordinates) {
    srsgnb_assert(
        coordinate.symbol < nof_symb, "Symbol index {} is out of range (max {}).", coordinate.symbol, nof_symb);
    srsgnb_assert(coordinate.subcarrier < nof_subc,
                  "Subcarrier index {} is out of range (max {}).",
                  coordinate.subcarrier,
                  nof_subc);

    // Select destination OFDM symbol from the resource grid.
    span<cf_t> rg_symbol = rg_buffer.get_view<dimensions::symbol>({coordinate.symbol, port_});

    // Write into the desired resource element.
    rg_symbol[coordinate.subcarrier] = symbols[count++];
  }
  empty[port_] = false;
}

span<const cf_t> resource_grid_impl::put(unsigned         port_,
                                         unsigned         l,
                                         unsigned         k_init,
                                         span<const bool> mask,
                                         span<const cf_t> symbol_buffer)
{
  assert(l < nof_symb);
  assert(mask.size() <= nof_subc);
  assert(port_ < nof_ports);

  // Select destination OFDM symbol from the resource grid.
  span<cf_t> rg_symbol = rg_buffer.get_view<dimensions::symbol>({l, port_});

  // Iterate mask using AVX2 intrinsics and preset groups of 4 subcarriers.
  for (unsigned i_subc = 0, i_subc_end = mask.size(); i_subc < i_subc_end; ++i_subc) {
    if (mask[i_subc]) {
      rg_symbol[i_subc + k_init] = symbol_buffer.front();
      symbol_buffer              = symbol_buffer.last(symbol_buffer.size() - 1);
    }
  }
  empty[port_] = false;

  // Update symbol buffer
  return symbol_buffer;
}

span<const cf_t> resource_grid_impl::put(unsigned                            port_,
                                         unsigned                            l,
                                         unsigned                            k_init,
                                         const bounded_bitset<NRE * MAX_RB>& mask,
                                         span<const cf_t>                    symbols)
{
  assert(l < nof_symb);
  assert(mask.size() <= nof_subc);
  assert(port_ < nof_ports);

  // Get view of the OFDM symbol subcarriers.
  span<cf_t> symb = rg_buffer.get_view<dimensions::symbol>({l, port_});

  empty[port] = false;

  unsigned mask_count = mask.count();
  srsgnb_assert(mask_count <= symbols.size(),
                "The number of actibe subcarriers (i.e., {}) exceeds the number of symbols (i.e., {}).",
                mask.count(),
                symbols.size());

  // Do a straight copy if the elements of the mask are all contiguous.
  if (mask.is_contiguous()) {
    srsvec::copy(symb.subspan(mask.find_lowest(), mask_count), symbols.first(mask_count));
    return symbols.last(symbols.size() - mask_count);
  }

  mask.for_each(0, mask.size(), [&](unsigned i_subc) {
    symb[i_subc + k_init] = symbols.front();
    symbols               = symbols.last(symbols.size() - 1);
  });

  return symbols;
}

void resource_grid_impl::put(unsigned port_, unsigned l, unsigned k_init, span<const cf_t> symbols)
{
  srsgnb_assert(
      k_init + symbols.size() <= nof_subc,
      "The initial subcarrier index (i.e., {}) plus the number of symbols (i.e., {}) exceeds the maximum number of "
      "subcarriers (i.e., {})",
      k_init,
      symbols.size(),
      nof_subc);
  srsgnb_assert(l < nof_symb, "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})", l, nof_symb);
  srsgnb_assert(
      port_ < nof_ports, "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})", port_, nof_ports);

  // Select destination OFDM symbol from the resource grid.
  span<cf_t> rg_symbol = rg_buffer.get_view<dimensions::symbol>({l, port_});

  // Copy resource elements.
  srsvec::copy(rg_symbol.subspan(k_init, symbols.size()), symbols);
  empty[port_] = false;
}
void resource_grid_impl::set_all_zero()
{
  // For each non-empty port, set the underlying resource elements to zero.
  for (unsigned port_ = 0; port_ != nof_ports; ++port_) {
    if (!empty[port_]) {
      srsvec::zero(rg_buffer.get_view<dimensions::port>({port_}));
      empty[port_] = true;
    }
  }
}

void resource_grid_impl::get(span<cf_t> symbols, unsigned port_, span<const resource_grid_coordinate> coordinates) const
{
  unsigned nof_re = coordinates.size();

  assert(nof_re == symbols.size());
  assert(port_ < nof_ports);

  // Iterate through the coordinates and access the desired resource elements.
  for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
    resource_grid_coordinate coordinate = coordinates[i_re];

    assert(coordinate.symbol < nof_symb);
    assert(coordinate.subcarrier < nof_subc);

    // Access the OFDM symbol from the resource grid.
    span<const cf_t> rg_symbol = rg_buffer.get_view<dimensions::symbol>({coordinate.symbol, port_});
    symbols[i_re]              = rg_symbol[coordinate.subcarrier];
  }
}

span<cf_t>
resource_grid_impl::get(span<cf_t> symbols, unsigned port_, unsigned l, unsigned k_init, span<const bool> mask) const
{
  unsigned mask_size = mask.size();

  srsgnb_assert(k_init + mask_size <= nof_subc,
                "The initial subcarrier index (i.e., {}) plus the mask size (i.e., {}) exceeds the maximum number of "
                "subcarriers (i.e., {})",
                k_init,
                mask.size(),
                nof_subc);
  srsgnb_assert(l < nof_symb, "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})", l, nof_symb);
  srsgnb_assert(
      port_ < nof_ports, "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})", port_, nof_ports);

  // Access the OFDM symbol from the resource grid.
  span<const cf_t> rg_symbol = rg_buffer.get_view<dimensions::symbol>({l, port_});

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

span<cf_t> resource_grid_impl::get(span<cf_t>                          symbols,
                                   unsigned                            port_,
                                   unsigned                            l,
                                   unsigned                            k_init,
                                   const bounded_bitset<MAX_RB * NRE>& mask) const
{
  srsgnb_assert(k_init + mask.size() <= nof_subc,
                "The initial subcarrier index (i.e., {}) plus the mask size (i.e., {}) exceeds the maximum number of "
                "subcarriers (i.e., {})",
                k_init,
                mask.size(),
                nof_subc);
  srsgnb_assert(l < nof_symb, "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})", l, nof_symb);
  srsgnb_assert(
      port_ < nof_ports, "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})", port_, nof_ports);

  // Get view of the OFDM symbol subcarriers.
  span<const cf_t> symb = rg_buffer.get_view<dimensions::symbol>({l, port_});

  srsgnb_assert(mask.count() <= symbols.size(),
                "The number ones in mask {} exceeds the number of symbols {}.",
                mask.count(),
                symbols.size());

  mask.for_each(0, mask.size(), [&](unsigned i_subc) {
    symbols.front() = symb[i_subc + k_init];
    symbols         = symbols.last(symbols.size() - 1);
  });

  return symbols;
}

void resource_grid_impl::get(span<cf_t> symbols, unsigned port_, unsigned l, unsigned k_init) const
{
  srsgnb_assert(
      k_init + symbols.size() <= nof_subc,
      "The initial subcarrier index (i.e., {}) plus the number of symbols (i.e., {}) exceeds the maximum number of "
      "subcarriers (i.e., {})",
      k_init,
      symbols.size(),
      nof_subc);
  srsgnb_assert(l < nof_symb, "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})", l, nof_symb);
  srsgnb_assert(
      port_ < nof_ports, "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})", port_, nof_ports);

  // Access the OFDM symbol from the resource grid.
  span<const cf_t> rg_symbol = rg_buffer.get_view<dimensions::symbol>({l, port_});

  // Copy resource elements.
  srsvec::copy(symbols, rg_symbol.subspan(k_init, symbols.size()));
}

bool resource_grid_impl::is_empty(unsigned port_) const
{
  srsgnb_assert(port_ < empty.size(), "Port index {} is out of range (max {})", port_, empty.size());
  return empty[port_];
}
