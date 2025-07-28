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

#include "dmrs_pusch_estimator_impl.h"
#include "dmrs_helper.h"
#include "srsran/srsvec/copy.h"
#include "srsran/srsvec/sc_prod.h"

using namespace srsran;

void dmrs_pusch_estimator_impl::estimate(channel_estimate&           estimate,
                                         const resource_grid_reader& grid,
                                         const configuration&        config)
{
  dmrs_type type          = config.get_dmrs_type();
  unsigned  nof_tx_layers = config.get_nof_tx_layers();
  unsigned  nof_rx_ports  = config.rx_ports.size();

  // Select the DM-RS pattern for this PUSCH transmission.
  span<layer_dmrs_pattern> coordinates = span<layer_dmrs_pattern>(temp_pattern).first(nof_tx_layers);

  // Prepare DM-RS symbol buffer dimensions.
  re_measurement_dimensions dims;
  dims.nof_subc    = config.rb_mask.count() * type.nof_dmrs_per_rb();
  dims.nof_symbols = config.symbols_mask.count();
  dims.nof_slices  = nof_tx_layers;

  // Resize DM-RS symbol buffer.
  temp_symbols.resize(dims);

  // Resize channel estimate.
  estimate.resize({static_cast<unsigned>(config.rb_mask.size()),
                   config.first_symbol + config.nof_symbols,
                   nof_rx_ports,
                   nof_tx_layers});

  // Generate symbols and allocation patterns.
  generate(temp_symbols, coordinates, config);

  port_channel_estimator::configuration est_cfg;
  est_cfg.dmrs_pattern.assign(coordinates.begin(), coordinates.end());
  est_cfg.scs          = to_subcarrier_spacing(config.slot.numerology());
  est_cfg.first_symbol = config.first_symbol;
  est_cfg.nof_symbols  = config.nof_symbols;
  est_cfg.rx_ports     = config.rx_ports;
  est_cfg.scaling      = config.scaling;

  for (unsigned i_port = 0; i_port != nof_rx_ports; ++i_port) {
    ch_estimator->compute(estimate, grid, i_port, temp_symbols, est_cfg);
  }
}

void dmrs_pusch_estimator_impl::sequence_generation(span<cf_t>           sequence,
                                                    unsigned             symbol,
                                                    const configuration& config) const
{
  // Generate low-PAPR sequence.
  if (std::holds_alternative<low_papr_sequence_configuration>(config.sequence_config)) {
    const auto& sequence_config = std::get<low_papr_sequence_configuration>(config.sequence_config);
    unsigned    sequence_group  = sequence_config.n_rs_id % 30;
    low_paper_sequence_gen->generate(sequence, sequence_group, 0, 0, 1);
    return;
  }

  // Generate pseudo-random sequence.
  const auto& sequence_config = std::get<pseudo_random_sequence_configuration>(config.sequence_config);

  // Get signal amplitude.
  float amplitude = M_SQRT1_2;

  // Extract parameters to calculate the PRG initial state.
  unsigned nslot    = config.slot.slot_index();
  unsigned nidnscid = sequence_config.scrambling_id;
  unsigned nscid    = sequence_config.n_scid ? 1 : 0;
  unsigned nsymb    = get_nsymb_per_slot(cyclic_prefix::NORMAL);

  // Calculate initial sequence state.
  unsigned c_init = ((nsymb * nslot + symbol + 1) * (2 * nidnscid + 1) * pow2(17) + (2 * nidnscid + nscid)) % pow2(31);

  // Initialize sequence.
  prg->init(c_init);

  // Generate sequence.
  dmrs_sequence_generate(
      sequence, *prg, amplitude, DMRS_REF_POINT_K_TO_POINT_A, sequence_config.type.nof_dmrs_per_rb(), config.rb_mask);
}

void dmrs_pusch_estimator_impl::generate(dmrs_symbol_list&        symbols,
                                         span<layer_dmrs_pattern> mask,
                                         const configuration&     cfg)
{
  dmrs_type type          = cfg.get_dmrs_type();
  unsigned  nof_tx_layers = cfg.get_nof_tx_layers();

  // For each symbol in the transmission generate DMRS for layer 0.
  for (unsigned ofdm_symbol_index = cfg.first_symbol,
                ofdm_symbol_end   = cfg.first_symbol + cfg.nof_symbols,
                dmrs_symbol_index = 0;
       ofdm_symbol_index != ofdm_symbol_end;
       ++ofdm_symbol_index) {
    // Skip symbol if it does not carry DM-RS.
    if (!cfg.symbols_mask.test(ofdm_symbol_index)) {
      continue;
    }

    // Select a view to the DM-RS symbols for this OFDM symbol and layer 0.
    span<cf_t> dmrs_symbols = symbols.get_symbol(dmrs_symbol_index, 0);

    // Generate DM-RS for PUSCH.
    sequence_generation(dmrs_symbols, ofdm_symbol_index, cfg);

    // Increment DM-RS OFDM symbol index.
    ++dmrs_symbol_index;
  }

  // For each layer...
  for (unsigned i_layer = 0; i_layer != nof_tx_layers; ++i_layer) {
    // Select layer parameters.
    dmrs_pxsch_parameters params = get_pxsch_dmrs_params(type, i_layer);

    // Skip copy for layer 0.
    if (i_layer != 0) {
      // For each symbol containing DM-RS...
      for (unsigned i_symbol = 0, i_symbol_end = symbols.size().nof_symbols; i_symbol != i_symbol_end; ++i_symbol) {
        // Get a view of the symbols for the current layer.
        span<cf_t> dmrs = symbols.get_symbol(i_symbol, i_layer);

        // Get a view of the symbols for layer 0.
        span<const cf_t> dmrs_layer0 = symbols.get_symbol(i_symbol, 0);

        // If a time weight is required...
        if ((params.w_t[0] != params.w_t[1]) && (i_symbol % 2 == 1)) {
          // apply the weight...
          srsvec::sc_prod(dmrs, dmrs_layer0, params.w_t[1]);
        } else {
          // otherwise, copy symbols from layer 0 to the current layer.
          srsvec::copy(dmrs, dmrs_layer0);
        }

        // If a frequency weight is required...
        if (params.w_f[0] != params.w_f[1]) {
          // apply frequency domain mask.
          for (unsigned subc = 1, subc_end = 2 * (dmrs.size() / 2) + 1; subc != subc_end; subc += 2) {
            dmrs[subc] *= params.w_f[1];
          }
        }
      }
    }

    mask[i_layer].symbols    = cfg.symbols_mask;
    mask[i_layer].rb_mask    = cfg.rb_mask;
    mask[i_layer].re_pattern = params.re_pattern;
  }
}
