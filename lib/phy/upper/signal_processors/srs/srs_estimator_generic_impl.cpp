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

#include "srs_estimator_generic_impl.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/static_vector.h"
#include "srsran/adt/tensor.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_configuration.h"
#include "srsran/phy/upper/signal_processors/srs/srs_estimator_result.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/srs/srs_constants.h"
#include "srsran/ran/srs/srs_information.h"
#include "srsran/srsvec/add.h"
#include "srsran/srsvec/mean.h"
#include "srsran/srsvec/prod.h"

using namespace srsran;

void srs_estimator_generic_impl::compensate_phase_shift(span<cf_t> mean_lse,
                                                        float      phase_shift_subcarrier,
                                                        float      phase_shift_offset)
{
  unsigned sequence_length = mean_lse.size();

  // Generate phase indices.
  span<unsigned> phase_indices = span<unsigned>(temp_phase).first(sequence_length);
  std::generate(
      phase_indices.begin(), phase_indices.end(), [phase_shift_subcarrier, phase_shift_offset, n = 0]() mutable {
        return static_cast<int>(std::round(static_cast<float>(cexp_table_size) *
                                           (static_cast<float>(n++) * phase_shift_subcarrier + phase_shift_offset) /
                                           TWOPI));
      });

  // Generate complex exponential.
  span<cf_t> cexp = span<cf_t>(temp_cexp).first(sequence_length);
  cexp_table.generate(cexp, phase_indices);

  // Compensate phase shift.
  srsvec::prod(mean_lse, cexp, mean_lse);
}

srs_estimator_result srs_estimator_generic_impl::estimate(const resource_grid_reader&        grid,
                                                          const srs_estimator_configuration& config)
{
  srsran_assert(!config.resource.has_frequency_hopping(), "Frequency hopping is not supported.");
  srsran_assert(config.resource.is_valid(), "Invalid SRS resource.");
  srsran_assert(!config.ports.empty(), "Receive port list is empty.");

  unsigned nof_rx_ports         = config.ports.size();
  unsigned nof_antenna_ports    = static_cast<unsigned>(config.resource.nof_antenna_ports);
  unsigned nof_symbols          = static_cast<unsigned>(config.resource.nof_symbols);
  unsigned nof_symbols_per_slot = get_nsymb_per_slot(cyclic_prefix::NORMAL);
  srsran_assert(config.resource.start_symbol.value() + nof_symbols <= nof_symbols_per_slot,
                "The start symbol index (i.e., {}) plus the number of symbols (i.e., {}) exceeds the number of symbols "
                "per slot (i.e., {})",
                config.resource.start_symbol,
                nof_symbols,
                nof_symbols_per_slot);

  // Extract subcarrier spacing.
  subcarrier_spacing scs = to_subcarrier_spacing(config.slot.numerology());

  // Extract comb size.
  unsigned comb_size = static_cast<unsigned>(config.resource.comb_size);

  srs_information common_info = get_srs_information(config.resource, 0);

  // Sequence length is common for all ports and symbols.
  unsigned sequence_length = common_info.sequence_length;

  // Maximum measurable delay due to cyclic shift.
  double max_ta = 1.0 / static_cast<double>(common_info.n_cs_max * scs_to_khz(scs) * 1000 * comb_size);

  // Prepare results.
  srs_estimator_result result;
  result.time_alignment.time_alignment = 0;
  result.time_alignment.resolution     = 0;
  result.time_alignment.min            = std::numeric_limits<double>::min();
  result.time_alignment.max            = std::numeric_limits<double>::max();
  result.channel_matrix                = srs_channel_matrix(nof_rx_ports, nof_antenna_ports);

  // Temporary LSE.
  static_tensor<3, cf_t, max_seq_length * srs_constants::max_nof_rx_ports * srs_constants::max_nof_tx_ports> temp_lse(
      {sequence_length, nof_rx_ports, nof_antenna_ports});

  // Iterate transmit ports.
  for (unsigned i_antenna_port = 0; i_antenna_port != nof_antenna_ports; ++i_antenna_port) {
    // Obtain SRS information for a given SRS antenna port.
    srs_information info = get_srs_information(config.resource, i_antenna_port);

    // Generate sequence.
    static_vector<cf_t, max_seq_length> sequence(info.sequence_length);
    deps.sequence_generator->generate(sequence, info.sequence_group, info.sequence_number, info.n_cs, info.n_cs_max);

    // Iterate receive ports.
    for (unsigned i_rx_port_index = 0; i_rx_port_index != nof_rx_ports; ++i_rx_port_index) {
      unsigned i_rx_port = config.ports[i_rx_port_index];

      // View to the mean LSE for a port combination.
      span<cf_t> mean_lse = temp_lse.get_view({i_rx_port_index, i_antenna_port});

      // Extract sequence for all symbols and average LSE.
      for (unsigned i_symbol     = config.resource.start_symbol.value(),
                    i_symbol_end = config.resource.start_symbol.value() + nof_symbols;
           i_symbol != i_symbol_end;
           ++i_symbol) {
        // Extract received sequence.
        static_vector<cf_t, max_seq_length> rx_sequence(info.sequence_length);
        grid.get(rx_sequence, i_rx_port, i_symbol, info.mapping_initial_subcarrier, info.comb_size);

        // Temporary LSE.
        span<cf_t> lse = rx_sequence;

        // Avoid accumulation for the first symbol containing SRS.
        if (i_symbol == config.resource.start_symbol.value()) {
          lse = mean_lse;
        }

        // Calculate LSE.
        srsvec::prod_conj(rx_sequence, sequence, lse);

        // Accumulate LSE for the averaging.
        if (lse.data() != mean_lse.data()) {
          srsvec::add(mean_lse, lse, mean_lse);
        }
      }

      // Scale accumulated LSE.
      if (nof_symbols > 1) {
        srsvec::sc_prod(mean_lse, 1.0 / static_cast<float>(nof_symbols), mean_lse);
      }

      // Estimate TA.
      time_alignment_measurement ta_meas = deps.ta_estimator->estimate(mean_lse, info.comb_size, scs, max_ta);

      // Combine time alignment measurement.
      result.time_alignment.time_alignment += ta_meas.time_alignment;
      result.time_alignment.min        = std::max(result.time_alignment.min, ta_meas.min);
      result.time_alignment.max        = std::min(result.time_alignment.max, ta_meas.max);
      result.time_alignment.resolution = std::max(result.time_alignment.resolution, ta_meas.resolution);
    }
  }

  // Average time alignment across all paths.
  result.time_alignment.time_alignment /= nof_antenna_ports * nof_rx_ports;

  // Compensate time alignment and estimate channel coefficients.
  for (unsigned i_antenna_port = 0; i_antenna_port != nof_antenna_ports; ++i_antenna_port) {
    for (unsigned i_rx_port = 0; i_rx_port != nof_rx_ports; ++i_rx_port) {
      // View to the mean LSE for a port combination.
      span<cf_t> mean_lse = temp_lse.get_view({i_rx_port, i_antenna_port});

      // Get sequence information.
      srs_information info = get_srs_information(config.resource, i_antenna_port);

      // Calculate subcarrier phase shift in radians.
      float phase_shift_subcarrier =
          static_cast<float>(TWOPI * result.time_alignment.time_alignment * scs_to_khz(scs) * 1000 * comb_size);

      // Calculate the initial phase shift in radians.
      float phase_shift_offset = phase_shift_subcarrier * (info.mapping_initial_subcarrier % comb_size) / comb_size;

      // Compensate phase shift.
      compensate_phase_shift(mean_lse, phase_shift_subcarrier, phase_shift_offset);

      // Calculate channel wideband coefficient.
      cf_t coefficient = srsvec::mean(mean_lse);
      result.channel_matrix.set_coefficient(coefficient, i_rx_port, i_antenna_port);
    }
  }

  return result;
}
