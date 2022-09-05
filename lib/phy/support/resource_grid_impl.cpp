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
#include "srsgnb/support/error_handling.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <cassert>

#ifdef HAVE_AVX2
#include <immintrin.h>
#endif // HAVE_AVX2

using namespace srsgnb;

resource_grid_impl::resource_grid_impl(unsigned nof_ports_, unsigned nof_symb_, unsigned nof_subc_) :
  empty(nof_ports_), nof_ports(nof_ports_), nof_symb(nof_symb_), nof_subc(nof_subc_), port_buffers(nof_ports)
{
  // For each port allocate buffer
  for (auto& buffer : port_buffers) {
    buffer.resize(nof_symb * nof_subc);
  }
}

void resource_grid_impl::put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols)
{
  srsgnb_assert(coordinates.size() == symbols.size(),
                "The number of coordinates {} is not equal to the number of symbols {}.",
                coordinates.size(),
                symbols.size());

  // Select buffer from the port index
  srsgnb_assert(port < nof_ports, "The port index {} is out of range (max {}).", port, nof_ports - 1);
  span<cf_t> buffer = port_buffers[port];

  unsigned count = 0;
  for (const resource_grid_coordinate& coordinate : coordinates) {
    srsgnb_assert(
        coordinate.symbol < nof_symb, "Symbol index {} is out of range (max {}).", coordinate.symbol, nof_symb);
    srsgnb_assert(coordinate.subcarrier < nof_subc,
                  "Subcarrier index {} is out of range (max {}).",
                  coordinate.subcarrier,
                  nof_subc);
    buffer[coordinate.symbol * nof_subc + coordinate.subcarrier] = symbols[count++];
  }
  empty[port] = false;
}

span<const cf_t> resource_grid_impl::put(unsigned         port,
                                         unsigned         l,
                                         unsigned         k_init,
                                         span<const bool> mask,
                                         span<const cf_t> symbol_buffer)
{
  assert(l < nof_symb);
  assert(mask.size() <= nof_subc);

  // Select buffer from the port index
  assert(port < nof_ports);
  span<cf_t> buffer = port_buffers[port];

  // Select destination symbol in buffer
  span<cf_t> symb = buffer.subspan(l * nof_subc, nof_subc);

  // Iterate mask using AVX2 intrinsics and preset groups of 4 subcarriers.
  unsigned i_subc = 0;
#if HAVE_AVX2
  for (unsigned i_subc_end = 32 * (mask.size() / 32); i_subc < i_subc_end; i_subc += 32) {
    // Load 32 consecutive subcarrier mask values in an AVX register.
    __m256i avx_mask = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(mask.data() + i_subc));

    // Get the subcarrier mask packed in an integer
    unsigned mask32 = _mm256_movemask_epi8(_mm256_cmpgt_epi8(avx_mask, _mm256_setzero_si256()));

    // Process presets of 4 bits.
    unsigned offset = 0;
    for (; mask32 != 0; mask32 = mask32 >> 4, offset += 4) {
      switch (mask32 & 0xF) {
        case 0B0000:
          // No subcarrier is active, skip.
          break;
        case 0B0001:
          symb[i_subc + k_init + 0 + offset] = symbol_buffer[0];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 1);
          break;
        case 0B0010:
          symb[i_subc + k_init + 1 + offset] = symbol_buffer[0];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 1);
          break;
        case 0B0011:
          symb[i_subc + k_init + 0 + offset] = symbol_buffer[0];
          symb[i_subc + k_init + 1 + offset] = symbol_buffer[1];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 2);
          break;
        case 0B0100:
          symb[i_subc + k_init + 2 + offset] = symbol_buffer[0];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 1);
          break;
        case 0B0101:
          symb[i_subc + k_init + 0 + offset] = symbol_buffer[0];
          symb[i_subc + k_init + 2 + offset] = symbol_buffer[1];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 2);
          break;
        case 0B0110:
          symb[i_subc + k_init + 1 + offset] = symbol_buffer[0];
          symb[i_subc + k_init + 2 + offset] = symbol_buffer[1];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 2);
          break;
        case 0B0111:
          symb[i_subc + k_init + 0 + offset] = symbol_buffer[0];
          symb[i_subc + k_init + 1 + offset] = symbol_buffer[1];
          symb[i_subc + k_init + 2 + offset] = symbol_buffer[2];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 3);
          break;
        case 0B1000:
          symb[i_subc + k_init + 3 + offset] = symbol_buffer[0];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 1);
          break;
        case 0B1001:
          symb[i_subc + k_init + 0 + offset] = symbol_buffer[0];
          symb[i_subc + k_init + 3 + offset] = symbol_buffer[1];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 2);
          break;
        case 0B1010:
          symb[i_subc + k_init + 1 + offset] = symbol_buffer[0];
          symb[i_subc + k_init + 3 + offset] = symbol_buffer[1];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 2);
          break;
        case 0B1011:
          symb[i_subc + k_init + 0 + offset] = symbol_buffer[0];
          symb[i_subc + k_init + 1 + offset] = symbol_buffer[1];
          symb[i_subc + k_init + 3 + offset] = symbol_buffer[2];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 3);
          break;
        case 0B1100:
          symb[i_subc + k_init + 2 + offset] = symbol_buffer[0];
          symb[i_subc + k_init + 3 + offset] = symbol_buffer[1];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 2);
          break;
        case 0B1101:
          symb[i_subc + k_init + 0 + offset] = symbol_buffer[0];
          symb[i_subc + k_init + 2 + offset] = symbol_buffer[1];
          symb[i_subc + k_init + 3 + offset] = symbol_buffer[2];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 3);
          break;
        case 0B1110:
          symb[i_subc + k_init + 1 + offset] = symbol_buffer[0];
          symb[i_subc + k_init + 2 + offset] = symbol_buffer[1];
          symb[i_subc + k_init + 3 + offset] = symbol_buffer[2];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 3);
          break;
        case 0B1111:
        default:
          symb[i_subc + k_init + 0 + offset] = symbol_buffer[0];
          symb[i_subc + k_init + 1 + offset] = symbol_buffer[1];
          symb[i_subc + k_init + 2 + offset] = symbol_buffer[2];
          symb[i_subc + k_init + 3 + offset] = symbol_buffer[3];
          symbol_buffer                      = symbol_buffer.last(symbol_buffer.size() - 4);
          break;
      }
    }
  }
#endif // HAVE_AVX2

  for (unsigned i_subc_end = mask.size(); i_subc < i_subc_end; ++i_subc) {
    if (mask[i_subc]) {
      symb[i_subc + k_init] = symbol_buffer.front();
      symbol_buffer         = symbol_buffer.last(symbol_buffer.size() - 1);
    }
  }
  empty[port] = false;

  // Update symbol buffer
  return symbol_buffer;
}
void resource_grid_impl::put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols)
{
  report_fatal_error_if_not(l < nof_symb, "Symbol index ({}) is out-of-range (max. {})", l, nof_symb);
  report_fatal_error_if_not(k_init + symbols.size() <= nof_subc,
                            "Subcarrier indexes ({},{}) are out-of-range (max. {})",
                            k_init,
                            symbols.size(),
                            nof_subc);
  report_fatal_error_if_not(port < nof_ports, "Port index ({}) is out-of-range (max. {})", port, nof_ports);

  // Select buffer from the port index
  span<cf_t> buffer = port_buffers[port];

  // Copy
  srsvec::copy(buffer.subspan(l * nof_subc + k_init, symbols.size()), symbols);
  empty[port] = false;
}
void resource_grid_impl::set_all_zero()
{
  // For each port buffer set to zero
  for (unsigned port = 0; port != nof_ports; ++port) {
    if (!empty[port]) {
      srsvec::zero(port_buffers[port]);
      empty[port] = true;
    }
  }
}

void resource_grid_impl::get(span<cf_t> symbols, unsigned port, span<const resource_grid_coordinate> coordinates) const
{
  assert(coordinates.size() == symbols.size());

  // Select buffer from the port index
  assert(port < nof_ports);
  span<const cf_t> buffer = port_buffers[port];

  cf_t* symbol_ptr = symbols.begin();
  for (const resource_grid_coordinate& coordinate : coordinates) {
    assert(coordinate.symbol < nof_symb);
    assert(coordinate.subcarrier < nof_subc);
    *(symbol_ptr++) = buffer[coordinate.symbol * nof_subc + coordinate.subcarrier];
  }
}

span<cf_t>
resource_grid_impl::get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const
{
  assert(l < nof_symb);
  assert(mask.size() <= nof_subc);

  // Select buffer from the port index
  assert(port < nof_ports);
  span<const cf_t> buffer = port_buffers[port];

  // Select destination symbol in buffer
  span<const cf_t> symb = buffer.subspan(l * nof_subc, nof_subc);

  // Iterate mask
  unsigned count = 0;
  for (unsigned k = 0; k != mask.size(); ++k) {
    if (mask[k]) {
      symbols[count] = symb[k + k_init];
      count++;
    }
  }

  // Update symbol buffer
  return symbols.last(symbols.size() - count);
}

void resource_grid_impl::get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const
{
  assert(l < nof_symb);
  assert(k_init + symbols.size() <= nof_subc);

  // Select buffer from the port index
  assert(port < nof_ports);
  span<const cf_t> buffer = port_buffers[port];

  // Copy
  srsvec::copy(symbols, buffer.subspan(l * nof_subc + k_init, symbols.size()));
}

bool resource_grid_impl::is_empty(unsigned port) const
{
  srsgnb_assert(port < empty.size(), "Port index {} is out of range (max {})", port, empty.size());
  return empty[port];
}
