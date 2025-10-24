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

#pragma once

#include "srsran/phy/support/precoding_formatters.h"
#include "srsran/phy/support/re_pattern_formatters.h"
#include "srsran/phy/upper/channel_processors/pucch/pucch_processor.h"
#include "srsran/phy/upper/channel_state_information_formatters.h"
#include "srsran/ran/pucch/pucch_context_formatter.h"
#include "srsran/support/format/delimited_formatter.h"

namespace fmt {

/// \brief Custom formatter for \c pucch_processor::format0_configuration.
template <>
struct formatter<srsran::pucch_processor::format0_configuration> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor::format0_configuration& config, FormatContext& ctx) const
  {
    if (config.context.has_value()) {
      helper.format_always(ctx, *config.context);
    }
    helper.format_always(ctx, "format=0");
    helper.format_if_verbose(ctx, "bwp=[{}, {})", config.bwp_start_rb, config.bwp_start_rb + config.bwp_size_rb);
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_always(ctx, "prb1={}", config.starting_prb);
    helper.format_always(
        ctx, "prb2={}", config.second_hop_prb.has_value() ? std::to_string(*config.second_hop_prb) : "na");
    helper.format_always(
        ctx, "symb=[{}, {})", config.start_symbol_index, config.start_symbol_index + config.nof_symbols);
    helper.format_always(ctx, "cs={}", config.initial_cyclic_shift);
    helper.format_if_verbose(ctx, "n_id={}", config.n_id);
    helper.format_if_verbose(ctx, "sr_opportunity={}", config.sr_opportunity);
    helper.format_if_verbose(ctx, "ports={}", srsran::span<const uint8_t>(config.ports));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pucch_processor::format1_configuration.
template <>
struct formatter<srsran::pucch_processor::format1_configuration> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor::format1_configuration& config, FormatContext& ctx) const
  {
    if (config.context.has_value()) {
      helper.format_always(ctx, *config.context);
    }
    helper.format_always(ctx, "format=1");
    helper.format_if_verbose(ctx, "bwp=[{}, {})", config.bwp_start_rb, config.bwp_start_rb + config.bwp_size_rb);
    helper.format_always(ctx, "prb1={}", config.starting_prb);
    helper.format_always(
        ctx, "prb2={}", config.second_hop_prb.has_value() ? std::to_string(*config.second_hop_prb) : "na");
    helper.format_always(
        ctx, "symb=[{}, {})", config.start_symbol_index, config.start_symbol_index + config.nof_symbols);
    helper.format_if_verbose(ctx, "n_id={}", config.n_id);
    helper.format_always(ctx, "cs={}", config.initial_cyclic_shift);
    helper.format_always(ctx, "occ={}", config.time_domain_occ);
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_if_verbose(ctx, "cp={}", config.cp.to_string());
    helper.format_if_verbose(ctx, "ports={}", srsran::span<const uint8_t>(config.ports));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pucch_processor::format2_configuration.
template <>
struct formatter<srsran::pucch_processor::format2_configuration> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor::format2_configuration& config, FormatContext& ctx) const
  {
    if (config.context.has_value()) {
      helper.format_always(ctx, *config.context);
    } else {
      helper.format_always(ctx, "rnti=0x{:04x}", config.rnti);
    }
    helper.format_always(ctx, "format=2");
    helper.format_if_verbose(ctx, "bwp=[{}, {})", config.bwp_start_rb, config.bwp_start_rb + config.bwp_size_rb);
    helper.format_always(ctx, "prb=[{}, {})", config.starting_prb, config.starting_prb + config.nof_prb);
    helper.format_always(
        ctx, "prb2={}", config.second_hop_prb.has_value() ? std::to_string(*config.second_hop_prb) : "na");
    helper.format_always(
        ctx, "symb=[{}, {})", config.start_symbol_index, config.start_symbol_index + config.nof_symbols);

    helper.format_if_verbose(ctx, "n_id={}", config.n_id);
    helper.format_if_verbose(ctx, "n_id0={}", config.n_id_0);
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_if_verbose(ctx, "cp={}", config.cp.to_string());
    helper.format_if_verbose(ctx, "ports={}", srsran::span<const uint8_t>(config.ports));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pucch_processor::format3_configuration.
template <>
struct formatter<srsran::pucch_processor::format3_configuration> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor::format3_configuration& config, FormatContext& ctx) const
  {
    if (config.context.has_value()) {
      helper.format_always(ctx, *config.context);
    } else {
      helper.format_always(ctx, "rnti=0x{:04x}", config.rnti);
    }
    helper.format_always(ctx, "format=3");
    helper.format_if_verbose(ctx, "bwp=[{}, {})", config.bwp_start_rb, config.bwp_start_rb + config.bwp_size_rb);
    helper.format_always(ctx, "prb=[{}, {})", config.starting_prb, config.starting_prb + config.nof_prb);
    helper.format_always(
        ctx, "prb2={}", config.second_hop_prb.has_value() ? std::to_string(*config.second_hop_prb) : "na");
    helper.format_always(
        ctx, "symb=[{}, {})", config.start_symbol_index, config.start_symbol_index + config.nof_symbols);

    helper.format_if_verbose(ctx, "n_id_scr={}", config.n_id_scrambling);
    helper.format_if_verbose(ctx, "n_id_hop={}", config.n_id_hopping);
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_if_verbose(ctx, "cp={}", config.cp.to_string());
    helper.format_if_verbose(ctx, "ports={}", srsran::span<const uint8_t>(config.ports));
    helper.format_if_verbose(ctx, "pi2_bpsk={}", config.pi2_bpsk);
    helper.format_if_verbose(ctx, "add_dmrs={}", config.additional_dmrs);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pucch_processor::format4_configuration.
template <>
struct formatter<srsran::pucch_processor::format4_configuration> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor::format4_configuration& config, FormatContext& ctx) const
  {
    if (config.context.has_value()) {
      helper.format_always(ctx, *config.context);
    } else {
      helper.format_always(ctx, "rnti=0x{:04x}", config.rnti);
    }
    helper.format_always(ctx, "format=4");
    helper.format_if_verbose(ctx, "bwp=[{}, {})", config.bwp_start_rb, config.bwp_start_rb + config.bwp_size_rb);
    helper.format_always(ctx, "prb=[{}, {})", config.starting_prb, config.starting_prb + 1);
    helper.format_always(
        ctx, "prb2={}", config.second_hop_prb.has_value() ? std::to_string(*config.second_hop_prb) : "na");
    helper.format_always(
        ctx, "symb=[{}, {})", config.start_symbol_index, config.start_symbol_index + config.nof_symbols);
    helper.format_always(ctx, "occ={}", config.occ_index);
    helper.format_always(ctx, "occ_len={}", config.occ_length);

    helper.format_if_verbose(ctx, "n_id_scr={}", config.n_id_scrambling);
    helper.format_if_verbose(ctx, "n_id_hop={}", config.n_id_hopping);
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_if_verbose(ctx, "cp={}", config.cp.to_string());
    helper.format_if_verbose(ctx, "ports={}", srsran::span<const uint8_t>(config.ports));
    helper.format_if_verbose(ctx, "pi2_bpsk={}", config.pi2_bpsk);
    helper.format_if_verbose(ctx, "add_dmrs={}", config.additional_dmrs);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pucch_processor_result.
template <>
struct formatter<srsran::pucch_processor_result> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor_result& result, FormatContext& ctx) const
  {
    unsigned nof_sr        = result.message.get_expected_nof_sr_bits();
    unsigned nof_harq_ack  = result.message.get_expected_nof_harq_ack_bits();
    unsigned nof_csi_part1 = result.message.get_expected_nof_csi_part1_bits();
    unsigned nof_csi_part2 = result.message.get_expected_nof_csi_part2_bits();

    helper.format_if_verbose(ctx, "status={}", to_string(result.message.get_status()));

    // PUCCH can carry a scheduling request without HARQ-ACK. In that case, the UE only transmits for positive SR.
    if ((nof_sr == 0) && (nof_harq_ack == 0) && (nof_csi_part1 == 0) && (nof_csi_part2 == 0)) {
      if (result.message.get_status() == srsran::uci_status::valid) {
        helper.format_always(ctx, "sr=yes");
      } else {
        helper.format_always(ctx, "sr=no");
      }
    }

    if (result.message.get_status() == srsran::uci_status::valid) {
      // Valid UCI payload.
      if (nof_harq_ack) {
        helper.format_always(ctx, "ack={:#}", srsran::span<const uint8_t>(result.message.get_harq_ack_bits()));
      }
      if (nof_sr) {
        helper.format_always(ctx, "sr={:#}", srsran::span<const uint8_t>(result.message.get_sr_bits()));
      }
      if (nof_csi_part1) {
        helper.format_always(ctx, "csi1={:#}", srsran::span<const uint8_t>(result.message.get_csi_part1_bits()));
      }
      if (nof_csi_part2) {
        helper.format_always(ctx, "csi2={:#}", srsran::span<const uint8_t>(result.message.get_csi_part2_bits()));
      }
    } else {
      // Bad UCI payload.
      std::array<uint8_t, srsran::uci_constants::MAX_NOF_PAYLOAD_BITS> bad_payload;
      std::fill(bad_payload.begin(), bad_payload.end(), 2U);

      if (nof_harq_ack) {
        helper.format_always(ctx, "ack={:#}", srsran::span<const uint8_t>(bad_payload).first(nof_harq_ack));
      }
      if (nof_sr) {
        helper.format_always(ctx, "sr={:#}", srsran::span<const uint8_t>(bad_payload).first(nof_sr));
      }
      if (nof_csi_part1) {
        helper.format_always(ctx, "csi1={:#}", srsran::span<const uint8_t>(bad_payload).first(nof_csi_part1));
      }
      if (nof_csi_part2) {
        helper.format_always(ctx, "csi2={:#}", srsran::span<const uint8_t>(bad_payload).first(nof_csi_part2));
      }
    }

    if (result.detection_metric.has_value()) {
      helper.format_always(ctx, "metric={:.1f}", *result.detection_metric);
    }

    helper.format_always(ctx, result.csi);
    return ctx.out();
  }
};

} // namespace fmt
