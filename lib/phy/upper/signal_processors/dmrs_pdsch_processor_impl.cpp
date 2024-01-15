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

#include "dmrs_pdsch_processor_impl.h"
#include "dmrs_helper.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

// Resource element allocation patterns within a resource block for PDSCH DM-RS type 1.
static const re_prb_mask& get_re_mask_type_1(unsigned cdm_group_id)
{
  static constexpr unsigned MAX_CDM_GROUPS_TYPE1 = 2;

  static const std::array<re_prb_mask, MAX_CDM_GROUPS_TYPE1> re_mask_type1 = {
      {{true, false, true, false, true, false, true, false, true, false, true, false},
       {false, true, false, true, false, true, false, true, false, true, false, true}}};

  return re_mask_type1[cdm_group_id];
};

// Resource element allocation patterns within a resource block for PDSCH DM-RS type 2.
static const re_prb_mask& get_re_mask_type_2(unsigned cdm_group_id)
{
  static constexpr unsigned MAX_CDM_GROUPS_TYPE2 = 3;

  static std::array<const re_prb_mask, MAX_CDM_GROUPS_TYPE2> re_mask_type2 = {
      {{true, true, false, false, false, false, true, true, false, false, false, false},
       {false, false, true, true, false, false, false, false, true, true, false, false},
       {false, false, false, false, true, true, false, false, false, false, true, true}}};

  return re_mask_type2[cdm_group_id];
};

const std::array<dmrs_pdsch_processor_impl::params_t, dmrs_type::DMRS_MAX_PORTS_TYPE1>
    dmrs_pdsch_processor_impl::params_type1 = {{
        /* Port 1000 */ {{+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1001 */ {{+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1002 */ {{+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1003 */ {{+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1004 */ {{+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1005 */ {{+1.0f, -1.0f}, {+1.0f, -1.0f}},
        /* Port 1006 */ {{+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1007 */ {{+1.0f, -1.0f}, {+1.0f, -1.0f}},
    }};

const std::array<dmrs_pdsch_processor_impl::params_t, dmrs_type::DMRS_MAX_PORTS_TYPE2>
    dmrs_pdsch_processor_impl::params_type2 = {{
        /* Port 1000 */ {{+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1001 */ {{+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1002 */ {{+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1003 */ {{+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1004 */ {{+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1005 */ {{+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1006 */ {{+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1007 */ {{+1.0f, -1.0f}, {+1.0f, -1.0f}},
        /* Port 1008 */ {{+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1009 */ {{+1.0f, -1.0f}, {+1.0f, -1.0f}},
        /* Port 1010 */ {{+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1011 */ {{+1.0f, -1.0f}, {+1.0f, -1.0f}},
    }};

void srsran::dmrs_pdsch_processor_impl::sequence_generation(span<cf_t>      sequence,
                                                            unsigned        symbol,
                                                            const config_t& config) const
{
  // Get signal amplitude.
  float amplitude = M_SQRT1_2 * config.amplitude;

  // Extract parameters to calculate the PRG initial state.
  unsigned nslot    = config.slot.slot_index();
  unsigned nidnscid = config.scrambling_id;
  unsigned nscid    = config.n_scid ? 1 : 0;
  unsigned nsymb    = get_nsymb_per_slot(cyclic_prefix::NORMAL);

  // Calculate initial sequence state.
  unsigned c_init = ((nsymb * nslot + symbol + 1) * (2 * nidnscid + 1) * pow2(17) + (2 * nidnscid + nscid)) % pow2(31);

  // Initialize sequence.
  prg->init(c_init);

  // Generate sequence.
  dmrs_sequence_generate(
      sequence, *prg, amplitude, config.reference_point_k_rb, config.type.nof_dmrs_per_rb(), config.rb_mask);
}

void dmrs_pdsch_processor_impl::apply_cdm(span<cf_t>       sequence,
                                          span<const cf_t> base_sequence,
                                          unsigned         dmrs_port,
                                          unsigned         symbol,
                                          const config_t&  config)
{
  // Set l_prime to 1 if the symbol follows another one.
  unsigned l_prime = 0;
  if (symbol != 0) {
    l_prime = config.symbols_mask.test(symbol - 1) ? 1 : 0;
  }

  // Apply CDM to DM-RS ports other than port zero.
  const params_t& params = (config.type == dmrs_type::TYPE1) ? params_type1[dmrs_port] : params_type2[dmrs_port];

  // If no weights are applied, copy the original sequence.
  if ((params.w_t[l_prime] == +1.0F) && (params.w_f[0] == params.w_f[1])) {
    srsvec::copy(sequence, base_sequence);
    return;
  }

  // Apply w_t weight can be +1 or -1 depending on l_prime and port.
  if (params.w_t[l_prime] != +1.0F) {
    srsvec::sc_prod(base_sequence, -1, sequence);
  } else {
    srsvec::copy(sequence, base_sequence);
  }

  // Apply w_f weight. It can be {+1, +1} or {+1, -1} depending on l_prime and port. On the first case, no operation
  // is required. On the second case, RE with odd sequence indices are multiplied by -1.
  if (params.w_f[0] != params.w_f[1]) {
    for (unsigned i_re = 1, nof_re_symbol = sequence.size(); i_re < nof_re_symbol; i_re += 2) {
      sequence[i_re] *= -1;
    }
  }
}

void srsran::dmrs_pdsch_processor_impl::map(resource_grid_mapper& mapper, const config_t& config)
{
  // Number of DM-RS RE in an OFDM symbol.
  unsigned nof_dmrs_re_symbol = config.type.nof_dmrs_per_rb() * config.rb_mask.count();

  // Number of DM-RS RE in the entire slot.
  unsigned nof_dmrs_re_slot = nof_dmrs_re_symbol * config.symbols_mask.count();

  // Number of logical DM-RS ports. It is equivalent to the number of PDSCH transmit layers.
  unsigned nof_dmrs_ports = config.precoding.get_nof_layers();

  // Number of physical antenna ports.
  unsigned nof_antenna_ports = config.precoding.get_nof_ports();

  // Number of grid OFDM symbols.
  unsigned nof_symbols = config.symbols_mask.size();

  // CDM group size in number of DM-RS ports.
  static constexpr unsigned cdm_group_size = 2;

  // Calculate number of CDM groups.
  unsigned nof_cdm_groups = divide_ceil(nof_dmrs_ports, cdm_group_size);

  // Iterate each CDM group.
  for (unsigned i_cdm_group = 0; i_cdm_group != nof_cdm_groups; ++i_cdm_group) {
    // Number of DM-RS ports belonging to the current CDM group.
    unsigned nof_dmrs_ports_cdm = std::min(cdm_group_size, nof_dmrs_ports - (cdm_group_size * i_cdm_group));

    // Resize RE buffer where the DM-RS RE are stored.
    temp_re.resize(nof_dmrs_ports_cdm, nof_dmrs_re_slot);

    // Prepare precoding configuration for the CDM group.
    precoding_configuration cdm_group_precoding(nof_dmrs_ports_cdm, nof_antenna_ports, 1, MAX_RB);

    // For each symbol in the slot....
    unsigned i_gen_dmrs_symbols = 0;
    for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
      // Skip symbol if it is not selected.
      if (!config.symbols_mask.test(i_symbol)) {
        continue;
      }

      // Buffer to hold the DM-RS sequence.
      span<cf_t> base_sequence = temp_re.get_slice(0).subspan(i_gen_dmrs_symbols, nof_dmrs_re_symbol);

      // Generate sequence for the given symbol.
      sequence_generation(base_sequence, i_symbol, config);

      // Advance view.
      i_gen_dmrs_symbols += nof_dmrs_re_symbol;
    }

    srsran_assert((i_gen_dmrs_symbols == nof_dmrs_re_slot),
                  "The number of generated DM-RS seymbols (i.e., {}) does not match the expected (i.e., {})",
                  i_gen_dmrs_symbols,
                  nof_dmrs_re_slot);

    // Set DM-RS allocation pattern of the CDM group.
    re_pattern dmrs_pattern_cdm;
    dmrs_pattern_cdm.prb_mask = config.rb_mask;
    dmrs_pattern_cdm.symbols  = config.symbols_mask;
    dmrs_pattern_cdm.re_mask =
        (config.type == dmrs_type::TYPE1) ? get_re_mask_type_1(i_cdm_group) : get_re_mask_type_2(i_cdm_group);

    // Iterate each port within the CDM group
    for (unsigned i_cdm_port = 0; i_cdm_port != nof_dmrs_ports_cdm; ++i_cdm_port) {
      // Current DM-RS port.
      unsigned i_dmrs_port = (cdm_group_size * i_cdm_group) + i_cdm_port;

      // Load the port coefficients for the CDM port.
      for (unsigned i_ant_port = 0, i_port_end = nof_antenna_ports; i_ant_port != i_port_end; ++i_ant_port) {
        for (unsigned i_prg = 0, i_prg_end = config.precoding.get_nof_prg(); i_prg != i_prg_end; ++i_prg) {
          // Extract the coefficient for mapping the DM-RS port to the antenna port.
          cf_t coefficient = config.precoding.get_coefficient(i_dmrs_port, i_ant_port, i_prg);

          // Set coefficient in the CDM group precoding.
          cdm_group_precoding.set_coefficient(coefficient, i_cdm_port, i_ant_port, i_prg);
        }
      }

      // Skip CDM weights for first DM-RS port.
      if (i_dmrs_port == 0) {
        continue;
      }

      // Apply CDM weights for each symbol.
      unsigned nof_proc_re_cdm = 0;
      for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
        // Skip symbol if it is not selected.
        if (!config.symbols_mask.test(i_symbol)) {
          continue;
        }

        // Get view of the base DM-RS sequence.
        span<const cf_t> base_sequence = temp_re.get_slice(0).subspan(nof_proc_re_cdm, nof_dmrs_re_symbol);

        // Get view of the CDM sequence destination.
        span<cf_t> sequence = temp_re.get_slice(i_cdm_port).subspan(nof_proc_re_cdm, nof_dmrs_re_symbol);

        // Apply the CDM codes.
        apply_cdm(sequence, base_sequence, i_dmrs_port, i_symbol, config);

        // Advance view.
        nof_proc_re_cdm += nof_dmrs_re_symbol;
      }

      srsran_assert((nof_proc_re_cdm == i_gen_dmrs_symbols),
                    "The length of the CDM-coded DM-RS (i.e., {}) for DM-RS port {} does not match the DM-RS sequence "
                    "length (i.e., {})",
                    nof_proc_re_cdm,
                    i_dmrs_port,
                    i_gen_dmrs_symbols);
    }

    // Map the CDM group symbols into the resource grid.
    mapper.map(temp_re, dmrs_pattern_cdm, cdm_group_precoding);
  }
}
