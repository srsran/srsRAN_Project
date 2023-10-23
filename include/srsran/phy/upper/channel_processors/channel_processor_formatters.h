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

#pragma once

#include "srsran/phy/support/precoding_formatters.h"
#include "srsran/phy/support/re_pattern_formatters.h"
#include "srsran/phy/upper/channel_processors/pdcch_processor.h"
#include "srsran/phy/upper/channel_processors/pdsch_processor.h"
#include "srsran/phy/upper/channel_processors/prach_detector.h"
#include "srsran/phy/upper/channel_processors/pucch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/phy/upper/channel_processors/ssb_processor.h"
#include "srsran/ran/pdcch/pdcch_context_formatter.h"
#include "srsran/ran/pdsch/pdsch_context_formatter.h"
#include "srsran/ran/pucch/pucch_context_formatter.h"
#include "srsran/ran/pusch/pusch_context_formatter.h"
#include "srsran/ran/uci/uci_formatters.h"
#include "srsran/srsvec/copy.h"
#include "srsran/support/format_utils.h"

namespace fmt {

/// \brief Custom formatter for \c pdcch_processor::coreset_description.
template <>
struct formatter<srsran::pdcch_processor::coreset_description> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pdcch_processor::coreset_description& coreset, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "bwp=[{}, {})", coreset.bwp_start_rb, coreset.bwp_start_rb + coreset.bwp_size_rb);
    helper.format_always(
        ctx, "symb=[{}, {})", coreset.start_symbol_index, coreset.start_symbol_index + coreset.duration);
    helper.format_always(ctx, "f_re={}", coreset.frequency_resources);

    switch (coreset.cce_to_reg_mapping) {
      case srsran::pdcch_processor::cce_to_reg_mapping_type::CORESET0:
        helper.format_if_verbose(ctx, "mapping=coreset0");
        break;
      case srsran::pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED:
        helper.format_if_verbose(ctx, "mapping=interleaved");
        helper.format_if_verbose(ctx, "reg_bundle_size={}", coreset.reg_bundle_size);
        helper.format_if_verbose(ctx, "interleaver_size={}", coreset.interleaver_size);
        break;
      case srsran::pdcch_processor::cce_to_reg_mapping_type::NON_INTERLEAVED:
        helper.format_if_verbose(ctx, "mapping=non-interleaved");
        break;
    }

    helper.format_if_verbose(ctx, "shift_idx={}", coreset.shift_index);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pdcch_processor::pdu_t.
template <>
struct formatter<srsran::pdcch_processor::pdu_t> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pdcch_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "rnti=0x{:04x}", pdu.dci.rnti);
    if (pdu.context.has_value()) {
      helper.format_always(ctx, pdu.context.value());
    }
    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "cp={}", pdu.cp.to_string());
    helper.format_if_verbose(ctx, pdu.coreset);
    helper.format_always(ctx, "cce={}", pdu.dci.cce_index);
    helper.format_always(ctx, "al={}", pdu.dci.aggregation_level);
    helper.format_if_verbose(ctx, "size={}", pdu.dci.payload.size());
    helper.format_if_verbose(ctx, "n_id_dmrs={}", pdu.dci.n_id_pdcch_dmrs);
    helper.format_if_verbose(ctx, "n_id_data={}", pdu.dci.n_id_pdcch_data);
    helper.format_if_verbose(ctx, "n_rnti={}", pdu.dci.n_rnti);
    helper.format_if_verbose(ctx, "power_dmrs={:+.1f}dB", pdu.dci.dmrs_power_offset_dB);
    helper.format_if_verbose(ctx, "power_data={:+.1f}dB", pdu.dci.data_power_offset_dB);
    helper.format_if_verbose(ctx, "precoding={}", pdu.dci.precoding);
    return ctx.out();
  }
};

/// \brief Custom formatter for \c pdsch_processor::codeword_description.
template <>
struct formatter<srsran::pdsch_processor::codeword_description> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pdsch_processor::codeword_description& codeword_descr, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "mod={}", to_string(codeword_descr.modulation));
    helper.format_always(ctx, "rv={}", codeword_descr.rv);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pdsch_processor::pdu_t.
template <>
struct formatter<srsran::pdsch_processor::pdu_t> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pdsch_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "rnti=0x{:04x}", pdu.rnti);
    if (pdu.context.has_value()) {
      helper.format_always(ctx, pdu.context.value());
    }
    helper.format_if_verbose(ctx, "bwp=[{}, {})", pdu.bwp_start_rb, pdu.bwp_start_rb + pdu.bwp_size_rb);
    helper.format_always(ctx, "prb={}", pdu.freq_alloc);
    helper.format_always(ctx, "symb=[{}, {})", pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols);
    helper.format_always(ctx, srsran::span<const srsran::pdsch_processor::codeword_description>(pdu.codewords));

    helper.format_if_verbose(ctx, "n_id={}", pdu.n_id);
    helper.format_if_verbose(
        ctx, "ref_point={}", (pdu.ref_point == srsran::pdsch_processor::pdu_t::CRB0) ? "CRB0" : "PRB0");
    helper.format_if_verbose(ctx, "dmrs_type={}", (pdu.dmrs == srsran::dmrs_type::TYPE1) ? 1 : 2);
    helper.format_if_verbose(ctx, "dmrs_mask={}", pdu.dmrs_symbol_mask);
    helper.format_if_verbose(ctx, "n_scidid={}", pdu.scrambling_id);
    helper.format_if_verbose(ctx, "n_scid={}", pdu.n_scid);
    helper.format_if_verbose(ctx, "ncgwd={}", pdu.nof_cdm_groups_without_data);
    helper.format_if_verbose(ctx, "bg={}", (pdu.ldpc_base_graph == srsran::ldpc_base_graph_type::BG1) ? "BG1" : "BG2");
    helper.format_if_verbose(ctx, "lbrm={}bytes", pdu.tbs_lbrm_bytes);
    helper.format_if_verbose(ctx, "power_dmrs={:+.1f}dB", -pdu.ratio_pdsch_dmrs_to_sss_dB);
    helper.format_if_verbose(ctx, "power_data={:+.1f}dB", pdu.ratio_pdsch_data_to_sss_dB);
    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "cp={}", pdu.cp.to_string());
    helper.format_if_verbose(ctx, "precoding={}", pdu.precoding);
    if (pdu.reserved.get_nof_entries() > 0) {
      helper.format_if_verbose(ctx, "reserved=[{:,}]", pdu.reserved.get_re_patterns());
    }
    return ctx.out();
  }
};

/// \brief Custom formatter for \c prach_detector::configuration.
template <>
struct formatter<srsran::prach_detector::configuration> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::prach_detector::configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_if_verbose(ctx, "rsi={}", config.root_sequence_index);
    helper.format_if_verbose(ctx,
                             "preambles=[{}, {})",
                             config.start_preamble_index,
                             config.start_preamble_index + config.nof_preamble_indices);
    helper.format_if_verbose(ctx, "format={}", to_string(config.format));
    helper.format_if_verbose(ctx, "set={}", to_string(config.restricted_set));
    helper.format_if_verbose(ctx, "zcz={}", config.zero_correlation_zone);
    helper.format_if_verbose(ctx, "scs={}", to_string(config.ra_scs));
    helper.format_if_verbose(ctx, "nof_rx_ports={}", config.nof_rx_ports);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c prach_detection_result::preamble_indication.
template <>
struct formatter<srsran::prach_detection_result::preamble_indication> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::prach_detection_result::preamble_indication& preamble, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    format_to(ctx.out(),
              "{{idx={} ta={:.2f}us power={:+.1f}dB snr={:.1f}dB}}",
              preamble.preamble_index,
              preamble.time_advance.to_seconds() * 1e6,
              preamble.power_dB,
              preamble.snr_dB);
    return ctx.out();
  }
};

/// \brief Custom formatter for \c prach_detection_result.
template <>
struct formatter<srsran::prach_detection_result> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::prach_detection_result& result, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "rssi={:+.1f}dB", result.rssi_dB);
    helper.format_if_verbose(ctx, "res={:.1f}us", result.time_resolution.to_seconds() * 1e6);
    helper.format_if_verbose(ctx, "max_ta={:.2f}us", result.time_advance_max.to_seconds() * 1e6);
    helper.format_always(ctx,
                         "detected_preambles=[{:,}]",
                         srsran::span<const srsran::prach_detection_result::preamble_indication>(result.preambles));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pucch_processor::format0_configuration.
template <>
struct formatter<srsran::pucch_processor::format0_configuration> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor::format0_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "format0_configuration");
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
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor::format1_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (config.context.has_value()) {
      helper.format_always(ctx, config.context.value());
    }
    helper.format_always(ctx, "format=1");
    helper.format_if_verbose(ctx, "bwp=[{}, {})", config.bwp_start_rb, config.bwp_start_rb + config.bwp_size_rb);
    helper.format_always(ctx, "prb1={}", config.starting_prb);
    helper.format_always(
        ctx, "prb2={}", config.second_hop_prb.has_value() ? std::to_string(config.second_hop_prb.value()) : "na");
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
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor::format2_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (config.context.has_value()) {
      helper.format_always(ctx, config.context.value());
    } else {
      helper.format_always(ctx, "rnti=0x{:04x}", config.rnti);
    }
    helper.format_always(ctx, "format=2");
    helper.format_if_verbose(ctx, "bwp=[{}, {})", config.bwp_start_rb, config.bwp_start_rb + config.bwp_size_rb);
    helper.format_always(ctx, "prb=[{}, {})", config.starting_prb, config.starting_prb + config.nof_prb);
    helper.format_always(
        ctx, "prb2={}", config.second_hop_prb.has_value() ? std::to_string(config.second_hop_prb.value()) : "na");
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
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor::format3_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "format3_configuration");
  }
};

/// \brief Custom formatter for \c pucch_processor::format4_configuration.
template <>
struct formatter<srsran::pucch_processor::format4_configuration> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor::format4_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "format4_configuration");
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
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pucch_processor_result& result, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
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
      helper.format_if_verbose(ctx, "detection_metric={:.1f}", result.detection_metric.value());
    }

    // Channel State Information.
    helper.format_if_verbose(ctx, "epre={:+.1f}dB", result.csi.get_epre_dB());
    helper.format_if_verbose(ctx, "rsrp={:+.1f}dB", result.csi.get_rsrp_dB());
    helper.format_if_verbose(ctx, "sinr={:+.1f}dB", result.csi.get_sinr_dB());
    helper.format_if_verbose(ctx, "t_align={:.1f}us", result.csi.get_time_alignment().to_seconds() * 1e6);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pusch_processor::codeword_description.
template <>
struct formatter<srsran::pusch_processor::codeword_description> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::optional<srsran::pusch_processor::codeword_description>& codeword, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "rv={}", codeword.value().rv);
    helper.format_if_verbose(ctx, "bg={}", codeword.value().ldpc_base_graph);
    helper.format_if_verbose(ctx, "new_data={}", codeword.value().new_data);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pusch_processor::uci_description.
template <>
struct formatter<srsran::pusch_processor::uci_description> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pusch_processor::uci_description& uci_desc, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    // Number of ACK, CSI Part 1 and CSI Part 2 bits.
    helper.format_if_verbose(ctx, "oack={}", uci_desc.nof_harq_ack);
    helper.format_if_verbose(ctx, "ocsi1={}", uci_desc.nof_csi_part1);
    helper.format_if_verbose(ctx, "part2={}", uci_desc.csi_part2_size);

    // Scaling and offset parameters.
    helper.format_if_verbose(ctx, "alpha={}", uci_desc.alpha_scaling);

    helper.format_if_verbose(ctx,
                             "betas=[{}, {}, {}]",
                             uci_desc.beta_offset_harq_ack,
                             uci_desc.beta_offset_csi_part1,
                             uci_desc.beta_offset_csi_part2);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pusch_processor::pdu_t.
template <>
struct formatter<srsran::pusch_processor::pdu_t> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pusch_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (pdu.context.has_value()) {
      helper.format_always(ctx, pdu.context.value());
    } else {
      helper.format_always(ctx, "rnti=0x{:04x}", pdu.rnti);
    }
    helper.format_if_verbose(ctx, "bwp=[{}, {})", pdu.bwp_start_rb, pdu.bwp_start_rb + pdu.bwp_size_rb);
    helper.format_always(ctx, "prb={}", pdu.freq_alloc);
    helper.format_always(ctx, "symb=[{}, {})", pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols);

    // UCI description.
    helper.format_always(ctx, pdu.uci);

    // Verbose parameters.
    helper.format_always(ctx, "mod={}", to_string(pdu.mcs_descr.modulation));
    helper.format_if_verbose(ctx, "tcr={}", pdu.mcs_descr.get_normalised_target_code_rate());

    // PUSCH data codeword if available.
    if (pdu.codeword.has_value()) {
      helper.format_always(ctx, pdu.codeword.value());
    }

    helper.format_if_verbose(ctx, "n_id={}", pdu.n_id);
    helper.format_if_verbose(ctx, "dmrs_mask={}", pdu.dmrs_symbol_mask);
    helper.format_if_verbose(ctx, "n_scr_id={}", pdu.scrambling_id);
    helper.format_if_verbose(ctx, "n_scid={}", pdu.n_scid);
    helper.format_if_verbose(ctx, "n_cdm_g_wd={}", pdu.nof_cdm_groups_without_data);
    helper.format_if_verbose(ctx, "dmrs_type={}", (pdu.dmrs == srsran::dmrs_type::TYPE1) ? 1 : 2);
    helper.format_if_verbose(ctx, "lbrm={}bytes", pdu.tbs_lbrm_bytes);
    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "cp={}", pdu.cp.to_string());
    helper.format_if_verbose(ctx, "nof_layers={}", pdu.nof_tx_layers);
    helper.format_if_verbose(ctx, "ports={}", srsran::span<const uint8_t>(pdu.rx_ports));
    helper.format_if_verbose(ctx, "dc_position={}", pdu.dc_position);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pusch_decoder_result.
template <>
struct formatter<srsran::pusch_decoder_result> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pusch_decoder_result& result, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "crc={}", result.tb_crc_ok ? "OK" : "KO");
    helper.format_always(ctx, "iter={:.1f}", result.ldpc_decoder_stats.get_mean());

    helper.format_if_verbose(ctx, "max_iter={}", result.ldpc_decoder_stats.get_max());
    helper.format_if_verbose(ctx, "min_iter={}", result.ldpc_decoder_stats.get_min());
    helper.format_if_verbose(ctx, "nof_cb={}", result.nof_codeblocks_total);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c channel_state_information.
template <>
struct formatter<srsran::channel_state_information> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::channel_state_information& csi, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (helper.is_verbose()) {
      // Verbose representation prints all available SINR parameters.
      if (csi.sinr_report_type == srsran::channel_state_information::sinr_type::channel_estimator) {
        helper.format_if_verbose(ctx, "sinr_ch_est[sel]={:.1f}dB", csi.get_sinr_dB());
      } else if (csi.sinr_ch_estimator_dB.has_value()) {
        helper.format_if_verbose(ctx, "sinr_ch_est={:.1f}dB", csi.sinr_ch_estimator_dB.value());
      }

      if (csi.sinr_report_type == srsran::channel_state_information::sinr_type::post_equalization) {
        helper.format_if_verbose(ctx, "sinr_eq[sel]={:.1f}dB", csi.get_sinr_dB());
      } else if (csi.sinr_post_eq_dB.has_value()) {
        helper.format_if_verbose(ctx, "sinr_eq={:.1f}dB", csi.sinr_post_eq_dB.value());
      }

      if (csi.sinr_report_type == srsran::channel_state_information::sinr_type::evm) {
        helper.format_if_verbose(ctx, "sinr_evm[sel]={:.1f}dB", csi.get_sinr_dB());
      } else if (csi.sinr_evm_dB.has_value()) {
        helper.format_if_verbose(ctx, "sinr_evm={:.1f}dB", csi.sinr_evm_dB.value());
      }

      if (csi.evm.has_value()) {
        helper.format_if_verbose(ctx, "evm={:.2f}", csi.evm.value());
      }

      helper.format_if_verbose(ctx, "epre={:+.1f}dB", csi.get_epre_dB());
      helper.format_if_verbose(ctx, "rsrp={:+.1f}dB", csi.get_rsrp_dB());
      helper.format_if_verbose(ctx, "t_align={:.1f}us", csi.get_time_alignment().to_seconds() * 1e6);
    } else {
      // Short representation only prints the SINR selected for CSI reporting to higher layers.
      helper.format_always(ctx, "sinr={:.1f}dB", csi.get_sinr_dB());
    }
    return ctx.out();
  }
};

/// \brief Custom formatter for \c pusch_processor_result_data.
template <>
struct formatter<srsran::pusch_processor_result_data> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pusch_processor_result_data& result, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, result.data);
    return ctx.out();
  }
};

/// \brief Custom formatter for \c pusch_processor_result_control.
template <>
struct formatter<srsran::pusch_processor_result_control> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::pusch_processor_result_control& result, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    // Generate bad payload, to use in case of invalid or unknown UCI status.
    std::array<uint8_t, srsran::uci_constants::MAX_NOF_PAYLOAD_BITS> bad_payload;
    std::fill(bad_payload.begin(), bad_payload.end(), 2U);

    if ((!result.harq_ack.payload.empty())) {
      if (result.harq_ack.status == srsran::uci_status::valid) {
        helper.format_always(ctx, "ack={:#}", srsran::span<const uint8_t>(result.harq_ack.payload));
      } else {
        helper.format_always(
            ctx, "ack={:#}", srsran::span<const uint8_t>(bad_payload).first(result.harq_ack.payload.size()));
      }
    }
    if ((!result.csi_part1.payload.empty())) {
      if (result.csi_part1.status == srsran::uci_status::valid) {
        helper.format_always(ctx, "csi1={:#}", srsran::span<const uint8_t>(result.csi_part1.payload));
      } else {
        helper.format_always(
            ctx, "csi1={:#}", srsran::span<const uint8_t>(bad_payload).first(result.csi_part1.payload.size()));
      }
    }
    if ((!result.csi_part2.payload.empty())) {
      if (result.csi_part2.status == srsran::uci_status::valid) {
        helper.format_always(ctx, "csi2={:#}", srsran::span<const uint8_t>(result.csi_part2.payload));
      } else {
        helper.format_always(
            ctx, "csi2={:#}", srsran::span<const uint8_t>(bad_payload).first(result.csi_part2.payload.size()));
      }
    }
    return ctx.out();
  }
};

/// \brief Custom formatter for \c ssb_processor::pdu_t.
template <>
struct formatter<srsran::ssb_processor::pdu_t> {
  /// Helper used to parse formatting options and format fields.
  srsran::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsran::ssb_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "pci={}", pdu.phys_cell_id);
    helper.format_always(ctx, "ssb_idx={}", pdu.ssb_idx);
    helper.format_always(ctx, "L_max={}", pdu.L_max);
    helper.format_always(ctx, "common_scs={}", scs_to_khz(pdu.common_scs));
    helper.format_always(ctx, "sc_offset={}", pdu.subcarrier_offset.value());
    helper.format_always(ctx, "offset_PointA={}", pdu.offset_to_pointA.value());
    helper.format_always(ctx, "pattern={}", to_string(pdu.pattern_case));

    helper.format_if_verbose(ctx, "beta_pss={:+.1f}dB", pdu.beta_pss);
    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "ports={}", srsran::span<const uint8_t>(pdu.ports));

    return ctx.out();
  }
};

} // namespace fmt
