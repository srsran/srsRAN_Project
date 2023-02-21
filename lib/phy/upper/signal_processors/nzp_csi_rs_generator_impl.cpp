/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "nzp_csi_rs_generator_impl.h"
#include "srsran/phy/upper/re_pattern.h"
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

void srsran::nzp_csi_rs_generator_impl::generate_sequence(span<cf_t>      sequence,
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

void srsran::nzp_csi_rs_generator_impl::map(resource_grid_writer& grid, const config_t& config)
{
  unsigned nof_ports = config.ports.size();

  srsgnb_assert(config.symbol_l0 < get_nsymb_per_slot(config.cp), "CSI-RS Mapping Row 1: l_0 outside the valid range.");

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
  mapping_config.nof_ports                = nof_ports;
  csi_rs_pattern pattern_all_port         = get_csi_rs_pattern(mapping_config);

  // Calculate the CSI-RS sequence length for a signel OFDM symbol.
  const unsigned seq_len = get_seq_len(config);

  // Determine the number of symbols in the CDM group.
  const unsigned cdm_group_size = get_cdm_group_size(config.cdm);

  // Stores the NZP-CSI-RS sequence for a single OFDM symbol.
  static_vector<cf_t, MAX_SEQ_LEN> sequence(seq_len);

  // Precoding not currently supported.
  if (config.pmi != 0) {
    srsgnb_assertion_failure("Precoding is not currently supported\n");
  }

  for (unsigned i_port = 0; i_port != nof_ports; ++i_port) {
    // Use the corresponding RE pattern.
    const re_pattern pattern = get_re_pattern_port(pattern_all_port, i_port);

    // Index within the CDM group for the specified port.
    const unsigned cdm_group_idx = i_port % cdm_group_size;
    // Symbol index within the CDM table.
    unsigned cdm_l_idx = 0;
    for (unsigned l = 0; l != get_nsymb_per_slot(config.cp); ++l) {
      if (pattern.symbols.test(l)) {
        // Create whole OFDM symbol mask.
        bounded_bitset<MAX_RB * NRE> mask(MAX_RB * NRE);
        pattern.get_inclusion_mask(mask, l);

        // Select from the mask the region of interest.
        bounded_bitset<MAX_RB* NRE> mask_csi =
            mask.slice(config.start_rb * NRE, (config.start_rb + config.nof_rb) * NRE);

        // Generate the generic sequence.
        generate_sequence(sequence, l, config);

        // Calculate the CDM sequence and write into the resource grid.
        apply_cdm(sequence, sequence, config.cdm, cdm_group_idx, cdm_l_idx++);
        grid.put(config.ports[i_port], l, config.start_rb * NRE, mask_csi, sequence);
      }
    }
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
      srsgnb_assert(
          l_idx <= L_PRIME_MAX_TD2, "l_idx value: {} outside of range: 0..{} for CDM4-FD2-TD2", l_idx, L_PRIME_MAX_TD2);

      // Get the CDM table.
      table = span<const cdm_sequence>(cdm4_fd2_td2_table);

    } else {
      srsgnb_assert(
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
