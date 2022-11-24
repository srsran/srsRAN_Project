/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdsch_modulator_impl.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

void pdsch_modulator_impl::scramble(span<uint8_t> b_hat, span<const uint8_t> b, unsigned q, const config_t& config)
{
  // Calculate initial scrambling state.
  unsigned c_init = (static_cast<unsigned>(config.rnti) << 15U) + (q << 14U) + config.n_id;

  // Initialize scrambling sequence.
  scrambler->init(c_init);

  // Apply scrambling sequence.
  scrambler->apply_xor_bit(b_hat, b);
}

void pdsch_modulator_impl::modulate(span<cf_t>          d_pdsch,
                                    span<const uint8_t> b_hat,
                                    modulation_scheme   modulation,
                                    float               scaling)
{
  // Modulate.
  modulator->modulate(b_hat, d_pdsch, modulation);

  // Apply scaling only if the value is valid.
  if (std::isnormal(scaling)) {
    srsvec::sc_prod(d_pdsch, scaling, d_pdsch);
  }
}

void pdsch_modulator_impl::layer_map(static_vector<span<cf_t>, MAX_PORTS>&        x_pdsch,
                                     static_vector<span<cf_t>, MAX_NOF_CODEWORDS> d_pdsch)
{
  // Deduce number of codeword from input parameter.
  unsigned nof_codewords = d_pdsch.size();

  // Deduce number of layer from input parameter.
  unsigned nof_layers = x_pdsch.size();

  // Calculate the number of layers the codeword 0 is mapped to. It is the number of layers divided by the number of
  // codewords, rounding down (floor).
  unsigned nof_layers_cw0 = nof_layers / nof_codewords;

  // Calculate the number of layers the codeword 1 is mapped. It is the unused number of layers from the previous
  // codeword.
  unsigned nof_layers_cw1 = nof_layers - nof_layers_cw0;
  unsigned M_layer_symb   = d_pdsch.size() * d_pdsch[0].size();

  // If only one layer, transfer reference.
  if (nof_layers == 1) {
    x_pdsch[0] = d_pdsch[0];
    return;
  }

  // Set up each layer reference.
  for (unsigned layer = 0; layer != nof_layers_cw0; ++layer) {
    x_pdsch[layer] = span<cf_t>(temp_x[layer].data(), M_layer_symb);
  }

  // Apply TS 38.211 Table 7.3.1.3-1: Codeword-to-layer mapping for spatial multiplexing.
  for (unsigned i = 0; i != M_layer_symb; ++i) {
    // Apply layer mapping for codeword 0.
    for (unsigned layer = 0; layer != nof_layers_cw0; ++layer) {
      x_pdsch[layer][i] = d_pdsch[0][nof_layers_cw0 * i + layer];
    }

    // Apply layer mapping for codeword 1.
    for (unsigned layer = 0; layer != nof_layers_cw1; ++layer) {
      x_pdsch[layer + nof_layers_cw0][i] = d_pdsch[1][nof_layers_cw1 * i + layer];
    }
  }
}

void pdsch_modulator_impl::map_to_contiguous_prb(resource_grid_writer&                grid,
                                                 static_vector<span<cf_t>, MAX_PORTS> x_pdsch,
                                                 const config_t&                      config)
{
  // Stores the resource grid allocation mask, common for all ports.
  std::array<bounded_bitset<MAX_RB * NRE>, MAX_NSYMB_PER_SLOT> allocation_mask;

  // Get the PRB allocation mask.
  const bounded_bitset<MAX_RB> prb_allocation_mask =
      config.freq_allocation.get_prb_mask(config.bwp_start_rb, config.bwp_size_rb);
  const bounded_bitset<MAX_RB* NRE> re_allocation_mask =
      prb_allocation_mask.kronecker_product<NRE>(~bounded_bitset<NRE>(NRE));

  // First symbol used in this transmission.
  unsigned start_symbol_index = config.start_symbol_index;

  // Calculate the end symbol index (excluded) and assert it does not exceed the slot boundary.
  unsigned end_symbol_index = config.start_symbol_index + config.nof_symbols;
  srsgnb_assert(end_symbol_index <= MAX_NSYMB_PER_SLOT,
                "The time allocation of the transmission (%d:%d) exceeds the slot boundary.",
                start_symbol_index,
                end_symbol_index);

  // Get DMRS RE pattern.
  re_pattern dmrs_pattern = config.dmrs_config_type.get_dmrs_pattern(
      config.bwp_start_rb, config.bwp_size_rb, config.nof_cdm_groups_without_data, config.dmrs_symb_pos);

  // Generate frequency allocation mask for each symbol in the transmission.
  for (unsigned symbol_idx = start_symbol_index; symbol_idx != end_symbol_index; ++symbol_idx) {
    // Create mask, skip initialization.
    bounded_bitset<NRE* MAX_RB>& symbol_mask = allocation_mask[symbol_idx];

    // Copy base RE allocation mask.
    symbol_mask = re_allocation_mask;

    // Exclude DM-RS.
    dmrs_pattern.get_exclusion_mask(symbol_mask, symbol_idx);

    // Exclude reserved RE.
    config.reserved.get_exclusion_mask(symbol_mask, symbol_idx);
  }

  // Extract number of layers.
  unsigned nof_layers = x_pdsch.size();

  // Iterate for each layer.
  for (unsigned layer_idx = 0; layer_idx != nof_layers; ++layer_idx) {
    // Make buffer with the RE to map.
    span<const cf_t> x_buffer = x_pdsch[layer_idx];

    // Get port from the layer index.
    unsigned port_idx = config.ports[layer_idx];

    // Iterate for each symbol.
    for (unsigned symbol_idx = start_symbol_index; symbol_idx != end_symbol_index; ++symbol_idx) {
      // Select mask for the OFDM symbol.
      const bounded_bitset<MAX_RB* NRE>& mask = allocation_mask[symbol_idx];

      // Write RE in resource grid.
      x_buffer = grid.put(port_idx, symbol_idx, 0, mask, x_buffer);
    }

    // Verify all the resource elements for the layer have been mapped.
    srsgnb_assert(x_buffer.empty(), "{} elements are not mapped in layer {}.", x_buffer.size(), layer_idx);
  }
}

void pdsch_modulator_impl::map_to_prb_other(resource_grid_writer&                grid,
                                            static_vector<span<cf_t>, MAX_PORTS> x_pdsch,
                                            const config_t&                      config)
{
  // Get PRB mapping indices including VRB-to-PRB mapping.
  static_vector<uint16_t, MAX_RB> prb_indices =
      config.freq_allocation.get_prb_indices(config.bwp_start_rb, config.bwp_size_rb);

  // Stores the resource grid allocation mask, common for all ports.
  std::array<bounded_bitset<NRE * MAX_RB>, MAX_NSYMB_PER_SLOT> allocation_mask = {};

  // First symbol used in this transmission.
  unsigned start_symbol_index = config.start_symbol_index;

  // Calculate the end symbol index (excluded) and assert it does not exceed the slot boundary.
  unsigned end_symbol_index = config.start_symbol_index + config.nof_symbols;
  srsgnb_assert(end_symbol_index <= MAX_NSYMB_PER_SLOT,
                "The time allocation of the transmission (%d:%d) exceeds the slot boundary.",
                start_symbol_index,
                end_symbol_index);

  // Get DMRS RE pattern.
  re_pattern dmrs_pattern = config.dmrs_config_type.get_dmrs_pattern(
      config.bwp_start_rb, config.bwp_size_rb, config.nof_cdm_groups_without_data, config.dmrs_symb_pos);

  // Generate frequency allocation mask for each symbol in the transmission.
  for (unsigned symbol_idx = start_symbol_index; symbol_idx != end_symbol_index; ++symbol_idx) {
    // Create mask, skip initialization.
    bounded_bitset<NRE* MAX_RB>& symbol_mask = allocation_mask[symbol_idx];

    // Fill symbol mask.
    symbol_mask = config.freq_allocation.get_prb_mask(config.bwp_start_rb, config.bwp_size_rb)
                      .kronecker_product<NRE>(~bounded_bitset<NRE>(NRE));

    // Exclude DM-RS.
    dmrs_pattern.get_exclusion_mask(symbol_mask, symbol_idx);

    // Exclude reserved RE.
    config.reserved.get_exclusion_mask(symbol_mask, symbol_idx);
  }

  // Extract number of layers.
  unsigned nof_layers = x_pdsch.size();

  // Map the PDSCH REs for all the layers.
  for (unsigned layer_idx = 0; layer_idx != nof_layers; ++layer_idx) {
    // Make buffer with the RE to map.
    span<const cf_t> x_buffer = x_pdsch[layer_idx];

    // Get port from the layer index.
    unsigned port_idx = config.ports[layer_idx];

    // Map the REs for each symbol in the layer.
    for (unsigned symbol_idx = start_symbol_index; symbol_idx != end_symbol_index; ++symbol_idx) {
      // Get whole symbol allocation mask.
      const bounded_bitset<NRE* MAX_RB>& rb_mask_symbol = allocation_mask[symbol_idx];

      // Mapping to VRB and, from there, to PRB according to TS38.211 Sections 7.3.1.5 and 7.3.1.6.
      for (unsigned prb_idx : prb_indices) {
        // Select RB mask for the PRB.
        const bounded_bitset<NRE* MAX_RB> rb_mask = rb_mask_symbol.slice(prb_idx * NRE, (prb_idx + 1) * NRE);

        // Write PRB in resource grid.
        x_buffer = grid.put(port_idx, symbol_idx, prb_idx * NRE, rb_mask, x_buffer);
      }
    }
  }
}

void pdsch_modulator_impl::modulate(srsgnb::resource_grid_writer&                   grid,
                                    srsgnb::span<const srsgnb::span<const uint8_t>> codewords,
                                    const srsgnb::pdsch_modulator::config_t&        config)
{
  // Deduce the number of layers from the number of ports
  unsigned nof_layers = config.ports.size();
  srsgnb_assert(nof_layers > 0, "Number of layers is zero.");

  // Deduce the number of codewords and assert it is coherent with the number of layers.
  unsigned nof_codewords = (nof_layers >= 4 ? 2 : 1);
  srsgnb_assert(
      codewords.size() == nof_codewords, "For %d layers, %d codewords are required", nof_layers, codewords.size());

  // Process codewords.
  static_vector<span<cf_t>, MAX_NOF_CODEWORDS> d_pdsch(nof_codewords);
  for (unsigned cw_idx = 0; cw_idx != nof_codewords; ++cw_idx) {
    // Select modulation and order.
    modulation_scheme mod = (cw_idx == 0) ? config.modulation1 : config.modulation2;
    unsigned          Qm  = get_bits_per_symbol(mod);

    // Calculate number of REs.
    unsigned nof_bits = codewords[cw_idx].size();
    unsigned nof_re   = nof_bits / Qm;

    // Create temporal storage for scrambled bits.
    span<uint8_t> b_hat = span<uint8_t>(temp_b_hat).first(nof_bits);

    // Scramble.
    scramble(b_hat, codewords[cw_idx], cw_idx, config);

    // Prepare destination buffer view.
    d_pdsch[cw_idx] = span<cf_t>(temp_d[cw_idx].data(), nof_re);

    // Modulate codeword.
    modulate(d_pdsch[cw_idx], b_hat, mod, config.scaling);
  }

  // Perform layer mapping.
  static_vector<span<cf_t>, MAX_PORTS> x_pdsch(nof_layers);
  layer_map(x_pdsch, d_pdsch);

  // Map resource elements.
  if (config.freq_allocation.is_contiguous()) {
    map_to_contiguous_prb(grid, x_pdsch, config);
  } else {
    map_to_prb_other(grid, x_pdsch, config);
  }
}
