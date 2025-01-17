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

#include "prs_generator_impl.h"
#include "prs_generator_validator_impl.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/phy/upper/signal_processors/prs/prs_generator_configuration.h"

using namespace srsran;

/// \brief Looks at the output of the validator and, if unsuccessful, fills \c msg with the error message.
///
/// This is used to call the validator inside the process methods only if asserts are active.
[[maybe_unused]] static bool handle_validation(std::string& msg, const error_type<std::string>& err)
{
  bool is_success = err.has_value();
  if (!is_success) {
    msg = err.error();
  }
  return is_success;
}

void prs_generator_impl::generate(resource_grid_writer& grid, const prs_generator_configuration& config)
{
  // Makes sure the configuration is valid.
  [[maybe_unused]] std::string msg;
  srsran_assert(handle_validation(msg, prs_generator_validator_impl().is_valid(config)), "{}", msg);

  for (unsigned i_symbol     = config.start_symbol,
                i_symbol_end = config.start_symbol + static_cast<unsigned>(config.duration);
       i_symbol != i_symbol_end;
       ++i_symbol) {
    // Generate sequence for this symbol.
    generate(i_symbol, config);

    // Map sequence.
    map(grid, i_symbol, config);
  }
}

void prs_generator_impl::generate(unsigned i_symbol, const prs_generator_configuration& config)
{
  // Calculate the pseudo-random sequence initial state.
  unsigned nof_symbols_slot = get_nsymb_per_slot(config.cp);
  unsigned c_init =
      (pow2(22) * (config.n_id_prs / 1024) +
       pow2(10) * (nof_symbols_slot * config.slot.slot_index() + i_symbol + 1) * (2 * (config.n_id_prs % 1024) + 1) +
       config.n_id_prs % 1024) %
      pow2(31);

  // Number of symbols per PRB.
  unsigned nof_symbols_prb = NOF_SUBCARRIERS_PER_RB / static_cast<unsigned>(config.comb_size);

  // Initialize sequence.
  prg->init(c_init);

  // Skip.
  prg->advance(2 * nof_symbols_prb * config.prb_start);

  // Calculate the sequence length.
  unsigned sequence_length = nof_symbols_prb * config.freq_alloc.length();
  sequence.resize(one_layer, sequence_length);

  // Calculate the sequence amplitude.
  float amplitude = convert_dB_to_amplitude(config.power_offset_dB);

  // Generate sequence.
  prg->generate(sequence.get_slice(0), amplitude * M_SQRT1_2);
}

void prs_generator_impl::map(resource_grid_writer& grid, unsigned i_symbol, const prs_generator_configuration& config)
{
  // Determine the resource element offset within each PRB.
  unsigned re_offset = get_freq_offset(config.comb_size, config.start_symbol, i_symbol);

  // Extract comb size.
  unsigned comb_size = static_cast<unsigned>(config.comb_size);

  // Calculate the initial subcarrier.
  unsigned k_init = config.freq_alloc.start() * NOF_SUBCARRIERS_PER_RB + (config.comb_offset + re_offset) % comb_size;

  // Extract the precoding weight matrix for the transmission.
  const precoding_weight_matrix& precoding_weights = config.precoding.get_prg_coefficients(0);

  // Extract the number of transmit ports.
  unsigned nof_ports = precoding_weights.get_nof_ports();

  // Prepare precoded symbol buffer.
  symbols.resize(precoding_weights.get_nof_ports(), sequence.get_nof_re());

  // Apply precoding.
  precoder->apply_precoding(symbols, sequence, config.precoding.get_prg_coefficients(0));

  // Write the precoded symbols for each of the transmission ports.
  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    // Get view of the destination OFDM symbol.
    span<cbf16_t> grid_symbol_view = grid.get_view(i_port, i_symbol);

    // Get view of the symbols to map.
    span<const cbf16_t> symbols_to_map = symbols.get_slice(i_port);

    // Write symbols in the grid OFDM symbol.
    for (unsigned i = 0, i_end = symbols_to_map.size(); i != i_end; ++i) {
      grid_symbol_view[comb_size * i + k_init] = symbols_to_map[i];
    }
  }
}

uint8_t prs_generator_impl::get_freq_offset(srsran::prs_comb_size comb_size, unsigned start_symbol, unsigned i_symbol)
{
  static constexpr std::array<uint8_t, NOF_SUBCARRIERS_PER_RB> row_2  = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
  static constexpr std::array<uint8_t, NOF_SUBCARRIERS_PER_RB> row_4  = {0, 2, 1, 3, 0, 2, 1, 3, 0, 2, 1, 3};
  static constexpr std::array<uint8_t, NOF_SUBCARRIERS_PER_RB> row_6  = {0, 3, 1, 4, 2, 5, 0, 3, 1, 4, 2, 5};
  static constexpr std::array<uint8_t, NOF_SUBCARRIERS_PER_RB> row_12 = {0, 6, 3, 9, 1, 7, 4, 10, 2, 8, 5, 11};

  span<const uint8_t> row;
  switch (comb_size) {
    case prs_comb_size::two:
      row = row_2;
      break;
    case prs_comb_size::four:
      row = row_4;
      break;
    case prs_comb_size::six:
      row = row_6;
      break;
    case prs_comb_size::twelve:
    default:
      row = row_12;
      break;
  }

  srsran_assert(i_symbol >= start_symbol, "Invalid symbol index.");
  return row[i_symbol - start_symbol];
}
