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
  ul_slot_repo(std::move(dependencies.ul_slot_repo)),
  ul_prach_repo(std::move(dependencies.ul_prach_repo)),
  data_flow(std::move(dependencies.data_flow)),
  frame_pool(std::move(dependencies.frame_pool))
{
  srsran_assert(ul_slot_repo, "Invalid uplink repository");
  srsran_assert(ul_prach_repo, "Invalid PRACH repository");
  srsran_assert(data_flow, "Invalid data flow");
  srsran_assert(frame_pool, "Invalid frame pool");
}

/// Determines slot index where U-Plane packet is expected for long format PRACH.
static slot_point get_long_prach_length_slots(const prach_buffer_context& context)
{
  static constexpr unsigned nof_symbols_per_slot = get_nsymb_per_slot(cyclic_prefix::NORMAL);
  srsran_assert(is_long_preamble(context.format), "Long PRACH format expected");

  // Get preamble information.
  prach_preamble_information preamble_info = get_prach_preamble_long_info(context.format);

  double pusch_symbol_duration_msec =
      static_cast<double>(SUBFRAME_DURATION_MSEC) /
      static_cast<double>(get_nof_slots_per_subframe(context.pusch_scs) * nof_symbols_per_slot);

  double   len_msecs   = (preamble_info.cp_length.to_seconds() + preamble_info.symbol_length().to_seconds()) * 1000;
  unsigned nof_symbols = std::ceil(len_msecs / pusch_symbol_duration_msec);

  unsigned prach_length_slots =
      std::ceil(static_cast<double>(context.start_symbol + nof_symbols) / static_cast<double>(nof_symbols_per_slot));

  // Subtract one to account for the current slot.
  return (context.slot + (prach_length_slots - 1));
}

/// \brief Determine PRACH start symbol index.
///
/// Determine startSymbolId as the last symbol that starts right at or before the PRACH preamble (after the cyclic
/// prefix). According to O-RAN.WG4.CUS.0.R003 section 4.4.3 if the SCS value provided by "frameStructure" is less than
/// 15 kHz (e.g. for long preamble PRACH formats), then the symbol timing used to determine startSymbolId is based on
/// the numerology of 15 kHz SCS.
///
/// \param context PRACH context storing PRACH time and frequency mapping parameters.
/// \return OFDM symbol index that marks the start of PRACH preamble after the cyclic prefix.
static unsigned get_prach_start_symbol(const prach_buffer_context& context)
{
  // Get preamble information (assuming only a single occasion is supported).
  prach_preamble_information preamble_info =
      is_long_preamble(context.format)
          ? get_prach_preamble_long_info(context.format)
          : get_prach_preamble_short_info(context.format, to_ra_subcarrier_spacing(context.pusch_scs), true);

  subcarrier_spacing scs = is_short_preamble(context.format) ? static_cast<subcarrier_spacing>(preamble_info.scs)
                                                             : subcarrier_spacing::kHz15;

  double symbol_duration_sec =
      phy_time_unit::from_units_of_kappa((144U + 2048U) >> to_numerology_value(scs)).to_seconds();
  double cp_length_sec = preamble_info.cp_length.to_seconds();

  return context.start_symbol + unsigned(cp_length_sec / symbol_duration_sec);
}

void uplink_request_handler_impl::handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer)
{
  logger.debug("Registering PRACH context entry for slot '{}' and sector#{}", context.slot, context.sector);

  frame_pool->clear_uplink_slot(context.slot, logger);

  // Sampling rate defining the \f$T_s = 1/(\Delta f_{ref} \times N_{f,ref})\f$ parameter, see 3GPP TS38.211,
  // clause 4.1.
  // Open Fronthaul parameters timeOffset and cpLength are expressed in multiple of \f$T_s\f$ units.
  static constexpr double ref_srate_Hz = 30.72e6;

  // Store the context in the repository, use correct slot index for long format accounting for PRACH duration.
  if (is_short_preamble(context.format)) {
    ul_prach_repo->add(context, buffer, std::nullopt, std::nullopt);
  } else {
    // Determine slot index where the PRACH U-Plane is expected.
    slot_point slot = get_long_prach_length_slots(context);
    // Determine PRACH start symbol.
    unsigned start_symbol = get_prach_start_symbol(context);

    ul_prach_repo->add(context, buffer, start_symbol, slot);
  }

  if (!is_prach_cp_enabled) {
    return;
  }
  // Get preamble information (assuming only single occasion is supported).
  prach_preamble_information preamble_info =
      is_long_preamble(context.format)
          ? get_prach_preamble_long_info(context.format)
          : get_prach_preamble_short_info(context.format, to_ra_subcarrier_spacing(context.pusch_scs), true);

  // Get frequency mapping information.
  prach_frequency_mapping_information freq_mapping_info =
      prach_frequency_mapping_get(preamble_info.scs, context.pusch_scs);

  // Add sixteen kappa to the cyclic prefix for short preamble.
  if (is_short_preamble(context.format)) {
    preamble_info.cp_length += phy_time_unit::from_units_of_kappa(16);
  }
  unsigned cp_length = preamble_info.cp_length.to_samples(ref_srate_Hz);
  unsigned K         = (1000 * scs_to_khz(context.pusch_scs)) / ra_scs_to_Hz(preamble_info.scs);

  data_flow_cplane_scheduling_prach_context cp_prach_context;
  cp_prach_context.slot            = context.slot;
  cp_prach_context.nof_repetitions = preamble_info.nof_symbols;
  cp_prach_context.start_symbol    = get_prach_start_symbol(context);
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

  frame_pool->clear_uplink_slot(context.slot, logger);

  data_flow_cplane_type_1_context df_context;
  df_context.slot         = context.slot;
  df_context.filter_type  = filter_index_type::standard_channel_filter;
  df_context.direction    = data_direction::uplink;
  df_context.symbol_range = tdd_config ? get_active_tdd_ul_symbols(tdd_config.value(), context.slot.slot_index(), cp)
                                       : ofdm_symbol_range(0, get_nsymb_per_slot(cp));

  // Store the context in the repository.
  ul_slot_repo->add(context, grid, df_context.symbol_range);

  for (auto eaxc : ul_eaxc) {
    df_context.eaxc = eaxc;
    data_flow->enqueue_section_type_1_message(df_context);
  }
}
