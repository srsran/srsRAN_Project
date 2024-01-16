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

/// \file
/// \brief Formatters for DCI packing related structures.

#pragma once

#include "srsran/ran/pdcch/dci_packing.h"
#include "srsran/support/format_utils.h"

namespace fmt {
/// \brief Custom formatter for \c dci_0_0_size.
template <>
struct formatter<srsran::dci_0_0_size> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() { helper = srsran::delimited_formatter("\n"); }

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::dci_0_0_size& dci_size, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "Total:                {}", dci_size.total);
    helper.format_always(ctx, "Frequency resource:   {}", dci_size.frequency_resource);
    helper.format_always(ctx, "Padding incl. UL/SUL: {}", dci_size.padding_incl_ul_sul);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c dci_1_0_size.
template <>
struct formatter<srsran::dci_1_0_size> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() { helper = srsran::delimited_formatter("\n"); }

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::dci_1_0_size& dci_size, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "Total:              {}", dci_size.total);
    helper.format_always(ctx, "Frequency resource: {}", dci_size.frequency_resource);
    helper.format_always(ctx, "Padding:            {}", dci_size.padding);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c dci_0_1_size.
template <>
struct formatter<srsran::dci_0_1_size> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() { helper = srsran::delimited_formatter("\n"); }

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::dci_0_1_size& dci_size, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "Total:                 {}", dci_size.total);
    helper.format_always(ctx, "Carrier indicator:     {}", dci_size.carrier_indicator);
    helper.format_always(ctx, "UL/SUL indicator:      {}", dci_size.ul_sul_indicator);
    helper.format_always(ctx, "BWP indicator:         {}", dci_size.bwp_indicator);
    helper.format_always(ctx, "Frequency resource:    {}", dci_size.frequency_resource);
    helper.format_always(ctx, "Time resource:         {}", dci_size.time_resource);
    helper.format_always(ctx, "Freq hopping flag:     {}", dci_size.freq_hopping_flag);
    helper.format_always(ctx, "First DAI:             {}", dci_size.first_dl_assignment_idx);
    helper.format_always(ctx, "Second DAI:            {}", dci_size.second_dl_assignment_idx);
    helper.format_always(ctx, "SRS res indicator:     {}", dci_size.srs_resource_indicator);
    helper.format_always(ctx, "Precoding info:        {}", dci_size.precoding_info_nof_layers);
    helper.format_always(ctx, "Antenna ports:         {}", dci_size.antenna_ports);
    helper.format_always(ctx, "SRS request:           {}", dci_size.srs_request);
    helper.format_always(ctx, "CSI request:           {}", dci_size.csi_request);
    helper.format_always(ctx, "CBGTI:                 {}", dci_size.cbg_transmission_info);
    helper.format_always(ctx, "PT-RS/DM-RS:           {}", dci_size.ptrs_dmrs_association);
    helper.format_always(ctx, "Beta offset indicator: {}", dci_size.beta_offset_indicator);
    helper.format_always(ctx, "DM-RS sequence init:   {}", dci_size.dmrs_seq_initialization);
    helper.format_always(ctx, "Padding:               {}", dci_size.padding);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c dci_1_1_size.
template <>
struct formatter<srsran::dci_1_1_size> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() { helper = srsran::delimited_formatter("\n"); }

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::dci_1_1_size& dci_size, FormatContext& ctx) -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "Total:                   {}", dci_size.total);
    helper.format_always(ctx, "Carrier indicator:       {}", dci_size.carrier_indicator);
    helper.format_always(ctx, "BWP indicator:           {}", dci_size.bwp_indicator);
    helper.format_always(ctx, "Frequency resource:      {}", dci_size.frequency_resource);
    helper.format_always(ctx, "Time resource:           {}", dci_size.time_resource);
    helper.format_always(ctx, "VRB/PRB mapping:         {}", dci_size.vrb_prb_mapping);
    helper.format_always(ctx, "PRB bundling size:       {}", dci_size.prb_bundling_size_indicator);
    helper.format_always(ctx, "Rate matching indicator: {}", dci_size.rate_matching_indicator);
    helper.format_always(ctx, "ZP CSI-RS trigger:       {}", dci_size.zp_csi_rs_trigger);
    helper.format_always(ctx, "TB2 MCS:                 {}", dci_size.tb2_modulation_coding_scheme);
    helper.format_always(ctx, "TB2 NDI:                 {}", dci_size.tb2_new_data_indicator);
    helper.format_always(ctx, "TB2 RV:                  {}", dci_size.tb2_redundancy_version);
    helper.format_always(ctx, "DAI:                     {}", dci_size.downlink_assignment_index);
    helper.format_always(ctx, "PDSCH-HARQ timing:       {}", dci_size.pdsch_harq_fb_timing_indicator);
    helper.format_always(ctx, "Antenna ports:           {}", dci_size.antenna_ports);
    helper.format_always(ctx, "Tx config indication:    {}", dci_size.tx_config_indication);
    helper.format_always(ctx, "SRS request:             {}", dci_size.srs_request);
    helper.format_always(ctx, "CBGTI:                   {}", dci_size.cbg_transmission_info);
    helper.format_always(ctx, "CBGFI:                   {}", dci_size.cbg_flushing_info);
    helper.format_always(ctx, "Padding:                 {}", dci_size.padding);

    return ctx.out();
  }
};

} // namespace fmt