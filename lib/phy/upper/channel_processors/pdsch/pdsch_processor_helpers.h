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

/// \file
/// \brief Helper functions for PDSCH processor implementations.

#pragma once

#include "srsran/instrumentation/traces/du_traces.h"
#include "srsran/phy/upper/channel_processors/pdsch/pdsch_processor.h"
#include "srsran/phy/upper/signal_processors/pdsch/dmrs_pdsch_processor.h"
#include "srsran/phy/upper/signal_processors/ptrs/ptrs_pdsch_generator.h"
#include "srsran/ran/dmrs.h"
#include "srsran/ran/ptrs/ptrs_pattern.h"
#include "srsran/support/tracing/event_tracing.h"

namespace srsran {

/// \brief Generates and maps DM-RS for the PDSCH transmission as per TS38.211 Section 7.4.1.1.
/// \param[out] grid Resource grid writer interface.
/// \param[out] dmrs DM-RS PDSCH processor interface.
/// \param[in]  pdu  Provides the PDSCH processor PDU.
inline void
pdsch_process_dmrs(resource_grid_writer& grid, dmrs_pdsch_processor& dmrs, const pdsch_processor::pdu_t& pdu)
{
  trace_point process_dmrs_tp = l1_dl_tracer.now();

  crb_bitmap rb_mask_bitset = pdu.freq_alloc.get_crb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Select the DM-RS reference point.
  unsigned dmrs_reference_point_k_rb = 0;
  if (pdu.ref_point == pdsch_processor::pdu_t::PRB0) {
    dmrs_reference_point_k_rb = pdu.bwp_start_rb;
  }

  // Prepare DM-RS configuration.
  dmrs_pdsch_processor::config_t dmrs_config;
  dmrs_config.slot                 = pdu.slot;
  dmrs_config.reference_point_k_rb = dmrs_reference_point_k_rb;
  dmrs_config.type                 = pdu.dmrs;
  dmrs_config.scrambling_id        = pdu.scrambling_id;
  dmrs_config.n_scid               = pdu.n_scid;
  dmrs_config.amplitude            = convert_dB_to_amplitude(-pdu.ratio_pdsch_dmrs_to_sss_dB);
  dmrs_config.symbols_mask         = pdu.dmrs_symbol_mask;
  dmrs_config.rb_mask              = rb_mask_bitset;
  dmrs_config.precoding            = pdu.precoding;

  // Put DM-RS.
  dmrs.map(grid, dmrs_config);

  l1_dl_tracer << trace_event("process_dmrs", process_dmrs_tp);
}

/// \brief Generates and maps PT-RS for the PDSCH transmission as per TS38.211 Section 7.4.1.2.
/// \param[out] grid           Resource grid writer interface.
/// \param[out] ptrs_generator PT-RS PDSCH generator interface.
/// \param[in]  pdu            Provides the PDSCH processor PDU.
inline void
pdsch_process_ptrs(resource_grid_writer& grid, ptrs_pdsch_generator& ptrs_generator, const pdsch_processor::pdu_t& pdu)
{
  trace_point process_ptrs_tp = l1_dl_tracer.now();

  // Extract PT-RS configuration parameters.
  const pdsch_processor::ptrs_configuration& ptrs = *pdu.ptrs;

  crb_bitmap rb_mask_bitset = pdu.freq_alloc.get_crb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Select the DM-RS reference point.
  unsigned ptrs_reference_point_k_rb = 0;
  if (pdu.ref_point == pdsch_processor::pdu_t::PRB0) {
    ptrs_reference_point_k_rb = pdu.bwp_start_rb;
  }

  // Calculate the PT-RS sequence amplitude following TS38.214 Section 4.1.
  float amplitude = convert_dB_to_amplitude(ptrs.ratio_ptrs_to_pdsch_data_dB - pdu.ratio_pdsch_data_to_sss_dB);

  // Prepare PT-RS configuration.
  ptrs_pdsch_generator::configuration ptrs_config;
  ptrs_config.slot                 = pdu.slot;
  ptrs_config.rnti                 = to_rnti(pdu.rnti);
  ptrs_config.dmrs_config_type     = pdu.dmrs;
  ptrs_config.reference_point_k_rb = ptrs_reference_point_k_rb;
  ptrs_config.scrambling_id        = pdu.scrambling_id;
  ptrs_config.n_scid               = pdu.n_scid;
  ptrs_config.amplitude            = amplitude;
  ptrs_config.dmrs_symbols_mask    = pdu.dmrs_symbol_mask;
  ptrs_config.rb_mask              = rb_mask_bitset;
  ptrs_config.time_allocation      = {pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols};
  ptrs_config.freq_density         = ptrs.freq_density;
  ptrs_config.time_density         = ptrs.time_density;
  ptrs_config.re_offset            = ptrs.re_offset;
  ptrs_config.reserved             = pdu.reserved;
  ptrs_config.precoding            = pdu.precoding;

  // Put PT-RS.
  ptrs_generator.generate(grid, ptrs_config);

  l1_dl_tracer << trace_event("process_ptrs", process_ptrs_tp);
}

/// \brief Computes the number of RE used for mapping PDSCH data.
///
/// The number of RE excludes the elements described by \c pdu as reserved and the RE used for DM-RS.
///
/// \param[in] pdu Describes a PDSCH transmission.
/// \return The number of resource elements.
inline unsigned pdsch_compute_nof_data_re(const pdsch_processor::pdu_t& pdu)
{
  // Get PRB mask.
  crb_bitmap crb_mask = pdu.freq_alloc.get_crb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Get number of RB.
  unsigned nof_prb = crb_mask.count();

  // Calculate the number of RE allocated in the grid.
  unsigned nof_grid_re = nof_prb * NRE * pdu.nof_symbols;

  // Generate DM-RS pattern.
  re_pattern dmrs_pattern = pdu.dmrs.get_dmrs_pattern(
      pdu.bwp_start_rb, pdu.bwp_size_rb, pdu.nof_cdm_groups_without_data, pdu.dmrs_symbol_mask);

  // Calculate the number of RE used by DM-RS. It assumes it does not overlap with reserved elements.
  unsigned nof_grid_dmrs = nof_prb * dmrs_pattern.re_mask.count() * dmrs_pattern.symbols.count();

  // Generate reserved pattern.
  re_pattern_list reserved = pdu.reserved;

  // If the pattern contains PT-RS, append the reserved elements to the list.
  if (pdu.ptrs.has_value()) {
    // Extract specific PT-RS configuration.
    const pdsch_processor::ptrs_configuration& ptrs_config = *pdu.ptrs;

    // Create PT-RS pattern configuration.
    ptrs_pattern_configuration ptrs_pattern_config = {
        .rnti             = to_rnti(pdu.rnti),
        .dmrs_type        = (pdu.dmrs == dmrs_type::TYPE1) ? dmrs_config_type::type1 : dmrs_config_type::type2,
        .dmrs_symbol_mask = pdu.dmrs_symbol_mask,
        .rb_mask          = crb_mask,
        .time_allocation  = {pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols},
        .freq_density     = ptrs_config.freq_density,
        .time_density     = ptrs_config.time_density,
        .re_offset        = ptrs_config.re_offset,
        .nof_ports        = 1};

    // Calculate PT-RS pattern and convert it to an RE pattern.
    ptrs_pattern ptrs_reserved_pattern = get_ptrs_pattern(ptrs_pattern_config);

    re_pattern ptrs_reserved_re_pattern;
    for (unsigned i_prb = ptrs_reserved_pattern.rb_begin; i_prb < ptrs_reserved_pattern.rb_end;
         i_prb += ptrs_reserved_pattern.rb_stride) {
      ptrs_reserved_re_pattern.crb_mask.set(i_prb);
    }
    ptrs_reserved_re_pattern.symbols = ptrs_reserved_pattern.symbol_mask;
    ptrs_reserved_re_pattern.re_mask.set(ptrs_reserved_pattern.re_offset.front());

    reserved.merge(ptrs_reserved_re_pattern);
  }

  // Calculate the number of reserved resource elements.
  unsigned nof_reserved_re = reserved.get_inclusion_count(pdu.start_symbol_index, pdu.nof_symbols, crb_mask);

  // Subtract the number of reserved RE from the number of allocated RE.
  srsran_assert(nof_grid_re > nof_reserved_re,
                "The number of reserved RE ({}) exceeds the number of RE allocated in the transmission ({})",
                nof_grid_re,
                nof_reserved_re);
  return nof_grid_re - nof_reserved_re - nof_grid_dmrs;
}

} // namespace srsran
