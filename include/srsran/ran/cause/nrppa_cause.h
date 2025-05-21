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

#include "fmt/format.h"
#include <variant>

namespace srsran {

/// The NRPPa radio network cause, see TS 38.455 section 9.2.1.
enum class nrppa_cause_radio_network_t : uint8_t {
  unspecified = 0,
  requested_item_not_supported,
  requested_item_temporarily_not_available
};

/// The NRPPa protocol cause, see TS 38.455 section 9.2.1.
enum class nrppa_cause_protocol_t : uint8_t {
  transfer_syntax_error = 0,
  abstract_syntax_error_reject,
  abstract_syntax_error_ignore_and_notify,
  msg_not_compatible_with_receiver_state,
  semantic_error,
  unspecified,
  abstract_syntax_error_falsely_constructed_msg
};

/// The NRPPa misc cause, see TS 38.455 section 9.2.1.
enum class nrppa_cause_misc_t : uint8_t { unspecified = 0 };

/// The NRPPa cause to indicate the reason for a particular event, see TS 38.455 section 9.2.1.
/// The NRPPa cause is a union of the radio network cause, protocol cause and misc cause.
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
