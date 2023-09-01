/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "resource_grid_mapper_test_doubles.h"
#include "srsran/phy/upper/channel_processors/channel_processor_formatters.h"

using namespace srsran;

void resource_grid_mapper_spy::map(const re_buffer_reader&        input,
                                   const re_pattern_list&         pattern,
                                   const re_pattern_list&         reserved,
                                   const precoding_configuration& precoding)
{
  srsran_assert(precoding.get_nof_prg() == 1, "Number of PRG must be one.");

  unsigned i_re = 0;
  for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    // Get the symbol RE mask.
    bounded_bitset<MAX_RB * NRE> symbol_re_mask(MAX_RB * NRE);
    pattern.get_inclusion_mask(symbol_re_mask, i_symbol);
    reserved.get_exclusion_mask(symbol_re_mask, i_symbol);

    // Find the highest used subcarrier. Skip symbol if no active subcarrier.
    int i_highest_subc = symbol_re_mask.find_highest();
    if (i_highest_subc < 0) {
      continue;
    }

    // Resize the mask to the highest subcarrier, ceiling to PRB.
    symbol_re_mask.resize(divide_ceil(i_highest_subc + 1, NRE) * NRE);

    // Count the number of mapped RE.
    unsigned nof_re = symbol_re_mask.count();

    for (unsigned i_layer = 0, nof_layers = input.get_nof_slices(); i_layer != nof_layers; ++i_layer) {
      // Map each layer without precoding.
      span<const cf_t> layer_data = input.get_slice(i_layer);

      unsigned i_layer_port = 0;
      cf_t     coefficient  = 0;
      for (unsigned i_port = 0, i_port_end = precoding.get_nof_ports(); i_port != i_port_end; ++i_port) {
        cf_t coefficient_port = precoding.get_coefficient(i_layer, i_port, 0);

        // The precoding coefficient is not zero.
        if (coefficient_port != cf_t()) {
          if (coefficient != cf_t()) {
            report_fatal_error("Invalid precoding configuration {}.", precoding);
          }

          coefficient  = coefficient_port;
          i_layer_port = i_port;
        }
      }

      span<const cf_t> unmapped =
          rg_writer_spy.put(i_layer_port, i_symbol, 0, symbol_re_mask, layer_data.subspan(i_re, nof_re));
      srsran_assert(unmapped.empty(), "Not all REs have been mapped to the grid. {} remaining.", unmapped.size());
    }

    // Advance RE counter.
    i_re += nof_re;
  }

  srsran_assert(i_re == input.get_nof_re(),
                "The nuber of mapped RE (i.e., {}) does not match the number of input RE (i.e., {}).",
                i_re,
                input.get_nof_re());
}

void resource_grid_mapper_spy::map(const re_buffer_reader&        input,
                                   const re_pattern_list&         pattern,
                                   const precoding_configuration& precoding)
{
  // Map with an empty list of reserved RE patterns.
  map(input, pattern, re_pattern_list(), precoding);
}

void resource_grid_mapper_spy::map(symbol_buffer&                 buffer,
                                   const re_pattern_list&         pattern,
                                   const re_pattern_list&         reserved,
                                   const precoding_configuration& precoding)
{
  static constexpr unsigned MAX_NOF_SYMBOLS = 512;

  // Temporary data storage.
  static_re_buffer<4, MAX_NOF_SYMBOLS> temp_mapped;

  unsigned max_block_size = std::min(MAX_NOF_SYMBOLS, buffer.get_max_block_size());

  // The number of layers is equal to the number of ports.
  unsigned nof_layers = precoding.get_nof_layers();

  // Extract number of antennas.
  unsigned nof_antennas = precoding.get_nof_ports();

  for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    // Get the symbol RE mask.
    bounded_bitset<MAX_RB * NRE> symbol_re_mask(MAX_RB * NRE);
    pattern.get_inclusion_mask(symbol_re_mask, i_symbol);
    reserved.get_exclusion_mask(symbol_re_mask, i_symbol);

    // Find the highest used subcarrier. Skip symbol if no active subcarrier.
    int i_highest_subc = symbol_re_mask.find_highest();
    if (i_highest_subc < 0) {
      continue;
    }

    // Iterate all precoding PRGs.
    unsigned prg_size = precoding.get_prg_size() * NRE;
    for (unsigned i_prg = 0, nof_prg = precoding.get_nof_prg(); i_prg != nof_prg; ++i_prg) {
      // Get the subcarrier interval for the PRG.
      unsigned i_subc = i_prg * prg_size;

      // Number of grid RE belonging to the current PRG for the provided allocation pattern dimensions.
      unsigned nof_subc_prg = std::min(prg_size, static_cast<unsigned>(i_highest_subc + 1) - i_subc);

      // Mask for the RE belonging to the current PRG.
      bounded_bitset<MAX_RB* NRE> prg_re_mask = symbol_re_mask.slice(i_subc, i_subc + nof_subc_prg);

      // Skip PRG if no RE is selected.
      if (prg_re_mask.none()) {
        continue;
      }

      // Process PRG in blocks smaller than or equal to max_block_size subcarriers.
      unsigned subc_offset = prg_re_mask.find_lowest();
      while (subc_offset != nof_subc_prg) {
        // Calculate the maximum number of subcarriers that can be processed in one block.
        unsigned max_nof_subc_block = max_block_size / nof_layers;

        // Calculate the number of pending subcarriers to process.
        unsigned nof_subc_pending = nof_subc_prg - subc_offset;
        srsran_assert(nof_subc_pending != 0, "The number of pending subcarriers cannot be zero.");

        // Select the number of subcarriers to process in a block.
        unsigned nof_subc_block = std::min(nof_subc_pending, max_nof_subc_block);

        // Get the allocation mask for the block.
        bounded_bitset<MAX_RB* NRE> block_mask = prg_re_mask.slice(subc_offset, subc_offset + nof_subc_block);

        unsigned nof_re_block      = block_mask.count();
        unsigned nof_symbols_block = nof_re_block * nof_layers;

        // Prepare destination of the modulation buffer.
        span<const cf_t> block = buffer.pop_symbols(nof_symbols_block);

        // Prepare buffers.
        temp_mapped.resize(nof_antennas, nof_re_block);

        // Layer map.
        for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
          span<cf_t> layer_data = temp_mapped.get_slice(i_layer);
          for (unsigned i_re = 0; i_re != nof_re_block; ++i_re) {
            layer_data[i_re] = block[i_re * nof_layers + i_layer];
          }
        }

        // Map for each port.
        for (unsigned i_port = 0; i_port != nof_layers; ++i_port) {
          rg_writer_spy.put(i_port, i_symbol, i_subc + subc_offset, block_mask, temp_mapped.get_slice(i_port));
        }

        // Increment the subcarrier offset.
        subc_offset += nof_subc_block;
      }
    }
  }
}
