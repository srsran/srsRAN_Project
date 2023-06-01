/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "resource_grid_impl.h"
#include "srsran/adt/interval.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/srsran_assert.h"
#include <cassert>

using namespace srsran;

resource_grid_impl::resource_grid_impl(unsigned                          nof_ports_,
                                       unsigned                          nof_symb_,
                                       unsigned                          nof_subc_,
                                       std::unique_ptr<channel_precoder> precoder_) :
  empty(nof_ports_), nof_ports(nof_ports_), nof_symb(nof_symb_), nof_subc(nof_subc_), precoder(std::move(precoder_))
{
  // Reserve memory for the internal buffer.
  rg_buffer.reserve({nof_subc, nof_symb, nof_ports});
}

void resource_grid_impl::put(unsigned port, span<const resource_grid_coordinate> coordinates, span<const cf_t> symbols)
{
  srsran_assert(coordinates.size() == symbols.size(),
                "The number of coordinates {} is not equal to the number of symbols {}.",
                coordinates.size(),
                symbols.size());

  srsran_assert(port < nof_ports, "The port index {} is out of range (max {}).", port, nof_ports - 1);

  unsigned count = 0;
  for (const resource_grid_coordinate& coordinate : coordinates) {
    srsran_assert(
        coordinate.symbol < nof_symb, "Symbol index {} is out of range (max {}).", coordinate.symbol, nof_symb);
    srsran_assert(coordinate.subcarrier < nof_subc,
                  "Subcarrier index {} is out of range (max {}).",
                  coordinate.subcarrier,
                  nof_subc);

    // Select destination OFDM symbol from the resource grid.
    span<cf_t> rg_symbol = rg_buffer.get_view<dim_symbol>({coordinate.symbol, port});

    // Write into the desired resource element.
    rg_symbol[coordinate.subcarrier] = symbols[count++];
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
  assert(port < nof_ports);

  // Select destination OFDM symbol from the resource grid.
  span<cf_t> rg_symbol = rg_buffer.get_view<dim_symbol>({l, port});

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

span<const cf_t> resource_grid_impl::put(unsigned                            port,
                                         unsigned                            l,
                                         unsigned                            k_init,
                                         const bounded_bitset<NRE * MAX_RB>& mask,
                                         span<const cf_t>                    symbols)
{
  interval<unsigned, false> l_range(0, nof_symb);
  interval<unsigned, true>  mask_size_range(1, nof_subc);
  interval<unsigned, false> port_range(0, nof_ports);
  srsran_assert(l_range.contains(l), "Symbol index (i.e., {}) is out of the range {}.", l_range, l_range);
  srsran_assert(
      mask_size_range.contains(mask.size()), "Mask size (i.e., {}) is out of range {}.", mask.size(), mask_size_range);
  srsran_assert(port_range.contains(port), "Port identifier (i.e., {}) is out of range {}.", port, port_range);

  // Get view of the OFDM symbol subcarriers.
  span<cf_t> symb = rg_buffer.get_view<dim_symbol>({l, port});

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

void resource_grid_impl::put(unsigned port, unsigned l, unsigned k_init, span<const cf_t> symbols)
{
  srsran_assert(
      k_init + symbols.size() <= nof_subc,
      "The initial subcarrier index (i.e., {}) plus the number of symbols (i.e., {}) exceeds the maximum number of "
      "subcarriers (i.e., {})",
      k_init,
      symbols.size(),
      nof_subc);
  srsran_assert(l < nof_symb, "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})", l, nof_symb);
  srsran_assert(
      port < nof_ports, "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})", port, nof_ports);

  // Select destination OFDM symbol from the resource grid.
  span<cf_t> rg_symbol = rg_buffer.get_view<dim_symbol>({l, port});

  // Copy resource elements.
  srsvec::copy(rg_symbol.subspan(k_init, symbols.size()), symbols);
  empty[port] = false;
}
void resource_grid_impl::set_all_zero()
{
  // For each non-empty port, set the underlying resource elements to zero.
  for (unsigned port = 0; port != nof_ports; ++port) {
    if (!empty[port]) {
      srsvec::zero(rg_buffer.get_view<dim_port>({port}));
      empty[port] = true;
    }
  }
}

void resource_grid_impl::get(span<cf_t> symbols, unsigned port, span<const resource_grid_coordinate> coordinates) const
{
  unsigned nof_re = coordinates.size();

  assert(nof_re == symbols.size());
  assert(port < nof_ports);

  // Iterate through the coordinates and access the desired resource elements.
  for (unsigned i_re = 0; i_re != nof_re; ++i_re) {
    resource_grid_coordinate coordinate = coordinates[i_re];

    assert(coordinate.symbol < nof_symb);
    assert(coordinate.subcarrier < nof_subc);

    // Access the OFDM symbol from the resource grid.
    span<const cf_t> rg_symbol = rg_buffer.get_view<dim_symbol>({coordinate.symbol, port});
    symbols[i_re]              = rg_symbol[coordinate.subcarrier];
  }
}

span<cf_t>
resource_grid_impl::get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init, span<const bool> mask) const
{
  unsigned mask_size = mask.size();

  srsran_assert(k_init + mask_size <= nof_subc,
                "The initial subcarrier index (i.e., {}) plus the mask size (i.e., {}) exceeds the maximum number of "
                "subcarriers (i.e., {})",
                k_init,
                mask.size(),
                nof_subc);
  srsran_assert(l < nof_symb, "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})", l, nof_symb);
  srsran_assert(
      port < nof_ports, "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})", port, nof_ports);

  // Access the OFDM symbol from the resource grid.
  span<const cf_t> rg_symbol = rg_buffer.get_view<dim_symbol>({l, port});

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
                                   unsigned                            port,
                                   unsigned                            l,
                                   unsigned                            k_init,
                                   const bounded_bitset<MAX_RB * NRE>& mask) const
{
  srsran_assert(k_init + mask.size() <= nof_subc,
                "The initial subcarrier index (i.e., {}) plus the mask size (i.e., {}) exceeds the maximum number of "
                "subcarriers (i.e., {})",
                k_init,
                mask.size(),
                nof_subc);
  srsran_assert(l < nof_symb, "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})", l, nof_symb);
  srsran_assert(
      port < nof_ports, "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})", port, nof_ports);

  // Get view of the OFDM symbol subcarriers.
  span<const cf_t> symb = rg_buffer.get_view<dim_symbol>({l, port});

  srsran_assert(mask.count() <= symbols.size(),
                "The number ones in mask {} exceeds the number of symbols {}.",
                mask.count(),
                symbols.size());

  mask.for_each(0, mask.size(), [&](unsigned i_subc) {
    symbols.front() = symb[i_subc + k_init];
    symbols         = symbols.last(symbols.size() - 1);
  });

  return symbols;
}

void resource_grid_impl::get(span<cf_t> symbols, unsigned port, unsigned l, unsigned k_init) const
{
  srsran_assert(
      k_init + symbols.size() <= nof_subc,
      "The initial subcarrier index (i.e., {}) plus the number of symbols (i.e., {}) exceeds the maximum number of "
      "subcarriers (i.e., {})",
      k_init,
      symbols.size(),
      nof_subc);
  srsran_assert(l < nof_symb, "Symbol index (i.e., {}) exceeds the maximum number of symbols (i.e., {})", l, nof_symb);
  srsran_assert(
      port < nof_ports, "Port index (i.e., {}) exceeds the maximum number of ports (i.e., {})", port, nof_ports);

  // Access the OFDM symbol from the resource grid.
  span<const cf_t> rg_symbol = rg_buffer.get_view<dim_symbol>({l, port});

  // Copy resource elements.
  srsvec::copy(symbols, rg_symbol.subspan(k_init, symbols.size()));
}

bool resource_grid_impl::is_empty(unsigned port) const
{
  srsran_assert(port < empty.size(), "Port index {} is out of range (max {})", port, empty.size());
  return empty[port];
}

void resource_grid_impl::map(const re_buffer_reader&        input,
                             const re_pattern_list&         pattern,
                             const precoding_configuration& precoding)
{
  unsigned nof_layers = precoding.get_nof_layers();

  srsran_assert(input.get_nof_slices() == precoding.get_nof_layers(),
                "The input number of layers (i.e., {}) and the precoding number of layers (i.e., {}) are different.",
                input.get_nof_slices(),
                nof_layers);

  unsigned nof_precoding_ports = precoding.get_nof_ports();
  srsran_assert(nof_precoding_ports <= nof_ports,
                "The precoding number of ports (i.e., {}) exceeds the grid number of ports (i.e., {}).",
                precoding.get_nof_ports(),
                nof_ports);

  // PRG size in number of subcarriers.
  unsigned prg_size = precoding.get_prg_size() * NRE;

  // Counter for the number of RE read from the input and mapped to the grid.
  unsigned i_re_buffer = 0;
  for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    // Get the symbol RE mask.
    bounded_bitset<MAX_RB * NRE> symbol_re_mask(nof_subc);
    pattern.get_inclusion_mask(symbol_re_mask, i_symbol);

    // Find the highest used subcarrier. Skip symbol if no active subcarrier.
    int i_highest_subc = symbol_re_mask.find_highest();
    if (i_highest_subc < 0) {
      continue;
    }

    // Resize the mask to the highest subcarrier, ceiling to PRB.
    symbol_re_mask.resize(divide_ceil(i_highest_subc, NRE) * NRE);

    // Number of RE to be allocated for the current symbol.
    unsigned nof_re_symbol = symbol_re_mask.count();

    // Bypass precoding if it has no effect on the signal.
    if ((nof_layers == 1) && (nof_precoding_ports == 1) && (precoding.get_nof_prg() == 1) &&
        (precoding.get_coefficient(0, 0, 0) == 1.0F)) {
      // View over the input RE belonging to the current symbol.
      re_buffer_reader_view input_re_symbol(input, i_re_buffer, nof_re_symbol);

      // Map directly to the grid.
      span<const cf_t> unmapped = put(0, i_symbol, 0, symbol_re_mask, input_re_symbol.get_slice(0));
      srsran_assert(unmapped.empty(), "Not all REs have been mapped to the grid.");
      i_re_buffer += nof_re_symbol;
      continue;
    }

    if ((nof_re_symbol != precoding_buffer.get_nof_re()) ||
        (nof_precoding_ports != precoding_buffer.get_nof_slices())) {
      // Resize the output buffer if the input dimensions don't match.
      precoding_buffer.resize(nof_precoding_ports, nof_re_symbol);
    }

    // Counter for the number of precoded REs for the current symbol.
    unsigned i_precoding_buffer = 0;
    for (unsigned i_prg = 0, nof_prg = precoding.get_nof_prg(), i_subc = 0; i_prg != nof_prg; ++i_prg) {
      // Get the precoding matrix for the current PRG.
      const precoding_weight_matrix& prg_weights = precoding.get_prg_coefficients(i_prg);

      // Number of grid RE belonging to the current PRG for the provided allocation pattern dimensions.
      unsigned nof_subc_prg = std::min(prg_size, static_cast<unsigned>(symbol_re_mask.size()) - i_subc);

      // Mask for the RE belonging to the current PRG.
      bounded_bitset<MAX_RB* NRE> prg_re_mask = symbol_re_mask.slice(i_subc, i_subc + nof_subc_prg);

      // Number of allocated RE for the current PRG.
      unsigned nof_re_prg = prg_re_mask.count();

      // Views of the input and precoder buffers for the REs belonging to the current PRG.
      re_buffer_reader_view input_re_prg(input, i_re_buffer, nof_re_prg);
      re_buffer_writer_view output_re_prg(precoding_buffer, i_precoding_buffer, nof_re_prg);

      // Apply precoding.
      precoder->apply_precoding(output_re_prg, input_re_prg, prg_weights);

      // Advance input and output buffers.
      i_re_buffer += nof_re_prg;
      i_precoding_buffer += nof_re_prg;

      // Advance mask slice.
      i_subc += nof_subc_prg;
    }

    // Assert that the precoding buffer has been filled.
    srsran_assert((i_precoding_buffer == precoding_buffer.get_nof_re()),
                  "The number of precoded RE (i.e., {}) does not match the precoding buffer size (i.e., {}).",
                  i_precoding_buffer,
                  precoding_buffer.get_nof_re());

    for (unsigned i_tx_port = 0; i_tx_port != nof_precoding_ports; ++i_tx_port) {
      // Map the precoded REs to each port for the current symbol.
      span<const cf_t> port_data = precoding_buffer.get_slice(i_tx_port);
      span<const cf_t> unmapped  = put(i_tx_port, i_symbol, 0, symbol_re_mask, port_data);
      srsran_assert(unmapped.empty(), "Not all REs have been mapped to the grid.");
    }
  }

  // Assert that all input REs have been processed.
  srsran_assert((i_re_buffer == input.get_nof_re()),
                "The number of total precoded RE (i.e., {}) does not match the number of total input RE (i.e., {}).",
                i_re_buffer,
                input.get_nof_re());
}
