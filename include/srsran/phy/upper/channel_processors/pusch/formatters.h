/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_processor_result_notifier.h"
#include "srsran/ran/pusch/pusch_context_formatter.h"
#include "srsran/ran/uci/uci_formatters.h"
#include "srsran/support/format_utils.h"

namespace srsran {
namespace detail {

class uci_bad_payload
{
public:
  uci_bad_payload(std::size_t size) : bad_payload(size, 2U) {}

  span<const uint8_t> get() const { return bad_payload; }

private:
  static_vector<uint8_t, uci_constants::MAX_NOF_PAYLOAD_BITS> bad_payload;
};

} // namespace detail
} // namespace srsran

namespace fmt {

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
    // Verbose representation prints all available SINR parameters. It appends [sel] to the label of the selected
    // measurement point.
    if (helper.is_verbose()) {
      if (csi.sinr_ch_estimator_dB.has_value()) {
        const char* selected =
            (csi.sinr_report_type == srsran::channel_state_information::sinr_type::channel_estimator) ? "[sel]" : "";
        helper.format_if_verbose(ctx, "sinr_ch_est{}={:.1f}dB", selected, csi.sinr_ch_estimator_dB.value());
      }

      if (csi.sinr_post_eq_dB.has_value()) {
        const char* selected =
            (csi.sinr_report_type == srsran::channel_state_information::sinr_type::post_equalization) ? "[sel]" : "";
        helper.format_if_verbose(ctx, "sinr_eq{}={:.1f}dB", selected, csi.sinr_post_eq_dB.value());
      }

      if (csi.sinr_evm_dB.has_value()) {
        const char* selected =
            (csi.sinr_report_type == srsran::channel_state_information::sinr_type::evm) ? "[sel]" : "";
        helper.format_if_verbose(ctx, "sinr_evm{}={:.1f}dB", selected, csi.sinr_evm_dB.value());
      }

      if (csi.evm.has_value()) {
        helper.format_if_verbose(ctx, "evm={:.2f}", csi.evm.value());
      }

      // Extract CSI measurements. It could be some of them are not available.
      srsran::optional<float>                 epre_dB       = csi.get_epre_dB();
      srsran::optional<float>                 rsrp_dB       = csi.get_rsrp_dB();
      srsran::optional<float>                 sinr_dB       = csi.get_sinr_dB();
      srsran::optional<srsran::phy_time_unit> time_aligment = csi.get_time_alignment();
      srsran::optional<float>                 cfo_Hz        = csi.get_cfo_Hz();

      // Print the measurements that are present.
      if (epre_dB.has_value()) {
        helper.format_if_verbose(ctx, "epre={:+.1f}dB", epre_dB.value());
      } else {
        helper.format_if_verbose(ctx, "epre=na");
      }
      if (rsrp_dB.has_value()) {
        helper.format_if_verbose(ctx, "rsrp={:+.1f}dB", rsrp_dB.value());
      } else {
        helper.format_if_verbose(ctx, "rsrp=na");
      }
      if (sinr_dB.has_value()) {
        helper.format_if_verbose(ctx, "sinr={:+.1f}dB", sinr_dB.value());
      } else {
        helper.format_if_verbose(ctx, "sinr=na");
      }
      if (time_aligment.has_value()) {
        helper.format_if_verbose(ctx, "t_align={:.2f}us", time_aligment.value().to_seconds() * 1e6);
      } else {
        helper.format_if_verbose(ctx, "t_align=na");
      }
      if (cfo_Hz.has_value()) {
        helper.format_if_verbose(ctx, "cfo={:.2f}Hz", cfo_Hz.value());
      } else {
        helper.format_if_verbose(ctx, "cfo=na");
      }
    } else {
      // Short representation only prints the SINR selected for CSI reporting to higher layers.
      srsran::optional<float> sinr_dB = csi.get_sinr_dB();
      if (sinr_dB.has_value()) {
        helper.format_always(ctx, "sinr={:.1f}dB", sinr_dB.value());
      } else {
        // SINR is not available.
        helper.format_always(ctx, "sinr=na");
      }
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
    if ((!result.harq_ack.payload.empty())) {
      if (result.harq_ack.status == srsran::uci_status::valid) {
        helper.format_always(ctx, "ack={:b}", result.harq_ack.payload);
      } else {
        helper.format_always(ctx, "ack={:#}", srsran::detail::uci_bad_payload(result.harq_ack.payload.size()).get());
      }
    }
    if ((!result.csi_part1.payload.empty())) {
      if (result.csi_part1.status == srsran::uci_status::valid) {
        helper.format_always(ctx, "csi1={:b}", result.csi_part1.payload);
      } else {
        helper.format_always(ctx, "csi1={:#}", srsran::detail::uci_bad_payload(result.csi_part1.payload.size()).get());
      }
    }
    if ((!result.csi_part2.payload.empty())) {
      if (result.csi_part2.status == srsran::uci_status::valid) {
        helper.format_always(ctx, "csi2={:b}", result.csi_part2.payload);
      } else {
        helper.format_always(ctx, "csi2={:#}", srsran::detail::uci_bad_payload(result.csi_part2.payload.size()).get());
      }
    }
    return ctx.out();
  }
};

} // namespace fmt
