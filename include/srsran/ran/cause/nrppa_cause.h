/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "fmt/format.h"
#include <variant>

namespace srsran {

enum class nrppa_cause_radio_network_t : uint8_t {
  unspecified = 0,
  requested_item_not_supported,
  requested_item_temporarily_not_available
};

enum class nrppa_cause_protocol_t : uint8_t {
  transfer_syntax_error = 0,
  abstract_syntax_error_reject,
  abstract_syntax_error_ignore_and_notify,
  msg_not_compatible_with_receiver_state,
  semantic_error,
  unspecified,
  abstract_syntax_error_falsely_constructed_msg
};

enum class nrppa_cause_misc_t : uint8_t { unspecified = 0 };

using nrppa_cause_t = std::variant<nrppa_cause_radio_network_t, nrppa_cause_protocol_t, nrppa_cause_misc_t>;

} // namespace srsran

namespace fmt {

// nrppa_cause_t formatter
template <>
struct formatter<srsran::nrppa_cause_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::nrppa_cause_t o, FormatContext& ctx)
  {
    if (const auto* result = std::get_if<srsran::nrppa_cause_radio_network_t>(&o)) {
      return format_to(ctx.out(), "radio_network-id{}", *result);
    }
    if (const auto* result = std::get_if<srsran::nrppa_cause_protocol_t>(&o)) {
      return format_to(ctx.out(), "protocol-id{}", *result);
    }
    return format_to(ctx.out(), "misc-id{}", std::get<srsran::nrppa_cause_misc_t>(o));
  }
};

} // namespace fmt
