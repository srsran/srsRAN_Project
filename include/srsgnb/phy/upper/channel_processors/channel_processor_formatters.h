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
#include "srsgnb/srsvec/copy.h"
namespace srsgnb {
namespace detail {

/// \brief Format helper used to insert delimiters between structure fields.
///
/// This class is used to format the structures used by the channel processors. Its methods can be called from an \c fmt
/// custom formatter specialization to aid in format argument parsing and formatting of the structure fields. It
/// automatically inserts the chosen delimiter between the formatted fields.
/// \note The default delimiter is a space character, however, a new line delimiter can be selected by formatting with
/// the \c n format specifier, as in <tt>{:n}</tt>.
/// \note A short formatting mode can be selected by formatting with the \c s format specifier, as in <tt>{:s}</tt>.
/// This option can be used to exclude some of the structure fields for a more compact representation.
class delimited_formatter
{
public:
  /// Default constructor.
  delimited_formatter()
  {
    static const fmt::string_view DEFAULT_FORMAT    = "{}";
    static const fmt::string_view DEFAULT_DELIMITER = " ";
    format_buffer.append(DEFAULT_FORMAT.begin(), DEFAULT_FORMAT.end());
    delimiter_buffer.append(DEFAULT_DELIMITER.begin(), DEFAULT_DELIMITER.end());
  }

  /// \brief Parsing helper for format specifiers.
  ///
  /// This helper detects the \c s and \c n short mode and new line delimiter specifiers. It also generates a format
  /// string with the parsed specifiers that can be used to propagate the formatting options to nested structures or
  /// structure fields.
  ///
  /// \tparam ParseContext Parse context type.
  /// \param[in] context A character range including the format specifiers.
  /// \return An iterator at the end of the parsed range.
  template <typename ParseContext>
  auto parse(ParseContext& context)
  {
    // Set the first field indicator.
    first = true;

    static const fmt::string_view PREAMBLE_FORMAT   = "{:";
    static const fmt::string_view NEWLINE_DELIMITER = "\n  ";

    // Skip if context is empty and use default format.
    if (context.begin() == context.end()) {
      return context.end();
    }

    format_buffer.clear();
    format_buffer.append(PREAMBLE_FORMAT.begin(), PREAMBLE_FORMAT.end());

    for (auto& it : context) {
      switch (it) {
        case 'n':
          // New line delimiter.
          delimiter_buffer.clear();
          delimiter_buffer.append(NEWLINE_DELIMITER.begin(), NEWLINE_DELIMITER.end());
          break;
        case 's':
          // Short representation.
          verbose = false;
          break;
        case '}':
          format_buffer.push_back(it);
          return &it;
      }
      // Propagate formatting to underlying structures.
      format_buffer.push_back(it);
    }
    // No end of context was found.
    return context.end();
  }

  /// \brief Formats one or more fields with the provided formatting options.
  ///
  /// Generates a string representation of the structure fields, according to the provided \c format formatting string,
  /// and writes it into the output iterator of the formatting \c context.
  ///
  /// \note The fields formatted with this method will be present in the default and the short form structure
  /// representations.
  ///
  /// \tparam FormatContext Format context type.
  /// \tparam Args Formatting arguments parameter pack.
  /// \param[in] context Formatting context, including an output iterator used to write the formatted representation.
  /// \param[in] format Formatting string, containing format specifiers.
  /// \param[in] args Fields to be formatted.
  template <typename FormatContext, typename... Args>
  void format_always(FormatContext& context, const char* format, Args&&... args)
  {
    this->format_to(context, format, std::forward<Args>(args)...);
  }

  /// \brief Formats a single field or nested structure with the parsed formatting options.
  ///
  /// Generates a string representation of the structure field, according to the previously parsed formatting string,
  /// and writes it into the output iterator of the formatting \c context. It can be used to propagate the parsed
  /// formatting options to nested structures.
  ///
  /// \note The fields formatted with this method will be present in the default and the short form structure
  /// representations.
  ///
  /// \tparam FormatContext Format context type.
  /// \tparam Args Formatting arguments parameter pack.
  /// \param[in] context Formatting context, including an output iterator used to write the formatted representation.
  /// \param[in] args Fields to be formatted.
  template <typename FormatContext, typename... Args>
  void format_always(FormatContext& context, Args&&... args)
  {
    this->format_to(context, std::forward<Args>(args)...);
  }

  /// \brief Formats one or more fields with the provided formatting options.
  ///
  /// Generates a string representation of the structure fields, according to the provided \c format formatting string,
  /// and writes it into the output iterator of the formatting \c context.
  ///
  /// \note The fields formatted with this method will not be present in the short form structure representation.
  ///
  /// \tparam FormatContext Format context type.
  /// \tparam Args Formatting arguments parameter pack.
  /// \param[in] context Formatting context, including an output iterator used to write the formatted representation.
  /// \param[in] format Formatting string, containing format specifiers.
  /// \param[in] args Fields to be formatted.
  template <typename FormatContext, typename... Args>
  void format_if_verbose(FormatContext& context, const char* format, Args&&... args)
  {
    if (verbose) {
      this->format_to(context, format, std::forward<Args>(args)...);
    }
  }

  /// \brief Formats a single field or nested structure with the parsed formatting options.
  ///
  /// Generates a string representation of the structure field, according to the previously parsed formatting string,
  /// and writes it into the output iterator of the formatting \c context. It can be used to propagate the parsed
  /// formatting options to nested structures.
  ///
  /// \note The fields formatted with this method will not be present in the short form structure representation.
  ///
  /// \tparam FormatContext Format context type.
  /// \tparam Args Formatting arguments parameter pack.
  /// \param[in] context Formatting context, including an output iterator used to write the formatted representation.
  /// \param[in] args Fields to be formatted.
  template <typename FormatContext, typename... Args>
  void format_if_verbose(FormatContext& context, Args&&... args)
  {
    if (verbose) {
      this->format_to(context, std::forward<Args>(args)...);
    }
  }

private:
  /// Internal method used to format with any formatting options.
  template <typename FormatContext, typename... Args>
  void format_to(FormatContext& context, const char* format, Args&&... args)
  {
    if (!first) {
      // Buffer to hold the formatted string.
      fmt::memory_buffer temp_buffer;
      fmt::format_to(temp_buffer, format, std::forward<Args>(args)...);

      if (temp_buffer.size() > 0) {
        // Prepend delimiter to the formatted output.
        fmt::format_to(context.out(), "{}", fmt::string_view(delimiter_buffer.data(), delimiter_buffer.size()));
      }

      // Append the formatted string to the context iterator.
      fmt::format_to(context.out(), "{}", fmt::string_view(temp_buffer.data(), temp_buffer.size()));

      return;
    }
    // Format without prepending delimiter.
    fmt::format_to(context.out(), format, std::forward<Args>(args)...);
    first = false;
  }

  /// Internal method used to format with the parsed formatting options.
  template <typename FormatContext, typename... Args>
  void format_to(FormatContext& context, Args&&... args)
  {
    if (!first) {
      // Buffer to hold the formatted string.
      fmt::memory_buffer temp_buffer;
      fmt::format_to(
          temp_buffer, fmt::string_view(format_buffer.data(), format_buffer.size()), std::forward<Args>(args)...);

      if (temp_buffer.size() > 0) {
        // Prepend delimiter to the formatted output.
        fmt::format_to(context.out(), "{}", fmt::string_view(delimiter_buffer.data(), delimiter_buffer.size()));
      }

      // Append the formatted string to the context iterator.
      fmt::format_to(context.out(), "{}", fmt::string_view(temp_buffer.data(), temp_buffer.size()));

      return;
    }
    // Format without prepending delimiter.
    fmt::format_to(
        context.out(), fmt::string_view(format_buffer.data(), format_buffer.size()), std::forward<Args>(args)...);
    first = false;
  }

  /// First field flag. It is used to determine when to insert delimiters between fields.
  bool first = true;
  /// Verbose flag. Verbose format option includes all the fields of the structure in the formatted output.
  bool verbose = true;

  /// Delimiter string.
  fmt::memory_buffer delimiter_buffer;
  /// Format string, used to propagate formatting options to nested structures.
  fmt::memory_buffer format_buffer;
};
} // namespace detail
} // namespace srsgnb

namespace fmt {

/// \brief Custom formatter for \c pdcch_processor::coreset_description.
template <>
struct formatter<srsgnb::pdcch_processor::coreset_description> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pdcch_processor::coreset_description& coreset, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "bwp=[{}, {})", coreset.bwp_start_rb, coreset.bwp_start_rb + coreset.bwp_size_rb);
    helper.format_always(
        ctx, "symbols=[{}, {})", coreset.start_symbol_index, coreset.start_symbol_index + coreset.duration);
    helper.format_always(ctx, "f_re={}", coreset.frequency_resources);

    switch (coreset.cce_to_reg_mapping) {
      case srsgnb::pdcch_processor::cce_to_reg_mapping_type::CORESET0:
        helper.format_if_verbose(ctx, "mapping=coreset0");
        break;
      case srsgnb::pdcch_processor::cce_to_reg_mapping_type::INTERLEAVED:
        helper.format_if_verbose(ctx, "mapping=interleaved");
        helper.format_if_verbose(ctx, "reg_bundle_size={}", coreset.reg_bundle_size);
        helper.format_if_verbose(ctx, "interleaver_size={}", coreset.interleaver_size);
        break;
      case srsgnb::pdcch_processor::cce_to_reg_mapping_type::NON_INTERLEAVED:
        helper.format_if_verbose(ctx, "mapping=non-interleaved");
        break;
    }

    helper.format_if_verbose(ctx, "shift_idx={}", coreset.shift_index);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pdcch_processor::dci_description.
template <>
struct formatter<srsgnb::pdcch_processor::dci_description> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pdcch_processor::dci_description& dci, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "rnti=0x{:04x}", dci.rnti);
    helper.format_always(ctx, "size={}", dci.payload.size());

    helper.format_if_verbose(ctx, "n_id_dmrs={}", dci.n_id_pdcch_dmrs);
    helper.format_if_verbose(ctx, "n_id_data={}", dci.n_id_pdcch_data);
    helper.format_if_verbose(ctx, "n_rnti={}", dci.n_rnti);
    helper.format_if_verbose(ctx, "cce={}", dci.cce_index);
    helper.format_if_verbose(ctx, "al={}", dci.aggregation_level);
    helper.format_if_verbose(ctx, "power_dmrs={:+.1f}dB", dci.dmrs_power_offset_dB);
    helper.format_if_verbose(ctx, "power_data={:+.1f}dB", dci.data_power_offset_dB);
    helper.format_if_verbose(ctx, "ports={}", srsgnb::span<const uint8_t>(dci.ports));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pdcch_processor::pdu_t.
template <>
struct formatter<srsgnb::pdcch_processor::pdu_t> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pdcch_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, pdu.coreset);
    helper.format_always(ctx, pdu.dci);

    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "cp={}", pdu.cp.to_string());
    return ctx.out();
  }
};

/// \brief Custom formatter for \c pdsch_processor::codeword_description.
template <>
struct formatter<srsgnb::pdsch_processor::codeword_description> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pdsch_processor::codeword_description& codeword_descr, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "mod={}", to_string(codeword_descr.modulation));
    helper.format_always(ctx, "rv={}", codeword_descr.rv);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pdsch_processor::pdu_t.
template <>
struct formatter<srsgnb::pdsch_processor::pdu_t> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pdsch_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "rnti=0x{:04x}", pdu.rnti);
    helper.format_always(ctx, "bwp=[{}, {})", pdu.bwp_start_rb, pdu.bwp_start_rb + pdu.bwp_size_rb);
    helper.format_always(ctx, "prb={}", pdu.freq_alloc);
    helper.format_always(ctx, "symbols=[{}, {})", pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols);
    helper.format_always(ctx, srsgnb::span<const srsgnb::pdsch_processor::codeword_description>(pdu.codewords));

    helper.format_if_verbose(ctx, "n_id={}", pdu.n_id);
    helper.format_if_verbose(
        ctx, "ref_point={}", (pdu.ref_point == srsgnb::pdsch_processor::pdu_t::CRB0) ? "CRB0" : "PRB0");
    helper.format_if_verbose(ctx, "dmrs_type={}", (pdu.dmrs == srsgnb::dmrs_type::TYPE1) ? 1 : 2);
    helper.format_if_verbose(ctx, "dmrs_mask={}", pdu.dmrs_symbol_mask);
    helper.format_if_verbose(ctx, "n_scidid={}", pdu.scrambling_id);
    helper.format_if_verbose(ctx, "n_scid={}", pdu.n_scid);
    helper.format_if_verbose(ctx, "ncgwd={}", pdu.nof_cdm_groups_without_data);
    helper.format_if_verbose(ctx, "bg={}", (pdu.ldpc_base_graph == srsgnb::ldpc_base_graph_type::BG1) ? "BG1" : "BG2");
    helper.format_if_verbose(ctx, "tbs_lbrm={}bytes", pdu.tbs_lbrm_bytes);
    helper.format_if_verbose(ctx, "power_dmrs={:+.1f}dB", pdu.ratio_pdsch_dmrs_to_sss_dB);
    helper.format_if_verbose(ctx, "power_data={:+.1f}dB", pdu.ratio_pdsch_data_to_sss_dB);
    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "cp={}", pdu.cp.to_string());
    helper.format_if_verbose(ctx, "ports={}", srsgnb::span<const uint8_t>(pdu.ports));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c prach_detector::configuration.
template <>
struct formatter<srsgnb::prach_detector::configuration> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::prach_detector::configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "rsi={}", config.root_sequence_index);
    helper.format_always(ctx,
                         "preambles=[{}, {})",
                         config.start_preamble_index,
                         config.start_preamble_index + config.nof_preamble_indices);

    helper.format_if_verbose(ctx, "format={}", config.format);
    helper.format_if_verbose(ctx, "set={}", to_string(config.restricted_set));
    helper.format_if_verbose(ctx, "zcz={}", config.zero_correlation_zone);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c prach_detection_result::preamble_indication.
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
              "{{idx={} ta={:.1f}us power={:+.1f}dB snr={:+.1f}dB}}",
              preamble.preamble_index,
              preamble.time_advance.to_seconds() * 1e6,
              preamble.power_dB,
              preamble.snr_dB);
    return ctx.out();
  }
};

/// \brief Custom formatter for \c prach_detection_result.
template <>
struct formatter<srsgnb::prach_detection_result> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::prach_detection_result& result, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "rssi={:+.1f}dB", result.rssi_dB);
    helper.format_if_verbose(ctx, "res={:.1f}us", result.time_resolution.to_seconds() * 1e6);
    helper.format_if_verbose(ctx, "max_ta={:.1f}us", result.time_advance_max.to_seconds() * 1e6);
    helper.format_always(ctx,
                         "det_preambles=[{:,}]",
                         srsgnb::span<const srsgnb::prach_detection_result::preamble_indication>(result.preambles));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pucch_processor::format0_configuration.
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

/// \brief Custom formatter for \c pucch_processor::format1_configuration.
template <>
struct formatter<srsgnb::pucch_processor::format1_configuration> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pucch_processor::format1_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "format=1");
    helper.format_always(ctx, "bwp=[{}, {})", config.bwp_start_rb, config.bwp_start_rb + config.bwp_size_rb);
    helper.format_always(ctx, "prb1={}", config.starting_prb);
    helper.format_always(
        ctx, "prb2={}", config.second_hop_prb.has_value() ? std::to_string(config.second_hop_prb.value()) : "na");
    helper.format_always(
        ctx, "symbols=[{}, {})", config.start_symbol_index, config.start_symbol_index + config.nof_symbols);

    helper.format_if_verbose(ctx, "n_id={}", config.n_id);
    helper.format_if_verbose(ctx, "intial_cs={}", config.initial_cyclic_shift);
    helper.format_if_verbose(ctx, "occi={}", config.time_domain_occ);
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_if_verbose(ctx, "cp={}", config.cp.to_string());
    helper.format_if_verbose(ctx, "ports={}", srsgnb::span<const uint8_t>(config.ports));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pucch_processor::format2_configuration.
template <>
struct formatter<srsgnb::pucch_processor::format2_configuration> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pucch_processor::format2_configuration& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "format=2");
    helper.format_always(ctx, "rnti=0x{:04x}", config.rnti);
    helper.format_always(ctx, "bwp=[{}, {})", config.bwp_start_rb, config.bwp_start_rb + config.bwp_size_rb);
    helper.format_always(ctx, "prb=[{}, {})", config.starting_prb, config.starting_prb + config.nof_prb);
    helper.format_always(
        ctx, "prb2={}", config.second_hop_prb.has_value() ? std::to_string(config.second_hop_prb.value()) : "na");
    helper.format_always(
        ctx, "symbols=[{}, {})", config.start_symbol_index, config.start_symbol_index + config.nof_symbols);

    helper.format_if_verbose(ctx, "n_id={}", config.n_id);
    helper.format_if_verbose(ctx, "n_id0={}", config.n_id_0);
    helper.format_if_verbose(ctx, "slot={}", config.slot);
    helper.format_if_verbose(ctx, "cp={}", config.cp.to_string());
    helper.format_if_verbose(ctx, "ports={}", srsgnb::span<const uint8_t>(config.ports));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pucch_processor::format3_configuration.
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

/// \brief Custom formatter for \c pucch_processor::format4_configuration.
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

/// \brief Custom formatter for \c pucch_processor_result.
template <>
struct formatter<srsgnb::pucch_processor_result> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pucch_processor_result& result, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "status={}", to_string(result.message.get_status()));

    unsigned nof_sr        = result.message.get_expected_nof_sr_bits();
    unsigned nof_harq_ack  = result.message.get_expected_nof_harq_ack_bits();
    unsigned nof_csi_part1 = result.message.get_expected_nof_csi_part1_bits();
    unsigned nof_csi_part2 = result.message.get_expected_nof_csi_part2_bits();

    if (result.message.get_status() == srsgnb::uci_status::valid) {
      // Valid UCI payload.
      if (nof_harq_ack) {
        helper.format_always(ctx, "ack={:#}", srsgnb::span<const uint8_t>(result.message.get_harq_ack_bits()));
      }
      if (nof_sr) {
        helper.format_always(ctx, "sr={:#}", srsgnb::span<const uint8_t>(result.message.get_sr_bits()));
      }
      if (nof_csi_part1) {
        helper.format_always(ctx, "csi1={:#}", srsgnb::span<const uint8_t>(result.message.get_csi_part1_bits()));
      }
      if (nof_csi_part2) {
        helper.format_always(ctx, "csi2={:#}", srsgnb::span<const uint8_t>(result.message.get_csi_part2_bits()));
      }
    } else {
      // Bad UCI payload.
      std::array<uint8_t, srsgnb::uci_constants::MAX_NOF_PAYLOAD_BITS> bad_payload;
      std::fill(bad_payload.begin(), bad_payload.end(), 2U);

      if (nof_harq_ack) {
        helper.format_always(ctx, "ack={:#}", srsgnb::span<const uint8_t>(bad_payload).first(nof_harq_ack));
      }
      if (nof_sr) {
        helper.format_always(ctx, "sr={:#}", srsgnb::span<const uint8_t>(bad_payload).first(nof_sr));
      }
      if (nof_csi_part1) {
        helper.format_always(ctx, "csi1={:#}", srsgnb::span<const uint8_t>(bad_payload).first(nof_csi_part1));
      }
      if (nof_csi_part2) {
        helper.format_always(ctx, "csi2={:#}", srsgnb::span<const uint8_t>(bad_payload).first(nof_csi_part2));
      }
    }

    // Channel State Information.
    helper.format_if_verbose(ctx, "epre={:+.1f}dB", result.csi.epre_dB);
    helper.format_if_verbose(ctx, "rsrp={:+.1f}dB", result.csi.rsrp_dB);
    helper.format_if_verbose(ctx, "sinr={:+.1f}dB", result.csi.sinr_dB);
    helper.format_if_verbose(ctx, "t_align={:.1f}us", result.csi.time_alignment.to_seconds() * 1e6);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pusch_processor::codeword_description.
template <>
struct formatter<srsgnb::pusch_processor::codeword_description> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::optional<srsgnb::pusch_processor::codeword_description>& codeword, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    helper.format_always(ctx, "rv={}", codeword.value().rv);
    helper.format_if_verbose(ctx, "bg={}", codeword.value().ldpc_base_graph);
    helper.format_always(ctx, "ndi={}", codeword.value().new_data);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pusch_processor::uci_description.
template <>
struct formatter<srsgnb::pusch_processor::uci_description> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pusch_processor::uci_description& uci_desc, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    // Number of ACK, CSI-Part1 and CSI-Part2 bits.
    helper.format_if_verbose(ctx, "oack={}", uci_desc.nof_harq_ack);
    helper.format_if_verbose(ctx, "ocsi1={}", uci_desc.nof_csi_part1);
    helper.format_if_verbose(ctx, "ocsi2={}", uci_desc.nof_csi_part2);

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
struct formatter<srsgnb::pusch_processor::pdu_t> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pusch_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    // Fields that are always printed.
    helper.format_always(ctx, "rnti=0x{:04x}", pdu.rnti);
    helper.format_always(ctx, "bwp=[{}, {})", pdu.bwp_start_rb, pdu.bwp_start_rb + pdu.bwp_size_rb);
    helper.format_always(ctx, "prb={}", pdu.freq_alloc);
    helper.format_always(ctx, "symbols=[{}, {})", pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols);

    // UCI description.
    helper.format_always(ctx, pdu.uci);

    // Verbose parameters.
    helper.format_if_verbose(ctx, "mod={}", to_string(pdu.mcs_descr.modulation));
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
    helper.format_if_verbose(ctx, "dmrs_type={}", (pdu.dmrs == srsgnb::dmrs_type::TYPE1) ? 1 : 2);
    helper.format_if_verbose(ctx, "tbs_lbrm={}bytes", pdu.tbs_lbrm_bytes);
    helper.format_if_verbose(ctx, "slot={}", pdu.slot);
    helper.format_if_verbose(ctx, "cp={}", pdu.cp.to_string());
    helper.format_if_verbose(ctx, "nof_layers={}", pdu.nof_tx_layers);
    helper.format_if_verbose(ctx, "ports={}", srsgnb::span<const uint8_t>(pdu.rx_ports));

    return ctx.out();
  }
};

/// \brief Custom formatter for \c pusch_decoder_result.
template <>
struct formatter<srsgnb::pusch_decoder_result> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pusch_decoder_result& result, FormatContext& ctx)
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

/// \brief Custom formatter for \c pusch_processor_result.
template <>
struct formatter<srsgnb::pusch_processor_result> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::pusch_processor_result& result, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    // Generate bad payload, to use in case of invalid or unknown UCI status.
    std::array<uint8_t, srsgnb::uci_constants::MAX_NOF_PAYLOAD_BITS> bad_payload;
    std::fill(bad_payload.begin(), bad_payload.end(), 2U);

    // PUSCH decoder result.
    if (result.data.has_value()) {
      helper.format_always(ctx, result.data.value());
    }
    if (result.evm.has_value()) {
      helper.format_always(ctx, "evm={:.1f}%", result.evm.value() * 100.0F);
    }
    if ((!result.harq_ack.payload.empty())) {
      if (result.harq_ack.status == srsgnb::uci_status::valid) {
        helper.format_always(ctx, "ack={:#}", srsgnb::span<const uint8_t>(result.harq_ack.payload));
      } else {
        helper.format_always(
            ctx, "ack={:#}", srsgnb::span<const uint8_t>(bad_payload).first(result.harq_ack.payload.size()));
      }
    }
    if ((!result.csi_part1.payload.empty())) {
      if (result.csi_part1.status == srsgnb::uci_status::valid) {
        helper.format_always(ctx, "csi1={:#}", srsgnb::span<const uint8_t>(result.csi_part1.payload));
      } else {
        helper.format_always(
            ctx, "csi1={:#}", srsgnb::span<const uint8_t>(bad_payload).first(result.csi_part1.payload.size()));
      }
    }
    if ((!result.csi_part2.payload.empty())) {
      if (result.csi_part2.status == srsgnb::uci_status::valid) {
        helper.format_always(ctx, "csi2={:#}", srsgnb::span<const uint8_t>(result.csi_part2.payload));
      } else {
        helper.format_always(
            ctx, "csi2={:#}", srsgnb::span<const uint8_t>(bad_payload).first(result.csi_part2.payload.size()));
      }
    }

    // Channel State Information.
    helper.format_if_verbose(ctx, "epre={:+.1f}dB", result.csi.epre_dB);
    helper.format_if_verbose(ctx, "rsrp={:+.1f}dB", result.csi.rsrp_dB);
    helper.format_if_verbose(ctx, "sinr={:+.1f}dB", result.csi.sinr_dB);
    helper.format_if_verbose(ctx, "t_align={:.1f}us", result.csi.time_alignment.to_seconds() * 1e6);

    return ctx.out();
  }
};

/// \brief Custom formatter for \c ssb_processor::pdu_t.
template <>
struct formatter<srsgnb::ssb_processor::pdu_t> {
  /// Helper used to parse formatting options and format fields.
  srsgnb::detail::delimited_formatter helper;

  /// Default constructor.
  formatter() = default;

  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return helper.parse(ctx);
  }

  template <typename FormatContext>
  auto format(const srsgnb::ssb_processor::pdu_t& pdu, FormatContext& ctx)
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
    helper.format_if_verbose(ctx, "ports={}", srsgnb::span<const uint8_t>(pdu.ports));

    return ctx.out();
  }
};

} // namespace fmt
