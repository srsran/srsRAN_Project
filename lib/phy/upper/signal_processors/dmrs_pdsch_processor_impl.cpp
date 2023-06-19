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
        /* Port 1000 */ {0, {+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1001 */ {0, {+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1002 */ {1, {+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1003 */ {1, {+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1004 */ {0, {+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1005 */ {0, {+1.0f, -1.0f}, {+1.0f, -1.0f}},
        /* Port 1006 */ {1, {+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1007 */ {1, {+1.0f, -1.0f}, {+1.0f, -1.0f}},
    }};

const std::array<dmrs_pdsch_processor_impl::params_t, dmrs_type::DMRS_MAX_PORTS_TYPE2>
    dmrs_pdsch_processor_impl::params_type2 = {{
        /* Port 1000 */ {0, {+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1001 */ {0, {+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1002 */ {1, {+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1003 */ {1, {+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1004 */ {2, {+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1005 */ {2, {+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1006 */ {0, {+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1007 */ {0, {+1.0f, -1.0f}, {+1.0f, -1.0f}},
        /* Port 1008 */ {1, {+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1009 */ {1, {+1.0f, -1.0f}, {+1.0f, -1.0f}},
        /* Port 1010 */ {2, {+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1011 */ {2, {+1.0f, -1.0f}, {+1.0f, -1.0f}},
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

void dmrs_pdsch_processor_impl::apply_cdm(re_buffer_writer& re_buffer_symbol,
                                          span<const cf_t>  sequence,
                                          unsigned          symbol,
                                          const config_t&   config)
{
  // Set l_prime to 1 if the symbol follows another one.
  unsigned l_prime = 0;
  if (symbol != 0) {
    l_prime = config.symbols_mask.test(symbol - 1) ? 1 : 0;
  }

  unsigned nof_dmrs_ports = re_buffer_symbol.get_nof_slices();

  // Apply CDM to DM-RS ports other than port zero.
  for (unsigned i_dmrs_port = 0; i_dmrs_port != nof_dmrs_ports; ++i_dmrs_port) {
    const params_t& params = (config.type == dmrs_type::TYPE1) ? params_type1[i_dmrs_port] : params_type2[i_dmrs_port];

    // If no weights are applied, copy the original sequence.
    if (params.w_t[l_prime] == +1.0F && params.w_f[0] == params.w_f[1]) {
      srsvec::copy(re_buffer_symbol.get_slice(i_dmrs_port), sequence);
      continue;
    }

    // Apply w_t weight can be +1 or -1 depending on l_prime and port.
    if (params.w_t[l_prime] != +1.0F) {
      srsvec::sc_prod(sequence, -1, re_buffer_symbol.get_slice(i_dmrs_port));
    } else {
      srsvec::copy(re_buffer_symbol.get_slice(i_dmrs_port), sequence);
    }

    // Apply w_f weight. It can be {+1, +1} or {+1, -1} depending on l_prime and port. On the first case, no operation
    // is required. On the second case, re with odd sequence indexes are multiplied by -1.
    if (params.w_f[0] != params.w_f[1]) {
      for (unsigned i_re = 1, nof_re_symbol = re_buffer_symbol.get_nof_re(); i_re < nof_re_symbol; i_re += 2) {
        re_buffer_symbol.get_slice(i_dmrs_port)[i_re] *= -1;
      }
    }
  }
}

void srsran::dmrs_pdsch_processor_impl::map(resource_grid_mapper& mapper, const config_t& config)
{
  // Number of DM-RS RE in an RB.
  unsigned nof_dmrs_re_rb = config.type.nof_dmrs_per_rb();

  // Number of DM-RS RE in an OFDM symbol.
  unsigned nof_dmrs_re_symbol = nof_dmrs_re_rb * config.rb_mask.count();

  // Number of DM-RS RE in the entire slot.
  unsigned nof_dmrs_re_slot = nof_dmrs_re_symbol * config.symbols_mask.count();

  // Number of logical DM-RS ports. It is equivalent to the number of PDSCH transmit layers.
  unsigned nof_dmrs_ports = config.precoding.get_nof_layers();

  // Resize RE buffer where the DM-RS RE are stored.
  if ((nof_dmrs_re_slot != temp_re.get_nof_re()) || (nof_dmrs_ports != temp_re.get_nof_slices())) {
    temp_re.resize(nof_dmrs_ports, nof_dmrs_re_slot);
  }

  // Number of grid OFDM symbols.
  unsigned nof_symbols = config.symbols_mask.size();

  // For each symbol in the slot....
  unsigned i_gen_dmrs_symbols = 0;
  for (unsigned i_symbol = 0; i_symbol != nof_symbols; ++i_symbol) {
    // Skip symbol if it is not selected.
    if (!config.symbols_mask.test(i_symbol)) {
      continue;
    }

    // View over the RE of all logical DM-RS ports for the current OFDM symbol.
    re_buffer_writer_view re_buffer_symbol(temp_re, i_gen_dmrs_symbols, nof_dmrs_re_symbol);

    // Buffer to hold the DM-RS sequence.
    static_vector<cf_t, MAX_DMRS_SYMBOLS * MAX_DMRS_PER_SYMBOL> sequence(nof_dmrs_re_symbol);

    // Generate sequence for the given symbol.
    sequence_generation(sequence, i_symbol, config);

    // Apply CDM codes.
    apply_cdm(re_buffer_symbol, sequence, i_symbol, config);

    // Advance view.
    i_gen_dmrs_symbols += nof_dmrs_re_symbol;
  }

  srsran_assert((i_gen_dmrs_symbols == nof_dmrs_re_slot),
                "The number of generated DM-RS seymbols (i.e., {}) does not match the expected (i.e., {})",
                i_gen_dmrs_symbols,
                nof_dmrs_re_slot);

  // Allocation pattern for the mapper.
  re_pattern_list allocation;

  if (nof_dmrs_ports > 2) {
    for (unsigned i_dmrs_port = 1; i_dmrs_port != nof_dmrs_ports; ++i_dmrs_port) {
      const params_t& params =
          (config.type == dmrs_type::TYPE1) ? params_type1[i_dmrs_port] : params_type2[i_dmrs_port];

      // Set DM-RS allocation pattern for each DM-RS port.
      re_pattern i_dmrs_pattern;
      i_dmrs_pattern.prb_mask = config.rb_mask;
      i_dmrs_pattern.symbols  = config.symbols_mask;
      i_dmrs_pattern.re_mask  = (config.type == dmrs_type::TYPE1) ? get_re_mask_type_1(params.cdm_group_id)
                                                                  : get_re_mask_type_2(params.cdm_group_id);

      // Combine all port DM-RS mappings into the DM-RS allocation.
      allocation.merge(i_dmrs_pattern);
    }

  } else {
    // Set DM-RS allocation pattern.
    re_pattern dmrs_pattern;
    dmrs_pattern.prb_mask = config.rb_mask;
    dmrs_pattern.symbols  = config.symbols_mask;
    dmrs_pattern.re_mask  = (config.type == dmrs_type::TYPE1) ? get_re_mask_type_1(0) : get_re_mask_type_2(0);
    allocation.merge(dmrs_pattern);

    // Mapping to physical resources for the given symbol.
    mapper.map(temp_re, allocation, config.precoding);
  }
}
