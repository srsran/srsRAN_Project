#include "resource_grid_impl.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/zero.h"
#include <cassert>

using namespace srsgnb;

resource_grid_impl::resource_grid_impl(unsigned nof_symb_, unsigned nof_subc_) :
  nof_symb(nof_symb_), nof_subc(nof_subc_), buffer(nof_symb * nof_subc)
{
  // Do nothing
}

void resource_grid_impl::put(unsigned l, unsigned k, cf_t value)
{
  assert(l < nof_symb);
  assert(k < nof_subc);
  buffer[l * nof_subc + k] = value;
}

cf_t resource_grid_impl::get(unsigned l, unsigned k) const
{
  assert(l < nof_symb);
  assert(k < nof_subc);
  return buffer[l * nof_subc + k];
}

void resource_grid_impl::put(unsigned l, span<const bool> mask, span<const cf_t>& symbol_buffer)
{
  assert(mask.size() <= nof_subc);

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
void resource_grid_impl::put(unsigned l, unsigned k_init, span<const cf_t> symbols)
{
  assert(k_init + symbols.size() < nof_subc);

  // Copy
  srsvec::copy(buffer.subspan(l * nof_subc + k_init, symbols.size()), symbols);
}
void resource_grid_impl::all_zero()
{
  srsvec::zero(buffer);
}

std::unique_ptr<resource_grid> srsgnb::create_resource_grid(unsigned nof_symbols, unsigned nof_subc)
{
  return std::make_unique<resource_grid_impl>(nof_symbols, nof_subc);
}

void resource_grid_impl::get(unsigned l, span<const bool> mask, span<cf_t>& symbol_buffer)
{
  assert(mask.size() <= nof_subc);

  // Select destination symbol in buffer
  span<cf_t> symb = buffer.subspan(l * nof_subc, nof_subc);

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

void resource_grid_impl::get(unsigned l, unsigned k_init, span<cf_t> symbols)
{
  assert(k_init + symbols.size() < nof_subc);

  // Copy
  srsvec::copy(symbols, buffer.subspan(l * nof_subc + k_init, symbols.size()));
}
