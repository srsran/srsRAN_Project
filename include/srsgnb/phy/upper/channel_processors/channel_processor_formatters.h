/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_processors/pdcch_processor.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_processor.h"
#include "srsgnb/phy/upper/channel_processors/prach_detector.h"
#include "srsgnb/phy/upper/channel_processors/pucch_processor.h"
#include "srsgnb/phy/upper/channel_processors/pusch_processor.h"
#include "srsgnb/phy/upper/channel_processors/ssb_processor.h"

namespace fmt {

/// \brief Custom formatter for pdcch_processor::coreset_description.
template <>
struct formatter<srsgnb::pdcch_processor::coreset_description> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pdcch_processor::coreset_description& coreset, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    format_to(ctx.out(),
              "bwp={}:{} t_alloc={}:{} f_res={}",
              coreset.bwp_start_rb,
              coreset.bwp_size_rb,
              coreset.start_symbol_index,
              coreset.duration,
              coreset.frequency_resources);

    switch (coreset.cce_to_reg_mapping) {
      case srsgnb::pdcch_processor::cce_to_reg_mapping_type::CORESET0:
        format_to(ctx.out(), " mapping=coreset0");
        break;
      case srsgnb::pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED:
        format_to(ctx.out(),
                  " mapping=interleaved reg_bundle_size={} interleaver_size={}",
                  coreset.reg_bundle_size,
                  coreset.interleaver_size);
        break;
      case srsgnb::pdcch_processor::cce_to_reg_mapping_type::NON_INTERLEAVED:
        format_to(ctx.out(), " mapping=non-interleaved");
        break;
    }

    return ctx.out();
  }
};

/// \brief Custom formatter for pdcch_processor::dci_description.
template <>
struct formatter<srsgnb::pdcch_processor::dci_description> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pdcch_processor::dci_description& dci, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(
        ctx.out(),
        "rnti=0x{:04x} n_id_dmrs={} n_id_data={} n_rnti={} cce={} al={} power={:+.1f}/{:+.1f} payload=[{}] ports=[{}]",
        dci.rnti,
        dci.n_id_pdcch_dmrs,
        dci.n_id_pdcch_data,
        dci.n_rnti,
        dci.cce_index,
        dci.aggregation_level,
        dci.dmrs_power_offset_dB,
        dci.data_power_offset_dB,
        srsgnb::span<const uint8_t>(dci.payload),
        srsgnb::span<const uint8_t>(dci.ports));
  }
};

/// \brief Custom formatter for pdcch_processor::pdu_t.
template <>
struct formatter<srsgnb::pdcch_processor::pdu_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pdcch_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "slot={} cp={} {} {}", pdu.slot, to_string(pdu.cp), pdu.coreset, pdu.dci_list.front());
  }
};

/// \brief Custom formatter for pdsch_processor::codeword_description.
template <>
struct formatter<srsgnb::pdsch_processor::codeword_description> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pdsch_processor::codeword_description& codeword_descr, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "{{mod={} rv={}}}", to_string(codeword_descr.modulation), codeword_descr.rv);
  }
};

/// \brief Custom formatter for pdsch_processor::pdu_t.
template <>
struct formatter<srsgnb::pdsch_processor::pdu_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pdsch_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "slot={} rnti=0x{:04x} bwp={}:{} cp={} cw=[{}] n_id={} ports=[{}] ref_point={} dmrs_type={} "
                     "dmrs_mask=[{}] n_scidid={} n_scid={} ncgwd={} f_alloc={} t_alloc={}:{}",
                     pdu.slot,
                     pdu.rnti,
                     pdu.bwp_start_rb,
                     pdu.bwp_size_rb,
                     pdu.cp.to_string(),
                     srsgnb::span<const srsgnb::pdsch_processor::codeword_description>(pdu.codewords),
                     pdu.n_id,
                     srsgnb::span<const uint8_t>(pdu.ports),
                     pdu.ref_point == srsgnb::pdsch_processor::pdu_t::CRB0 ? "CRB0" : "PRB0",
                     (pdu.dmrs == srsgnb::dmrs_type::TYPE1) ? 1 : 2,
                     pdu.dmrs_symbol_mask,
                     pdu.scrambling_id,
                     pdu.n_scid,
                     pdu.nof_cdm_groups_without_data,
                     pdu.freq_alloc,
                     pdu.start_symbol_index,
                     pdu.nof_symbols);
  }
};

/// \brief Custom formatter for prach_detector::configuration.
template <>
struct formatter<srsgnb::prach_detector::configuration> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::prach_detector::configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "rsi={} format={} restricted={} zcz={} preambles={}:{}",
                     config.root_sequence_index,
                     config.format,
                     config.restricted_set,
                     config.zero_correlation_zone,
                     config.start_preamble_index,
                     config.nof_preamble_indices);
  }
};

/// \brief Custom formatter for prach_detector::configuration::preamble_indication.
template <>
struct formatter<srsgnb::prach_detection_result::preamble_indication> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::prach_detection_result::preamble_indication& preamble, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    format_to(ctx.out(),
              "idx={} ta={:.1f}us power={:+.1f}dB snr={:+.1f}dB",
              preamble.preamble_index,
              preamble.time_advance.to_seconds() * 1e6,
              preamble.power_dB,
              preamble.snr_dB);
    return ctx.out();
  }
};

/// \brief Custom formatter for prach_detector::configuration.
template <>
struct formatter<srsgnb::prach_detection_result> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::prach_detection_result& result, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    format_to(ctx.out(),
              "rssi={:+.1f}dB res={:.1f}us max_ta={:.1f}us preambles=[{}]",
              result.rssi_dB,
              result.time_resolution.to_seconds() * 1e6,
              result.time_advance_max.to_seconds() * 1e6,
              srsgnb::span<const srsgnb::prach_detection_result::preamble_indication>(result.preambles));
    return ctx.out();
  }
};

/// \brief Custom formatter for pucch_processor::format0_configuration.
template <>
struct formatter<srsgnb::pucch_processor::format0_configuration> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pucch_processor::format0_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "format0_configuration");
  }
};

/// \brief Custom formatter for pucch_processor::format1_configuration.
template <>
struct formatter<srsgnb::pucch_processor::format1_configuration> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pucch_processor::format1_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "slot={} bwp_size_rb={} bwp_start_rb={} cp={} starting_prb={} second_hop_prb={} n_id={} "
                     "nof_harq_ack={} ports=[{}] intial_cyclic_shift={} nof_symbols={} start_symbol_index={} "
                     "time_domain_occ={}",
                     config.slot,
                     config.bwp_size_rb,
                     config.bwp_start_rb,
                     config.cp.to_string(),
                     config.starting_prb,
                     config.second_hop_prb.has_value() ? config.second_hop_prb.value() : 0,
                     config.n_id,
                     config.nof_harq_ack,
                     srsgnb::span<const uint8_t>(config.ports),
                     config.initial_cyclic_shift,
                     config.nof_symbols,
                     config.start_symbol_index,
                     config.time_domain_occ);
  }
};

/// \brief Custom formatter for pucch_processor::format2_configuration.
template <>
struct formatter<srsgnb::pucch_processor::format2_configuration> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pucch_processor::format2_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(
        ctx.out(),
        "slot={} cp={} ports=[{}] bwp_size_rb={} bwp_start_rb={} starting_prb={} nof_prb={} start_symbol_index={} "
        "nof_symbols={} rnti={} n_id={} n_id0={} nof_harq_ack={} nof_sr={} nof_csi_part1={} nof_csi_part2={}",
        config.slot,
        config.cp.to_string(),
        srsgnb::span<const uint8_t>(config.ports),
        config.bwp_size_rb,
        config.bwp_start_rb,
        config.starting_prb,
        config.nof_prb,
        config.start_symbol_index,
        config.nof_symbols,
        config.rnti,
        config.n_id,
        config.n_id_0,
        config.nof_harq_ack,
        config.nof_sr,
        config.nof_csi_part1,
        config.nof_csi_part2);
  }
};

/// \brief Custom formatter for pucch_processor::format3_configuration.
template <>
struct formatter<srsgnb::pucch_processor::format3_configuration> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pucch_processor::format3_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "format3_configuration");
  }
};

/// \brief Custom formatter for pucch_processor::format4_configuration.
template <>
struct formatter<srsgnb::pucch_processor::format4_configuration> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pucch_processor::format4_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "format4_configuration");
  }
};

/// \brief Custom formatter for pucch_processor_result.
template <>
struct formatter<srsgnb::pucch_processor_result> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pucch_processor_result& result, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "pucch_processor_result");
  }
};

/// \brief Custom formatter for optional<pusch_processor::codeword_description>.
template <>
struct formatter<srsgnb::optional<srsgnb::pusch_processor::codeword_description>> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::optional<srsgnb::pusch_processor::codeword_description>& codeword, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (!codeword.has_value()) {
      return format_to(ctx.out(), "{{na}}");
    }
    return format_to(ctx.out(),
                     "{{rv={} bg={} ndi={}}}",
                     codeword.value().rv,
                     codeword.value().ldpc_base_graph,
                     codeword.value().new_data);
  }
};

/// \brief Custom formatter for pusch_processor::uci_description.
template <>
struct formatter<srsgnb::pusch_processor::uci_description> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pusch_processor::uci_description& uci_desc, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "oack={} ocsi1={} ocsi2={} alpha={} betas=[{}, {}, {}]",
                     uci_desc.nof_harq_ack,
                     uci_desc.nof_csi_part1,
                     uci_desc.nof_csi_part2,
                     uci_desc.alpha_scaling,
                     uci_desc.beta_offset_harq_ack,
                     uci_desc.beta_offset_csi_part1,
                     uci_desc.beta_offset_csi_part2);
  }
};

/// \brief Custom formatter for pusch_processor::pdu_t.
template <>
struct formatter<srsgnb::pusch_processor::pdu_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pusch_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "slot={} rnti=0x{:04x} bwp={}:{} cp={} mod={} tcr={} cw={} uci={} n_id={} dmrs=[{}] n_scidid={} "
                     "n_scid={} ncgwd={} f_alloc={} t_alloc={}:{}",
                     pdu.slot,
                     pdu.rnti,
                     pdu.bwp_start_rb,
                     pdu.bwp_size_rb,
                     pdu.cp.to_string(),
                     to_string(pdu.mcs_descr.modulation),
                     pdu.mcs_descr.target_code_rate,
                     pdu.codeword,
                     pdu.uci,
                     pdu.n_id,
                     pdu.dmrs_symbol_mask,
                     pdu.scrambling_id,
                     pdu.n_scid,
                     pdu.nof_cdm_groups_without_data,
                     pdu.freq_alloc,
                     pdu.start_symbol_index,
                     pdu.nof_symbols);
  }
};

/// \brief Custom formatter for pusch_processor_result.
template <>
struct formatter<srsgnb::pusch_processor_result> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pusch_processor_result& result, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(), "pusch_processor_result");
  }
};

/// \brief Custom formatter for ssb_processor::pdu_t.
template <>
struct formatter<srsgnb::ssb_processor::pdu_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::ssb_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "slot={} pci={} beta={} ssb_idx={} L_max={} common_scs={} sc_offset={} offset_PointA={} "
                     "pattern={} payload=[{}] ports=[{}]",
                     pdu.slot,
                     pdu.phys_cell_id,
                     pdu.beta_pss,
                     pdu.ssb_idx,
                     pdu.L_max,
                     scs_to_khz(pdu.common_scs),
                     pdu.subcarrier_offset.value(),
                     pdu.offset_to_pointA.value(),
                     to_string(pdu.pattern_case),
                     srsgnb::span<const uint8_t>(pdu.bch_payload),
                     srsgnb::span<const uint8_t>(pdu.ports));
  }
};

} // namespace fmt
