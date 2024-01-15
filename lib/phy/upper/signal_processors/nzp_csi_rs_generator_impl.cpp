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

#include "nzp_csi_rs_generator_impl.h"
#include "srsran/phy/support/re_pattern.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/ran/csi_rs/csi_rs_config_helpers.h"
#include "srsran/ran/csi_rs/csi_rs_pattern.h"

using namespace srsran;

bool nzp_csi_rs_configuration_validator_impl::is_valid(const nzp_csi_rs_generator::config_t& config)
{
  return true;
}

const std::array<const nzp_csi_rs_generator_impl::cdm_sequence, 2> nzp_csi_rs_generator_impl::fd_cdm2_table = {{
    {{+1.0f, +1.0f}, {0, 0, 0, 0}},
    {{+1.0f, -1.0f}, {0, 0, 0, 0}},
}};

const std::array<const nzp_csi_rs_generator_impl::cdm_sequence, 4> nzp_csi_rs_generator_impl::cdm4_fd2_td2_table = {{
    {{+1.0f, +1.0f}, {+1.0f, +1.0f, 0, 0}},
    {{+1.0f, -1.0f}, {+1.0f, +1.0f, 0, 0}},
    {{+1.0f, +1.0f}, {+1.0f, -1.0f, 0, 0}},
    {{+1.0f, -1.0f}, {+1.0f, -1.0f, 0, 0}},
}};

const std::array<const nzp_csi_rs_generator_impl::cdm_sequence, 8> nzp_csi_rs_generator_impl::cdm8_fd2_td4_table = {{
    {{+1.0f, +1.0f}, {+1.0f, +1.0f, +1.0f, +1.0f}},
    {{+1.0f, -1.0f}, {+1.0f, +1.0f, +1.0f, +1.0f}},
    {{+1.0f, +1.0f}, {+1.0f, -1.0f, +1.0f, -1.0f}},
    {{+1.0f, -1.0f}, {+1.0f, -1.0f, +1.0f, -1.0f}},
    {{+1.0f, +1.0f}, {+1.0f, +1.0f, -1.0f, -1.0f}},
    {{+1.0f, -1.0f}, {+1.0f, +1.0f, -1.0f, -1.0f}},
    {{+1.0f, +1.0f}, {+1.0f, -1.0f, -1.0f, +1.0f}},
    {{+1.0f, -1.0f}, {+1.0f, -1.0f, -1.0f, +1.0f}},
}};

// Gets a RE pattern from the CSI-RS pattern for a given port.
static re_pattern get_re_pattern_port(const csi_rs_pattern& pattern_all_ports, unsigned i_port)
{
  return re_pattern(pattern_all_ports.rb_begin,
                    pattern_all_ports.rb_end,
                    pattern_all_ports.rb_stride,
                    pattern_all_ports.prb_patterns[i_port].re_mask,
                    pattern_all_ports.prb_patterns[i_port].symbol_mask);
}

// Calculates the number of sequence elements that must be discarded from the
// pseudo-random number generator for the CSI-RS sequence generation.
static unsigned get_nof_skipped_elements(const nzp_csi_rs_generator::config_t& config)
{
  // Calculate the first occupied PRB.
  unsigned first_prb = 0;
  if (config.freq_density == csi_rs_freq_density_type::one || config.freq_density == csi_rs_freq_density_type::three) {
    first_prb = config.start_rb;
  } else if (config.freq_density == csi_rs_freq_density_type::dot5_even_RB) {
    first_prb = config.start_rb + config.start_rb % 2;
  } else if (config.freq_density == csi_rs_freq_density_type::dot5_odd_RB) {
    first_prb = config.start_rb + (1 - config.start_rb % 2);
  }

  // Determine the start of the sequence. The sequence elements that correspond to PRB's
  // below the first occupied PRB are skipped.
  unsigned nof_advance = 0;
  if (config.freq_density == csi_rs_freq_density_type::three) {
    nof_advance = 3 * first_prb;
  } else if (config.freq_density == csi_rs_freq_density_type::one) {
    if (config.csi_rs_mapping_table_row == 2) {
      nof_advance = first_prb;
    } else {
      nof_advance = 2 * first_prb;
    }
  } else if (config.freq_density == csi_rs_freq_density_type::dot5_even_RB) {
    if (config.csi_rs_mapping_table_row == 2) {
      nof_advance = first_prb / 2;
    } else {
      nof_advance = first_prb;
    }
  } else if (config.freq_density == csi_rs_freq_density_type::dot5_odd_RB) {
    if (config.csi_rs_mapping_table_row == 2) {
      nof_advance = first_prb / 2;
    } else {
      nof_advance = first_prb;
    }
  }
  return nof_advance;
}

void nzp_csi_rs_generator_impl::generate_sequence(span<cf_t>      sequence,
                                                  const unsigned  symbol,
                                                  const config_t& config) const
{
  // Slot index.
  const unsigned nslot = config.slot.slot_index();
  // Number of OFDM symbols per slot.
  const unsigned nsymb = get_nsymb_per_slot(config.cp);
  // Scrambling ID.
  const unsigned n_id = config.scrambling_id;

  // Calculate initial state for the pseudo-random sequence.
  const unsigned c_init = (pow2(10) * (nsymb * nslot + symbol + 1) * (2 * n_id + 1) + n_id) % pow2(31);

  // Initialise the pseudo-random sequence generator.
  prg->init(c_init);

  // Determine the symbol amplitude.
  const float amplitude = M_SQRT1_2 * config.amplitude;

  // Calculate the number of sequence elements to skip from the start.
  const unsigned nof_advance = get_nof_skipped_elements(config);

  // Skip the undesired first elements of the sequence.
  prg->advance(2 * nof_advance);

  // Generate the sequence from the adequate starting point.
  prg->generate(sequence, amplitude);
}

// Computes the CSI-RS sequence length for a single OFDM symbol.
static unsigned get_seq_len(const nzp_csi_rs_generator::config_t& config)
{
  // The CSI-RS sequence length varies according to the density.
  unsigned seq_len = config.nof_rb;
  if (config.freq_density == csi_rs_freq_density_type::dot5_even_RB ||
      config.freq_density == csi_rs_freq_density_type::dot5_odd_RB) {
    seq_len /= 2;

    // For Densities of 0.5, and with and odd-numbered PRB range, in some cases the number of occupied PRB needs to be
    // rounded up instead of truncated.
    if (config.nof_rb % 2 != 0) {
      if (config.start_rb % 2 != 0) {
        if (config.freq_density == csi_rs_freq_density_type::dot5_odd_RB) {
          ++seq_len;
        }
      } else {
        if (config.freq_density == csi_rs_freq_density_type::dot5_even_RB) {
          ++seq_len;
        }
      }
    }
  } else if (config.freq_density == csi_rs_freq_density_type::three) {
    seq_len *= 3;
  }

  // The CSI-RS sequence increases in length if CDM is used.
  if (config.cdm == csi_rs_cdm_type::fd_CDM2 || config.cdm == csi_rs_cdm_type::cdm4_FD2_TD2 ||
      config.cdm == csi_rs_cdm_type::cdm8_FD2_TD4) {
    seq_len *= 2;
  }
  return seq_len;
}

void nzp_csi_rs_generator_impl::map(resource_grid_mapper& mapper, const config_t& config)
{
  unsigned nof_ports = csi_rs::get_nof_csi_rs_ports(config.csi_rs_mapping_table_row);

  srsran_assert(nof_ports == config.precoding.get_nof_ports(),
                "CSI-RS number of ports, i.e., {}, does not match the precoding number of ports, i.e., {}.",
                nof_ports,
                config.precoding.get_nof_ports());

  srsran_assert(nof_ports == config.precoding.get_nof_layers(),
                "CSI-RS precoding number of ports, i.e., {} and number of layers, i.e., {}, must be equal.",
                nof_ports,
                config.precoding.get_nof_layers());

  interval<unsigned, false> l0_range(0, get_nsymb_per_slot(config.cp));
  srsran_assert(l0_range.contains(config.symbol_l0),
                "CSI-RS Mapping: l_0, i.e., {} outside the valid range, i.e., {}.",
                config.symbol_l0,
                l0_range);

  // Generate the grid allocations patterns for each port.
  csi_rs_pattern_configuration mapping_config;
  mapping_config.start_rb                 = config.start_rb;
  mapping_config.nof_rb                   = config.nof_rb;
  mapping_config.csi_rs_mapping_table_row = config.csi_rs_mapping_table_row;
  mapping_config.freq_allocation_ref_idx  = config.freq_allocation_ref_idx;
  mapping_config.symbol_l0                = config.symbol_l0;
  mapping_config.symbol_l1                = config.symbol_l1;
  mapping_config.cdm                      = config.cdm;
  mapping_config.freq_density             = config.freq_density;
  csi_rs_pattern pattern_all_port         = get_csi_rs_pattern(mapping_config);

  // Calculate the CSI-RS sequence length for each OFDM symbol.
  unsigned seq_len = get_seq_len(config);

  // Determine the number of symbols in the CDM group.
  unsigned cdm_group_size = get_cdm_group_size(config.cdm);

  // Calculate number of CDM groups.
  unsigned nof_cdm_groups = nof_ports / cdm_group_size;

  // Iterate each CDM group.
  for (unsigned i_cdm_group = 0; i_cdm_group != nof_cdm_groups; ++i_cdm_group) {
    // Use the corresponding RE pattern.
    re_pattern pattern = get_re_pattern_port(pattern_all_port, i_cdm_group * cdm_group_size);

    // Count the number of OFDM symbols.
    unsigned nof_symbols = pattern.symbols.count();

    // Prepare data.
    data.resize(cdm_group_size, seq_len * nof_symbols);

    // Prepare precoding configuration for the CDM group.
    precoding_configuration cdm_group_precoding(cdm_group_size, nof_ports, 1, MAX_RB);

    // Prepare base sequence for each symbol within the slot.
    for (unsigned i_symbol_slot = 0, i_symbol_slot_end = get_nsymb_per_slot(config.cp), i_symbol_cdm = 0;
         i_symbol_slot != i_symbol_slot_end;
         ++i_symbol_slot) {
      // Skip symbol if not used.
      if (!pattern.symbols.test(i_symbol_slot)) {
        continue;
      }

      // Get view of the sequence destination.
      span<cf_t> sequence = data.get_slice(0).subspan(i_symbol_cdm * seq_len, seq_len);

      // Generate the generic sequence.
      generate_sequence(sequence, i_symbol_slot, config);

      // Increment the CDM group symbol counter.
      ++i_symbol_cdm;
    }

    // Iterate each port within the CDM group
    for (unsigned i_cdm_port = 0; i_cdm_port != cdm_group_size; ++i_cdm_port) {
      // Calculate the absolute port identifier.
      unsigned i_port_layer = i_cdm_group * cdm_group_size + i_cdm_port;

      // Make sure all the CDM group shares the same pattern.
      srsran_assert(pattern == get_re_pattern_port(pattern_all_port, i_port_layer),
                    "All ports within a CDM group must have the same pattern.");

      // Load the port coefficients for the CDM port.
      for (unsigned i_port = 0, i_port_end = nof_ports; i_port != i_port_end; ++i_port) {
        for (unsigned i_prg = 0, i_prg_end = config.precoding.get_nof_prg(); i_prg != i_prg_end; ++i_prg) {
          // Extract the coefficient for mapping the NZP-CSI-RS to the port.
          cf_t coefficient = config.precoding.get_coefficient(i_port_layer, i_port, i_prg);

          // Set coefficient in the CDM group precoding.
          cdm_group_precoding.set_coefficient(coefficient, i_cdm_port, i_port, i_prg);
        }
      }

      // Skip CDM weights for first port of the CDM group.
      if (i_cdm_port == 0) {
        continue;
      }

      // Apply CDM weights for each symbol.
      for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
        // Get view of the base sequence.
        span<const cf_t> base_sequence = data.get_slice(0).subspan(i_symbol * seq_len, seq_len);

        // Get view of the sequence.
        span<cf_t> sequence = data.get_slice(i_cdm_port).subspan(i_symbol * seq_len, seq_len);

        // Apply the CDM sequence.
        apply_cdm(sequence, base_sequence, config.cdm, i_cdm_port, i_symbol);
      }
    }

    mapper.map(data, pattern, cdm_group_precoding);
  }
}

void nzp_csi_rs_generator_impl::apply_cdm(span<cf_t>            seq_out,
                                          span<const cf_t>      seq_in,
                                          const csi_rs_cdm_type cdm,
                                          const unsigned        cdm_idx,
                                          const unsigned        l_idx)
{
  // Maximum CDM RE and symbol index values.
  constexpr unsigned       K_PRIME_MAX     = 0b1;
  constexpr unsigned       L_PRIME_MAX_TD2 = 0b1;
  constexpr unsigned       L_PRIME_MAX_TD4 = 0b11;
  span<const cdm_sequence> table;

  const unsigned seq_in_len = seq_in.size();

  // Apply FD-CDM2.
  if (cdm == csi_rs_cdm_type::fd_CDM2) {
    // Get the CDM table.
    table = span<const cdm_sequence>(fd_cdm2_table);

    // CDM frequency index.
    unsigned k_prime = 0;
    // Iterate through the input sequence.
    for (unsigned k = 0; k != seq_in_len; ++k) {
      // Get the required output symbols and apply the CDM tables.
      seq_out[k] = table[cdm_idx].w_f[k_prime++] * seq_in[k];
      // Wrap k_prime.
      k_prime &= K_PRIME_MAX;
    }
  }

  // Apply CDM4-FD2-TD2 or CDM8-FD2-TD4.
  else if (cdm == csi_rs_cdm_type::cdm4_FD2_TD2 || cdm == csi_rs_cdm_type::cdm8_FD2_TD4) {
    if (cdm == csi_rs_cdm_type::cdm4_FD2_TD2) {
      srsran_assert(
          l_idx <= L_PRIME_MAX_TD2, "l_idx value: {} outside of range: 0..{} for CDM4-FD2-TD2", l_idx, L_PRIME_MAX_TD2);

      // Get the CDM table.
      table = span<const cdm_sequence>(cdm4_fd2_td2_table);

    } else {
      srsran_assert(
          l_idx <= L_PRIME_MAX_TD4, "l_idx value: {} outside of range: 0..{} for CDM8-FD2-TD4", l_idx, L_PRIME_MAX_TD4);

      // Get the CDM table.
      table = span<const cdm_sequence>(cdm8_fd2_td4_table);
    }

    // CDM frequency index.
    unsigned k_prime = 0;
    // Iterate through the input sequence.
    for (unsigned k = 0; k != seq_in_len; ++k) {
      // Get the required output symbols and apply the CDM tables.
      seq_out[k] = table[cdm_idx].w_t[l_idx] * table[cdm_idx].w_f[k_prime++] * seq_in[k];
      // Wrap k_prime.
      k_prime &= K_PRIME_MAX;
    }
  }

  // If no CDM, do nothing.
  else {
    return;
  }
}
