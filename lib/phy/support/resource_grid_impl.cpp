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

#include "resource_grid_impl.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/zero.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

resource_grid_impl::resource_grid_impl(unsigned                          nof_ports_,
                                       unsigned                          nof_symb_,
                                       unsigned                          nof_subc_,
                                       std::unique_ptr<channel_precoder> precoder_) :
  empty(nof_ports_),
  nof_ports(nof_ports_),
  nof_symb(nof_symb_),
  nof_subc(nof_subc_),
  precoder(std::move(precoder_)),
  writer(rg_buffer, empty),
  reader(rg_buffer, empty)
{
  // Reserve memory for the internal buffer.
  rg_buffer.reserve({nof_subc, nof_symb, nof_ports});

  // Set all the resource elements to zero.
  for (unsigned port = 0; port != nof_ports; ++port) {
    srsvec::zero(rg_buffer.get_view<static_cast<unsigned>(resource_grid_dimensions::port)>({port}));
    empty[port] = true;
  }
}

void resource_grid_impl::set_all_zero()
{
  // For each non-empty port, set the underlying resource elements to zero.
  for (unsigned port = 0; port != nof_ports; ++port) {
    if (!empty[port]) {
      srsvec::zero(rg_buffer.get_view<static_cast<unsigned>(resource_grid_dimensions::port)>({port}));
      empty[port] = true;
    }
  }
}

void resource_grid_impl::map(const re_buffer_reader&        input,
                             const re_pattern_list&         pattern,
                             const re_pattern_list&         reserved,
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
    reserved.get_exclusion_mask(symbol_re_mask, i_symbol);

    // Find the highest used subcarrier. Skip symbol if no active subcarrier.
    int i_highest_subc = symbol_re_mask.find_highest();
    if (i_highest_subc < 0) {
      continue;
    }

    // Resize the mask to the highest subcarrier, ceiling to PRB.
    symbol_re_mask.resize(divide_ceil(i_highest_subc + 1, NRE) * NRE);

    // Number of RE to be allocated for the current symbol.
    unsigned nof_re_symbol = symbol_re_mask.count();

    // Bypass precoding if it has no effect on the signal.
    if ((nof_layers == 1) && (nof_precoding_ports == 1) && (precoding.get_nof_prg() == 1) &&
        (precoding.get_coefficient(0, 0, 0) == 1.0F)) {
      // View over the input RE belonging to the current symbol.
      re_buffer_reader_view input_re_symbol(input, i_re_buffer, nof_re_symbol);

      // Map directly to the grid.
      span<const cf_t> unmapped = writer.put(0, i_symbol, 0, symbol_re_mask, input_re_symbol.get_slice(0));
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
      span<const cf_t> unmapped  = writer.put(i_tx_port, i_symbol, 0, symbol_re_mask, port_data);
      srsran_assert(unmapped.empty(), "Not all REs have been mapped to the grid.");
    }
  }

  // Assert that all input REs have been processed.
  srsran_assert((i_re_buffer == input.get_nof_re()),
                "The number of total precoded RE (i.e., {}) does not match the number of total input RE (i.e., {}).",
                i_re_buffer,
                input.get_nof_re());
}

resource_grid_writer& resource_grid_impl::get_writer()
{
  return writer;
}

const resource_grid_reader& resource_grid_impl::get_reader() const
{
  return reader;
}

void resource_grid_impl::map(const re_buffer_reader&        input,
                             const re_pattern_list&         pattern,
                             const precoding_configuration& precoding)
{
  // Map with an empty list of reserved RE patterns.
  map(input, pattern, re_pattern_list(), precoding);
}

void resource_grid_impl::map(span<const cf_t>                    symbols,
                             unsigned                            i_symbol,
                             unsigned                            i_subcarrier,
                             const bounded_bitset<NRE * MAX_RB>& mask,
                             const precoding_weight_matrix&      precoding)
{
  // Extract the number of layers.
  unsigned nof_layers = precoding.get_nof_layers();

  // Extract number of antennas.
  unsigned nof_antennas = precoding.get_nof_ports();

  // Maximum number of subcarriers processed per block.
  unsigned max_nof_subc_block = MAX_NOF_SYMBOLS / nof_layers;

  srsran_assert(mask.count() * nof_layers == symbols.size(),
                "The number of RE (i.e., {}) is not consistent with the number of symbols (i.e., {}) and the number of "
                "layers (i.e., {}).",
                mask.count(),
                symbols.size(),
                nof_layers);

  // Counts the subcarriers that have been processed.
  unsigned subcarrier_offset = 0;

  // Run until all symbols are mapped on the resource grid.
  while (!symbols.empty()) {
    // Calculate the number of pending subcarriers to process.
    unsigned pending_nof_subc = mask.size() - subcarrier_offset;

    // Determine the number of subcarriers to map in this block.
    unsigned nof_subc_block = std::min(pending_nof_subc, max_nof_subc_block);

    // Select a slice of the mask for the portion of subcarriers to map.
    bounded_bitset<NRE* MAX_RB> mask_block = mask.slice(subcarrier_offset, subcarrier_offset + nof_subc_block);

    // Number of RE/subcarriers to map in this block.
    unsigned nof_re_block = mask_block.count();

    // Determine the block size.
    unsigned nof_symbols_block = nof_re_block * nof_layers;

    // Pop symbols for the processing block.
    span<const cf_t> symbols_block = symbols.first(nof_symbols_block);
    symbols                        = symbols.last(symbols.size() - nof_symbols_block);

    // Prepare buffers.
    precoding_buffer.resize(nof_antennas, nof_re_block);
    if (nof_layers == 1) {
      srsvec::copy(precoding_buffer.get_slice(0), symbols_block);
    } else {
      layer_mapping_buffer.resize(nof_layers, nof_re_block);
      // Only if the number of layers is greater than one.
      {
        // Layer map.
        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          span<cf_t> layer_data = layer_mapping_buffer.get_slice(i_layer);
          for (unsigned i_re = 0; i_re != nof_re_block; ++i_re) {
            layer_data[i_re] = symbols_block[i_re * nof_layers + i_layer];
          }
        }

        // Precode.
        precoder->apply_precoding(precoding_buffer, layer_mapping_buffer, precoding);
      }
    }

    // Map for each port.
    for (unsigned i_port = 0; i_port != nof_layers; ++i_port) {
      writer.put(i_port, i_symbol, i_subcarrier + subcarrier_offset, mask_block, precoding_buffer.get_slice(i_port));
    }

    // Increment RE offset.
    subcarrier_offset += nof_subc_block;
  }
}
