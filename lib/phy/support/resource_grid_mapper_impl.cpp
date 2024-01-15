/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "resource_grid_mapper_impl.h"
#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/phy/support/re_pattern.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

using precoding_buffer_type = static_re_buffer<precoding_constants::MAX_NOF_PORTS, NRE * MAX_RB>;

static concurrent_thread_local_object_pool<precoding_buffer_type>
    precoding_buffers(std::thread::hardware_concurrency());

// Resource element allocation patterns within a resource block for PDSCH DM-RS type 1.
static const re_prb_mask& get_re_mask_type_1(unsigned cdm_group_id)
{
  static constexpr unsigned MAX_CDM_GROUPS_TYPE1 = 2;
  srsran_assert(cdm_group_id < MAX_CDM_GROUPS_TYPE1, "Invalid CDM group ID.");

  static const std::array<re_prb_mask, MAX_CDM_GROUPS_TYPE1> re_mask_type1 = {
      {{true, false, true, false, true, false, true, false, true, false, true, false},
       {false, true, false, true, false, true, false, true, false, true, false, true}}};

  return re_mask_type1[cdm_group_id];
};

// Optimized mapping for PDSCH DM-RS Type 1 mapped on contiguous RBs. It derives the CDM group ID of the input symbols
// from the RE allocation pattern.
static void map_dmrs_type1_contiguous(resource_grid_writer&          writer,
                                      precoding_buffer_type&         precoding_buffer,
                                      const re_buffer_reader&        input,
                                      const re_pattern&              pattern,
                                      const precoding_configuration& precoding,
                                      const channel_precoder&        precoder)
{
  // Obtain the DM-RS CDM group ID.
  unsigned cdm_group_id = pattern.re_mask == get_re_mask_type_1(0) ? 0 : 1;

  // The PDSCH DM-RS Type 1 occupies one of every two resource elements across the OFDM symbol.
  static constexpr unsigned re_stride       = 2;
  static constexpr unsigned nof_dmrs_re_prb = NRE / re_stride;

  unsigned nof_layers          = precoding.get_nof_layers();
  unsigned nof_precoding_ports = precoding.get_nof_ports();

  // PRG size in number of RB.
  unsigned prg_size = precoding.get_prg_size();

  int first_symbol = pattern.symbols.find_lowest(true);
  int end_symbol   = pattern.symbols.find_highest(true) + 1;
  srsran_assert((first_symbol >= 0) && (end_symbol >= 0),
                "At least one OFDM symbol must be used by the allocation pattern.");

  unsigned nof_re_symbol = nof_dmrs_re_prb * pattern.prb_mask.count();

  if ((nof_re_symbol != precoding_buffer.get_nof_re()) || (nof_precoding_ports != precoding_buffer.get_nof_slices())) {
    // Resize the output buffer if the input dimensions don't match.
    precoding_buffer.resize(nof_precoding_ports, nof_re_symbol);
  }

  unsigned first_prb = pattern.prb_mask.find_lowest(true);
  unsigned prb_end   = pattern.prb_mask.find_highest(true) + 1;

  // First subcarrier occupied by a DM-RS symbol.
  unsigned first_subcarrier = first_prb * NRE + cdm_group_id;

  // Counter for the number of RE read from the input and mapped to the grid.
  unsigned i_re_buffer = 0;
  for (int i_symbol = first_symbol; i_symbol != end_symbol; ++i_symbol) {
    // Skip symbol if it is not in the allocation pattern.
    if (!pattern.symbols.test(i_symbol)) {
      continue;
    }

    // Bypass precoding if it has no effect on the signal.
    if ((nof_layers == 1) && (nof_precoding_ports == 1) && (precoding.get_nof_prg() == 1) &&
        (precoding.get_coefficient(0, 0, 0) == 1.0F)) {
      // View over the input RE belonging to the current symbol.
      re_buffer_reader_view input_re_symbol(input, i_re_buffer, nof_re_symbol);

      // Map directly to the grid.
      span<const cf_t> port_data = input_re_symbol.get_slice(0);
      writer.put(0, i_symbol, first_subcarrier, re_stride, port_data);
      i_re_buffer += nof_re_symbol;
      continue;
    }

    // Counter for the number of precoded REs for the current symbol.
    unsigned i_precoding_buffer = 0;
    // First PRG in the allocation pattern.
    unsigned first_prg = first_prb / prg_size;
    for (unsigned i_prg = first_prg, nof_prg = precoding.get_nof_prg(); i_prg != nof_prg; ++i_prg) {
      // Get the precoding matrix for the current PRG.
      const precoding_weight_matrix& prg_weights = precoding.get_prg_coefficients(i_prg);

      // First PRB in the PRG used by the allocation pattern.
      unsigned prg_prb_start = std::max(i_prg * prg_size, first_prb);

      // First PRB outside the range of PRB belonging to the current PRG and used by the allocation pattern.
      unsigned prg_prb_end = std::min((i_prg + 1) * prg_size, prb_end);

      // Number of allocated RE for the current PRG.
      unsigned nof_re_prg = (prg_prb_end - prg_prb_start) * nof_dmrs_re_prb;

      // Views of the input and precoder buffers for the REs belonging to the current PRG.
      re_buffer_reader_view input_re_prg(input, i_re_buffer, nof_re_prg);
      re_buffer_writer_view output_re_prg(precoding_buffer, i_precoding_buffer, nof_re_prg);

      // Apply precoding.
      precoder.apply_precoding(output_re_prg, input_re_prg, prg_weights);

      // Advance input and output buffers.
      i_re_buffer += nof_re_prg;
      i_precoding_buffer += nof_re_prg;
    }

    // Assert that the precoding buffer has been filled.
    srsran_assert((i_precoding_buffer == precoding_buffer.get_nof_re()),
                  "The number of precoded RE (i.e., {}) does not match the precoding buffer size (i.e., {}).",
                  i_precoding_buffer,
                  precoding_buffer.get_nof_re());

    for (unsigned i_tx_port = 0; i_tx_port != nof_precoding_ports; ++i_tx_port) {
      // Map the precoded REs to each port for the current symbol.
      span<const cf_t> port_data = precoding_buffer.get_slice(i_tx_port);
      writer.put(i_tx_port, i_symbol, first_subcarrier, re_stride, port_data);
    }
  }
}

resource_grid_mapper_impl::resource_grid_mapper_impl(unsigned                          nof_ports_,
                                                     unsigned                          nof_subc_,
                                                     resource_grid_writer&             writer_,
                                                     std::unique_ptr<channel_precoder> precoder_) :
  nof_ports(nof_ports_), nof_subc(nof_subc_), writer(writer_), precoder(std::move(precoder_))
{
  srsran_assert(nof_ports <= max_nof_ports,
                "The number of ports (i.e., {}) exceeds the maximum number of ports (i.e., {}).",
                nof_ports,
                static_cast<unsigned>(max_nof_ports));
  srsran_assert(nof_subc <= max_nof_subcarriers,
                "The number of subcarriers (i.e., {}) exceeds the maximum number of subcarriers (i.e., {}).",
                nof_subc,
                static_cast<unsigned>(max_nof_subcarriers));
}

void resource_grid_mapper_impl::map(const re_buffer_reader&        input,
                                    const re_pattern&              pattern,
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

  precoding_buffer_type& precoding_buffer = precoding_buffers.get();

  bool is_dmrs_type1 = pattern.prb_mask.is_contiguous(true) &&
                       (pattern.re_mask == get_re_mask_type_1(0) || pattern.re_mask == get_re_mask_type_1(1));

  if (is_dmrs_type1) {
    // Optimized contiguous DM-RS Type 1 mapping.
    map_dmrs_type1_contiguous(writer, precoding_buffer, input, pattern, precoding, *precoder);
    return;
  }

  // PRG size in number of subcarriers.
  unsigned prg_size = precoding.get_prg_size() * NRE;

  int first_symbol = pattern.symbols.find_lowest(true);
  int end_symbol   = pattern.symbols.find_highest(true) + 1;
  srsran_assert((first_symbol >= 0) && (end_symbol >= 0),
                "At least one OFDM symbol must be used by the allocation pattern.");

  // Get the symbol RE mask. It is the same for all allocated OFDM symbols.
  bounded_bitset<max_nof_subcarriers> symbol_re_mask(nof_subc);
  pattern.get_inclusion_mask(symbol_re_mask, first_symbol);

  // Find the highest used subcarrier. Skip symbol if no active subcarrier.
  int i_highest_subc = symbol_re_mask.find_highest();
  srsran_assert(i_highest_subc >= 0, "At least one subcarrier must be used by the allocation pattern.");

  // Resize the mask to the highest subcarrier, ceiling to PRB.
  symbol_re_mask.resize(divide_ceil(i_highest_subc + 1, NRE) * NRE);

  // Number of RE to be allocated for each OFDM symbol in the pattern.
  unsigned nof_re_symbol = symbol_re_mask.count();

  if ((nof_re_symbol != precoding_buffer.get_nof_re()) || (nof_precoding_ports != precoding_buffer.get_nof_slices())) {
    // Resize the output buffer if the input dimensions don't match.
    precoding_buffer.resize(nof_precoding_ports, nof_re_symbol);
  }

  // Counter for the number of RE read from the input and mapped to the grid.
  unsigned i_re_buffer = 0;
  for (int i_symbol = first_symbol; i_symbol != end_symbol; ++i_symbol) {
    // Skip symbol if it is not in the allocation pattern.
    if (!pattern.symbols.test(i_symbol)) {
      continue;
    }

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

    // Counter for the number of precoded REs for the current symbol.
    unsigned i_precoding_buffer = 0;
    for (unsigned i_prg = 0, nof_prg = precoding.get_nof_prg(), i_subc = 0; i_prg != nof_prg; ++i_prg) {
      // Get the precoding matrix for the current PRG.
      const precoding_weight_matrix& prg_weights = precoding.get_prg_coefficients(i_prg);

      // Number of grid RE belonging to the current PRG for the provided allocation pattern dimensions.
      unsigned nof_subc_prg = std::min(prg_size, static_cast<unsigned>(symbol_re_mask.size()) - i_subc);

      // Mask for the RE belonging to the current PRG.
      bounded_bitset<max_nof_subcarriers> prg_re_mask = symbol_re_mask.slice(i_subc, i_subc + nof_subc_prg);

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

void resource_grid_mapper_impl::map(symbol_buffer&                 buffer,
                                    const re_pattern_list&         pattern,
                                    const re_pattern_list&         reserved,
                                    const precoding_configuration& precoding,
                                    unsigned                       re_skip)
{
  precoding_buffer_type& precoding_buffer = precoding_buffers.get();

  // The number of layers is equal to the number of ports.
  unsigned nof_layers = precoding.get_nof_layers();

  // Extract number of antennas.
  unsigned nof_antennas = precoding.get_nof_ports();

  // Verify that the number of layers is consistent with the number of ports.
  interval<unsigned, true> nof_antennas_range(1, nof_ports);
  srsran_assert(nof_antennas_range.contains(nof_antennas),
                "The number of antennas (i.e., {}) must be in range {}",
                nof_antennas,
                nof_antennas_range);
  interval<unsigned, true> nof_layers_range(1, nof_antennas);
  srsran_assert(nof_layers_range.contains(nof_layers),
                "The number of layers (i.e., {}) must be in range {}",
                nof_layers,
                nof_layers_range);

  unsigned re_count = 0;
  for (unsigned i_symbol = 0; i_symbol != MAX_NSYMB_PER_SLOT; ++i_symbol) {
    // Get the symbol RE mask.
    bounded_bitset<max_nof_subcarriers> symbol_re_mask(max_nof_subcarriers);
    pattern.get_inclusion_mask(symbol_re_mask, i_symbol);
    reserved.get_exclusion_mask(symbol_re_mask, i_symbol);

    // Find the highest used subcarrier. Skip symbol if no active subcarrier.
    int i_highest_subc = symbol_re_mask.find_highest();
    if (i_highest_subc < 0) {
      continue;
    }

    // Get the number of active RE in the OFDM symbol. Skip symbol if the number of active RE does not reach the skip.
    unsigned nof_re_symbol = symbol_re_mask.count();
    if (re_count + nof_re_symbol < re_skip) {
      re_count += nof_re_symbol;
      continue;
    }

    // Iterate all precoding PRGs.
    unsigned prg_size = precoding.get_prg_size() * NRE;
    for (unsigned i_prg = 0, nof_prg = precoding.get_nof_prg(); i_prg != nof_prg; ++i_prg) {
      // Get the precoding matrix for the current PRG.
      const precoding_weight_matrix& prg_weights = precoding.get_prg_coefficients(i_prg);

      // Get the subcarrier interval for the PRG.
      unsigned i_subc = i_prg * prg_size;

      // Number of grid RE belonging to the current PRG for the provided allocation pattern dimensions.
      unsigned nof_subc_prg = std::min(prg_size, static_cast<unsigned>(i_highest_subc + 1) - i_subc);

      // Mask for the RE belonging to the current PRG.
      bounded_bitset<max_nof_subcarriers> prg_re_mask = symbol_re_mask.slice(i_subc, i_subc + nof_subc_prg);

      // Skip PRG if no RE is selected.
      if (prg_re_mask.none()) {
        continue;
      }

      // Get the number of active RE in the PRG. Skip PRG if the number of active RE does not reach the skip.
      unsigned nof_re_prg = prg_re_mask.count();
      if (re_count + nof_re_prg < re_skip) {
        re_count += nof_re_prg;
        continue;
      }

      // Advance subcarrier offset to reach the skip count.
      unsigned subc_offset = prg_re_mask.find_lowest();
      while (re_count < re_skip) {
        // Calculate the maximum number of subcarriers that can be processed in one block.
        unsigned max_nof_subc_block = re_skip - re_count;

        // Calculate the number of pending subcarriers to process.
        unsigned nof_subc_pending = nof_subc_prg - subc_offset;
        srsran_assert(nof_subc_pending != 0, "The number of pending subcarriers cannot be zero.");

        // Select the number of subcarriers to process in a block.
        unsigned nof_subc_block = std::min(nof_subc_pending, max_nof_subc_block);

        // Get the allocation mask for the block.
        bounded_bitset<max_nof_subcarriers> block_mask = prg_re_mask.slice(subc_offset, subc_offset + nof_subc_block);

        // Count the number of resource elements to map in the block.
        unsigned nof_re_block = block_mask.count();

        re_count += nof_re_block;
        subc_offset += nof_subc_block;
      }

      // Process PRG in blocks smaller than or equal to max_block_size subcarriers.
      while (subc_offset != nof_subc_prg) {
        // Calculate the maximum number of subcarriers that can be processed in one block.
        unsigned max_nof_subc_block = buffer.get_max_block_size() / nof_layers;

        // Calculate the number of pending subcarriers to process.
        unsigned nof_subc_pending = nof_subc_prg - subc_offset;

        // Select the number of subcarriers to process in a block.
        unsigned nof_subc_block = std::min(nof_subc_pending, max_nof_subc_block);
        srsran_assert(nof_subc_block != 0, "The number of pending subcarriers cannot be zero.");

        // Get the allocation mask for the block.
        bounded_bitset<max_nof_subcarriers> block_mask = prg_re_mask.slice(subc_offset, subc_offset + nof_subc_block);

        // Count the number of resource elements to map in the block.
        unsigned nof_re_block = block_mask.count();
        if (nof_re_block > 0) {
          // Calculate the number of symbols to generate.
          unsigned nof_symbols_block = nof_re_block * nof_layers;

          // Prepare destination of the modulation buffer.
          span<const ci8_t> block = buffer.pop_symbols(nof_symbols_block);

          // Prepare buffers.
          precoding_buffer.resize(nof_antennas, nof_re_block);

          // Layer map and precoding.
          precoder->apply_layer_map_and_precoding(precoding_buffer, block, prg_weights);

          // Map for each port.
          for (unsigned i_port = 0; i_port != nof_antennas; ++i_port) {
            writer.put(i_port, i_symbol, i_subc + subc_offset, block_mask, precoding_buffer.get_slice(i_port));
          }

          // Early return  if the buffer is empty.
          if (buffer.empty()) {
            return;
          }
        }

        // Increment the subcarrier offset.
        subc_offset += nof_subc_block;
      }
    }
  }
}
