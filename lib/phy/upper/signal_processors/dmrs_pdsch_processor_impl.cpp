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
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

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
        /* Port 1002 */ {2, {+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1003 */ {2, {+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1004 */ {4, {+1.0f, +1.0f}, {+1.0f, +1.0f}},
        /* Port 1005 */ {4, {+1.0f, -1.0f}, {+1.0f, +1.0f}},
        /* Port 1006 */ {0, {+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1007 */ {0, {+1.0f, -1.0f}, {+1.0f, -1.0f}},
        /* Port 1008 */ {2, {+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1009 */ {2, {+1.0f, -1.0f}, {+1.0f, -1.0f}},
        /* Port 1010 */ {4, {+1.0f, +1.0f}, {+1.0f, -1.0f}},
        /* Port 1011 */ {4, {+1.0f, -1.0f}, {+1.0f, -1.0f}},
    }};

void srsran::dmrs_pdsch_processor_impl::sequence_generation(span<cf_t>      sequence,
                                                            unsigned int    symbol,
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

void srsran::dmrs_pdsch_processor_impl::mapping(resource_grid_writer&               grid,
                                                span<const cf_t>                    sequence,
                                                const bounded_bitset<MAX_RB * NRE>& rg_subc_mask,
                                                unsigned                            symbol,
                                                const config_t&                     config)
{
  // Resource elements views for each port.
  static_vector<span<const cf_t>, MAX_PORTS> re(config.ports.size());

  // Set l_prime to 1 if the symbol follows another one.
  unsigned l_prime = 0;
  if (symbol != 0) {
    l_prime = config.symbols_mask.test(symbol - 1) ? 1 : 0;
  }

  // For each port compute the sequence.
  for (unsigned port = 0; port != config.ports.size(); ++port) {
    // Get parameter for this port and symbol.
    const params_t& params = (config.type == dmrs_type::TYPE1) ? params_type1[port] : params_type2[port];

    // If no weights are applied, use sequence reference.
    if (params.w_t[l_prime] == +1.0f && params.w_f[0] == params.w_f[1]) {
      re[port] = sequence;
      continue;
    }

    // Setup temporal RE.
    span<cf_t> temp_re_port = span<cf_t>(static_temp_re[port]).first(sequence.size());

    // Apply w_t weight can be +1 or -1 depending on l_prime and port.
    if (params.w_t[l_prime] != +1.0f) {
      srsvec::sc_prod(sequence, -1, temp_re_port);
    } else {
      srsvec::copy(temp_re_port, sequence);
    }

    // Apply w_f weight. It can be {+1, +1} or {+1, -1} depending on l_prime and port. On the first case, no operation
    // is required. On the second case, re with odd sequence indexes are multiplied by -1.
    if (params.w_f[0] != params.w_f[1]) {
      for (unsigned i = 1; i < temp_re_port.size(); i += 2) {
        temp_re_port[i] *= -1;
      }
    }

    // Setup RE
    re[port] = temp_re_port;
  }

  // For each port put elements in grid.
  for (unsigned port = 0; port != config.ports.size(); ++port) {
    // Get parameter for this port and symbol.
    const params_t& params = (config.type == dmrs_type::TYPE1) ? params_type1[port] : params_type2[port];

    // Put port elements in the resource grid.
    grid.put(config.ports[port], symbol, params.delta, rg_subc_mask, re[port]);
  }
}

void srsran::dmrs_pdsch_processor_impl::map(resource_grid_writer& grid, const config_t& config)
{
  // Resource element allocation within a resource block for PDCCH.
  static const re_prb_mask re_mask_type1 = {
      true, false, true, false, true, false, true, false, true, false, true, false};
  static const re_prb_mask re_mask_type2 = {
      true, true, false, false, false, false, true, true, false, false, false, false};

  // Count number of RB.
  unsigned dmrs_re_count = config.type.nof_dmrs_per_rb() * config.rb_mask.count();

  // Create RG OFDM symbol mask. Identical for all OFDM symbols.
  bounded_bitset<MAX_RB* NRE> rg_subc_mask =
      config.rb_mask.kronecker_product<NRE>((config.type == dmrs_type::TYPE1) ? re_mask_type1 : re_mask_type2);

  // For each symbol in the slot....
  for (unsigned symbol = 0; symbol < MAX_NSYMB_PER_SLOT; ++symbol) {
    // Skip symbol if it is not selected.
    if (!config.symbols_mask.test(symbol)) {
      continue;
    }

    // Generate sequence for the given symbol.
    static_vector<cf_t, MAX_DMRS_PER_SYMBOL> sequence(dmrs_re_count);
    sequence_generation(sequence, symbol, config);

    // Mapping to physical resources for the given symbol.
    mapping(grid, sequence, rg_subc_mask, symbol, config);
  }
}
