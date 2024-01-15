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

#include "ofh_uplink_request_handler_impl.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/prach/prach_frequency_mapping.h"
#include "srsran/ran/prach/prach_preamble_information.h"
#include "srsran/ran/resource_block.h"

using namespace srsran;
using namespace ofh;

/// Determines and returns Open Fronthaul filter index type given the PRACH preamble info and associated context.
static filter_index_type get_prach_cplane_filter_index(const prach_buffer_context&       context,
                                                       const prach_preamble_information& preamble_info)
{
  if (context.format == prach_format_type::three) {
    return filter_index_type::ul_prach_preamble_5kHz;
  }

  if (is_long_preamble(context.format)) {
    return filter_index_type::ul_prach_preamble_1p25khz;
  }

  if (context.format == prach_format_type::C0 || context.format == prach_format_type::C2) {
    return (preamble_info.scs == prach_subcarrier_spacing::kHz15) ? filter_index_type::ul_prach_preamble_short_15kHz
                                                                  : filter_index_type::ul_prach_preamble_short_30kHz;
  }
  return filter_index_type::ul_prach_preamble_short;
}

uplink_request_handler_impl::uplink_request_handler_impl(const uplink_request_handler_impl_config&  config,
                                                         uplink_request_handler_impl_dependencies&& dependencies) :
  logger(*dependencies.logger),
  is_prach_cp_enabled(config.is_prach_cp_enabled),
  cp(config.cp),
  tdd_config(config.tdd_config),
  prach_eaxc(config.prach_eaxc),
  ul_eaxc(config.ul_data_eaxc),
  ul_slot_repo_ptr(dependencies.ul_slot_repo),
  ul_prach_repo_ptr(dependencies.ul_prach_repo),
  ul_slot_repo(*ul_slot_repo_ptr),
  ul_prach_repo(*ul_prach_repo_ptr),
  data_flow(std::move(dependencies.data_flow))
{
  srsran_assert(ul_slot_repo_ptr, "Invalid uplink repository");
  srsran_assert(ul_prach_repo_ptr, "Invalid PRACH repository");
  srsran_assert(data_flow, "Invalid data flow");
}

void uplink_request_handler_impl::handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer)
{
  logger.debug("Registering PRACH context entry for slot '{}' and sector#{}", context.slot, context.sector);

  // Sampling rate defining the \f$T_s = 1/(\Delta f_{ref} \times N_{f,ref})\f$ parameter, see 3GPP TS38.211,
  // clause 4.1.
  // Open Fronthaul parameters timeOffset and cpLength are expressed in multiple of \f$T_s\f$ units.
  static constexpr double ref_srate_Hz = 30.72e6;

  // Get preamble information (assuming only single occasion is supported).
  prach_preamble_information preamble_info =
      is_long_preamble(context.format)
          ? get_prach_preamble_long_info(context.format)
          : get_prach_preamble_short_info(context.format, to_ra_subcarrier_spacing(context.pusch_scs), true);

  unsigned nof_prach_ports = std::min(size_t(buffer.get_max_nof_ports()), prach_eaxc.size());

  // Store the context in the repository, use correct slot index for long format accounting for PRACH duration.
  if (is_short_preamble(context.format)) {
    ul_prach_repo.add(context, buffer, nof_prach_ports);
  } else {
    static constexpr unsigned nof_symbols_per_slot = get_nsymb_per_slot(cyclic_prefix::NORMAL);

    double symbol_duration_msec =
        static_cast<double>(SUBFRAME_DURATION_MSEC) /
        static_cast<double>(get_nof_slots_per_subframe(context.pusch_scs) * nof_symbols_per_slot);

    double   len_msecs   = (preamble_info.cp_length.to_seconds() + preamble_info.symbol_length.to_seconds()) * 1000;
    unsigned nof_symbols = ceil(len_msecs / symbol_duration_msec);

    unsigned prach_length_slots = static_cast<unsigned>(std::ceil(
        static_cast<double>(context.start_symbol + nof_symbols) / (static_cast<double>(nof_symbols_per_slot))));

    // Subtract one to account for the current slot.
    slot_point slot = context.slot + (prach_length_slots - 1);
    ul_prach_repo.add(context, buffer, nof_prach_ports, slot);
  }

  if (!is_prach_cp_enabled) {
    return;
  }

  // Get frequency mapping information.
  prach_frequency_mapping_information freq_mapping_info =
      prach_frequency_mapping_get(preamble_info.scs, context.pusch_scs);

  // Add sixteen kappa to the cyclic prefix for short preamble.
  if (is_short_preamble(context.format)) {
    preamble_info.cp_length += phy_time_unit::from_units_of_kappa(16);
  }
  unsigned cp_length = preamble_info.cp_length.to_samples(ref_srate_Hz);

  // Determine the last symbol that starts right at or before the PRACH preamble (after the cyclic prefix).
  unsigned pusch_symbol_duration =
      phy_time_unit::from_units_of_kappa((144U + 2048U) >> to_numerology_value(context.pusch_scs))
          .to_samples(ref_srate_Hz);
  unsigned prach_start_symbol = context.start_symbol + (cp_length / pusch_symbol_duration);

  unsigned K = (1000 * scs_to_khz(context.pusch_scs)) / ra_scs_to_Hz(preamble_info.scs);

  data_flow_cplane_scheduling_prach_context cp_prach_context;
  cp_prach_context.slot            = context.slot;
  cp_prach_context.nof_repetitions = get_preamble_duration(context.format);
  cp_prach_context.start_symbol    = prach_start_symbol;
  cp_prach_context.prach_scs       = preamble_info.scs;
  cp_prach_context.scs             = context.pusch_scs;
  cp_prach_context.prach_nof_rb    = freq_mapping_info.nof_rb_ra * K;
  cp_prach_context.time_offset     = cp_length;
  cp_prach_context.prach_start_re  = context.rb_offset * K * NOF_SUBCARRIERS_PER_RB;

  // Determine Open Fronthaul filter index.
  cp_prach_context.filter_type = get_prach_cplane_filter_index(context, preamble_info);

  for (auto eaxc : prach_eaxc) {
    cp_prach_context.eaxc = eaxc;
    data_flow->enqueue_section_type_3_prach_message(cp_prach_context);
  }
}

void uplink_request_handler_impl::handle_new_uplink_slot(const resource_grid_context& context, resource_grid& grid)
{
  logger.debug("Registering UL context entry for slot '{}' and sector#{}", context.slot, context.sector);

  // Store the context in the repository.
  ul_slot_repo.add(context, grid);

  data_flow_cplane_type_1_context df_context;
  df_context.slot         = context.slot;
  df_context.filter_type  = filter_index_type::standard_channel_filter;
  df_context.direction    = data_direction::uplink;
  df_context.symbol_range = tdd_config ? get_active_tdd_ul_symbols(tdd_config.value(), context.slot.slot_index(), cp)
                                       : ofdm_symbol_range(0, get_nsymb_per_slot(cp));

  for (auto eaxc : ul_eaxc) {
    df_context.eaxc = eaxc;
    data_flow->enqueue_section_type_1_message(df_context);
  }
}
