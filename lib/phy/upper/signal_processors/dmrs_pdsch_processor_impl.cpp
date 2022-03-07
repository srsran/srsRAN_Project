/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dmrs_pdsch_processor_impl.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/sc_prod.h"

using namespace srsgnb;

const std::array<dmrs_pdsch_processor_impl::params_t, DMRS_MAX_PORTS_TYPE1> dmrs_pdsch_processor_impl::params_type1 = {{
    /* Port 1000 */ {0, {+1.0f, +1.0f}, {+1.0f, +1.0f}},
    /* Port 1001 */ {0, {+1.0f, -1.0f}, {+1.0f, +1.0f}},
    /* Port 1002 */ {1, {+1.0f, +1.0f}, {+1.0f, +1.0f}},
    /* Port 1003 */ {1, {+1.0f, -1.0f}, {+1.0f, +1.0f}},
    /* Port 1004 */ {0, {+1.0f, +1.0f}, {+1.0f, -1.0f}},
    /* Port 1005 */ {0, {+1.0f, -1.0f}, {+1.0f, -1.0f}},
    /* Port 1006 */ {1, {+1.0f, +1.0f}, {+1.0f, -1.0f}},
    /* Port 1007 */ {1, {+1.0f, -1.0f}, {+1.0f, -1.0f}},
}};

const std::array<dmrs_pdsch_processor_impl::params_t, DMRS_MAX_PORTS_TYPE2> dmrs_pdsch_processor_impl::params_type2 = {{
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

void srsgnb::dmrs_pdsch_processor_impl::sequence_generation(span<cf_t>      sequence,
                                                            unsigned int    symbol,
                                                            const config_t& config) const
{
  // Get signal amplitude
  float amplitude = M_SQRT1_2 * get_amplitude(config.nof_cdm_groups_without_data);

  unsigned prb_count            = 0; // Counts consecutive used PRB
  unsigned prb_skip             = 0; // Number of PRB to skip
  unsigned nof_pilots_x_prb     = dmrs_nof_dmrs_per_rb(config.type);
  unsigned reference_point_k_rb = config.reference_point_k_rb;

  unsigned nslot    = config.slot.get_frame_slot();
  unsigned nidnscid = config.scrambling_id;
  unsigned nscid    = config.n_scid ? 1 : 0;

  // Calculate initial sequence state
  unsigned c_init =
      ((((NSYMB_PER_SLOT_NORM * nslot + symbol + 1UL) * (2UL * nidnscid + 1UL)) << 17UL) + (2UL * nidnscid + nscid)) &
      INT32_MAX;

  // Initialise sequence
  prg->init(c_init);

  // Iterate over all PRBs, starting at reference point for k
  for (unsigned prb_idx = 0; prb_idx < MAX_RB; prb_idx++) {
    // If the PRB is used for PDSCH transmission count
    if (config.rb_mask[prb_idx]) {
      // If it is the first PRB...
      if (prb_count == 0) {
        // ... discard unused pilots and reset counter unless the PDSCH transmission carries SIB
        prb_skip = std::max(0, static_cast<int>(prb_skip) - static_cast<int>(reference_point_k_rb));
        prg->advance(prb_skip * nof_pilots_x_prb * 2);
        prb_skip             = 0;
        reference_point_k_rb = 0;
      }
      ++prb_count;

      continue;
    }

    // Increase number of PRB to skip
    ++prb_skip;

    // End of consecutive PRB, skip copying if no PRB was counted
    if (prb_count == 0) {
      continue;
    }

    // Generate contiguous pilots
    prg->generate(sequence.first(prb_count * nof_pilots_x_prb), amplitude);

    // Advance sequence buffer
    sequence = sequence.last(sequence.size() - prb_count * nof_pilots_x_prb);

    // Reset counter
    prb_count = 0;
  }

  if (prb_count > 0) {
    // Generate contiguous pilots
    prg->generate(sequence.first(prb_count * nof_pilots_x_prb), amplitude);

    // Advance sequence buffer
    sequence = sequence.last(sequence.size() - prb_count * nof_pilots_x_prb);
  }
}

void srsgnb::dmrs_pdsch_processor_impl::mapping(resource_grid_writer& grid,
                                                span<const cf_t>      sequence,
                                                span<const bool>      base_mask,
                                                unsigned              symbol,
                                                const config_t&       config)
{
  // Resource elements views for each port
  static_vector<span<const cf_t>, MAX_PORTS> re(config.ports.size());

  // Set l_prime to 1 if the symbol follows another one
  unsigned l_prime = 0;
  if (symbol != 0) {
    l_prime = (config.symbols_mask[symbol - 1]) ? 1 : 0;
  }

  // For each port compute the sequence
  for (unsigned port = 0; port != config.ports.size(); ++port) {
    // Get parameter for this port and symbol
    const params_t& params = (config.type == dmrs_type::TYPE1) ? params_type1[port] : params_type2[port];

    // If no precoding, neither weights are applied, use sequence reference
    if (config.pmi == 0 && params.w_t[l_prime] == +1.0f && params.w_f[0] == params.w_f[1]) {
      re[port] = sequence;
      continue;
    }

    // Setup temporal RE
    temp_re[port].resize(sequence.size());

    // Apply w_t weight can be +1 or -1 depending on l_prime and port
    if (params.w_t[l_prime] != +1.0f) {
      srsvec::sc_prod(sequence, -1, temp_re[port]);
    } else {
      srsvec::copy(span<cf_t>(temp_re[port]), sequence);
    }

    // Apply w_f weight. It can be {+1, +1} or {+1, -1} depending on l_prime and port. On the first case, no operation
    // is required. On the second case, re with odd sequence indexes are multiplied by -1.
    if (params.w_f[0] != params.w_f[1]) {
      for (unsigned i = 1; i < temp_re[port].size(); i += 2) {
        temp_re[port][i] *= -1;
      }
    }

    // Setup RE
    re[port] = temp_re[port];
  }

  // Apply precoding
  if (config.pmi != 0) {
    srsran_assertion_failure("Precoding is not currently supported\n");
  }

  // For each port put elements in grid
  for (unsigned port = 0; port != config.ports.size(); ++port) {
    // Get parameter for this port and symbol
    const params_t& params = (config.type == dmrs_type::TYPE1) ? params_type1[port] : params_type2[port];

    // Put port elements in the resource grid
    grid.put(config.ports[port], symbol, base_mask.subspan(NRE - params.delta, MAX_RB * NRE), re[port]);
  }
}

void srsgnb::dmrs_pdsch_processor_impl::map(resource_grid_writer& grid, const config_t& config)
{
  // Count number of RB
  unsigned rb_count = 0;
  for (unsigned rb = 0; rb < MAX_RB; ++rb) {
    rb_count += config.rb_mask[rb] ? 1 : 0;
  }
  unsigned dmrs_re_count = dmrs_nof_dmrs_per_rb(config.type) * rb_count;

  // Generate DMRS mask, with NRE padding at the first samples
  std::array<bool, (MAX_RB + 1)* NRE> base_mask = {};
  if (config.type == dmrs_type::TYPE1) {
    // Generate type 1: {1,0,1,0,1,0,1,0,1,0,1,0}
    for (unsigned rb = 0; rb < MAX_RB; rb++) {
      if (config.rb_mask[rb]) {
        for (unsigned k = rb * NRE; k < (rb + 1) * NRE; k += 2) {
          base_mask[NRE + k] = true;
        }
      }
    }
  } else {
    // Generate type 2: {1,1,0,0,0,0,1,1,0,0,0,0,}
    for (unsigned rb = 0; rb < MAX_RB; rb++) {
      if (config.rb_mask[rb]) {
        for (unsigned k = rb * NRE; k < (rb + 1) * NRE; k += 6) {
          base_mask[NRE + k]     = true;
          base_mask[NRE + k + 1] = true;
        }
      }
    }
  }

  // Resize temporal RE for the symbol according to the number of ports
  temp_re.resize(config.ports.size());

  // For each symbol in the slot....
  for (unsigned symbol = 0; symbol < NSYMB_PER_SLOT_NORM; ++symbol) {
    // Skip symbol if it is not used to transmit
    if (!config.symbols_mask[symbol]) {
      continue;
    }

    // Generate sequence for the given symbol
    static_vector<cf_t, MAX_DMRS_PER_SYMBOl> sequence(dmrs_re_count);
    sequence_generation(sequence, symbol, config);

    // Mapping to physical resources for the given symbol
    mapping(grid, sequence, base_mask, symbol, config);
  }
}

std::unique_ptr<dmrs_pdsch_processor> srsgnb::create_dmrs_pdsch_processor()
{
  return std::make_unique<dmrs_pdsch_processor_impl>();
}
