#include "resource_grid_impl.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/zero.h"
#include <cassert>

using namespace srsgnb;

resource_grid_impl::resource_grid_impl(unsigned nof_ports_, unsigned nof_symb_, unsigned nof_subc_) :
  nof_ports(nof_ports_), nof_symb(nof_symb_), nof_subc(nof_subc_), port_buffers(nof_ports)
{
  // For each port allocate buffer
  for (auto& buffer : port_buffers) {
    buffer.resize(nof_symb * nof_subc);
  }
}

void resource_grid_impl::put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols)
{
  assert(coordinates.size() == symbols.size());

  // Select buffer from the port index
  assert(port < nof_ports);
  span<cf_t> buffer = port_buffers[port];

  unsigned count = 0;
  for (const resource_grid_coordinate& coordinate : coordinates) {
    assert(coordinate.symbol < nof_symb);
    assert(coordinate.subcarrier < nof_subc);
    buffer[coordinate.symbol * nof_subc + coordinate.subcarrier] = symbols[count++];
  }
}

void resource_grid_impl::put(unsigned port, unsigned l, span<const bool> mask, span<const cf_t>& symbol_buffer)
{
  assert(l < nof_symb);
  assert(mask.size() <= nof_subc);

  // Select buffer from the port index
  assert(port < nof_ports);
  span<cf_t> buffer = port_buffers[port];

  // Select destination symbol in buffer
  span<cf_t> symb = buffer.subspan(l * nof_subc, nof_subc);

  // Iterate mask
  unsigned count = 0;
  for (unsigned k = 0; k != mask.size(); ++k) {
    if (mask[k]) {
      symb[k] = symbol_buffer[count];
      count++;
    }
  }

  // Update symbol buffer
  symbol_buffer = symbol_buffer.last(symbol_buffer.size() - count);
}
void resource_grid_impl::put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols)
{
  assert(l < nof_symb);
  assert(k_init + symbols.size() < nof_subc);

  // Select buffer from the port index
  assert(port < nof_ports);
  span<cf_t> buffer = port_buffers[port];

  // Copy
  srsvec::copy(buffer.subspan(l * nof_subc + k_init, symbols.size()), symbols);
}
void resource_grid_impl::set_all_zero()
{
  // For each port buffer set to zero
  for (auto& buffer : port_buffers) {
    srsvec::zero(buffer);
  }
}

void resource_grid_impl::get(unsigned port, span<const resource_grid_coordinate> coordinates, span<cf_t> symbols) const
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

void resource_grid_impl::get(unsigned port, unsigned l, span<const bool> mask, span<cf_t>& symbol_buffer) const
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
      symbol_buffer[count] = symb[k];
      count++;
    }
  }

  // Update symbol buffer
  symbol_buffer = symbol_buffer.last(symbol_buffer.size() - count);
}

void resource_grid_impl::get(unsigned port, unsigned l, unsigned k_init, span<cf_t> symbols) const
{
  assert(l < nof_symb);
  assert(k_init + symbols.size() <= nof_subc);

  // Select buffer from the port index
  assert(port < nof_ports);
  span<const cf_t> buffer = port_buffers[port];

  // Copy
  srsvec::copy(symbols, buffer.subspan(l * nof_subc + k_init, symbols.size()));
}

std::unique_ptr<resource_grid> srsgnb::create_resource_grid(unsigned nof_ports, unsigned nof_symbols, unsigned nof_subc)
{
  return std::make_unique<resource_grid_impl>(nof_ports, nof_symbols, nof_subc);
}