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
#include <cstdint>

namespace srsran {

/// The protocol cause.
/// For E1AP see TS 38.463 section 9.3.1.2.
/// For F1AP see TS 38.473 section 9.3.1.2.
/// For NGAP see TS 38.413 section 9.3.1.2.
enum class cause_protocol_t : uint8_t {
  transfer_syntax_error = 0,
  abstract_syntax_error_reject,
  abstract_syntax_error_ignore_and_notify,
  msg_not_compatible_with_receiver_state,
  semantic_error,
  abstract_syntax_error_falsely_constructed_msg,
  unspecified
};

/// The misc cause.
/// For E1AP see TS 38.463 section 9.3.1.2.
/// For F1AP see TS 38.473 section 9.3.1.2.
enum class cause_misc_t : uint8_t {
  ctrl_processing_overload = 0,
  not_enough_user_plane_processing_res,
  hardware_fail,
  om_intervention,
  unspecified
};

/// Provides the establishment cause for the RRCSetupRequest in accordance with the information received from upper
/// layers, see TS 38.331 section 6.2.2.
enum class establishment_cause_t : uint8_t {
  emergency = 0,
  high_prio_access,
  mt_access,
  mo_sig,
  mo_data,
  mo_voice_call,
  mo_video_call,
  mo_sms,
  mps_prio_access,
  mcs_prio_access
};

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::cause_protocol_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::cause_protocol_t& cause, FormatContext& ctx) const
  {
    if (cause == srsran::cause_protocol_t::transfer_syntax_error) {
      return format_to(ctx.out(), "transfer_syntax_error");
    }
    if (cause == srsran::cause_protocol_t::abstract_syntax_error_reject) {
      return format_to(ctx.out(), "abstract_syntax_error_reject");
    }
    if (cause == srsran::cause_protocol_t::abstract_syntax_error_ignore_and_notify) {
      return format_to(ctx.out(), "abstract_syntax_error_ignore_and_notify");
    }
    if (cause == srsran::cause_protocol_t::msg_not_compatible_with_receiver_state) {
      return format_to(ctx.out(), "msg_not_compatible_with_receiver_state");
    }
    if (cause == srsran::cause_protocol_t::semantic_error) {
      return format_to(ctx.out(), "semantic_error");
    }
    if (cause == srsran::cause_protocol_t::abstract_syntax_error_falsely_constructed_msg) {
      return format_to(ctx.out(), "abstract_syntax_error_falsely_constructed_msg");
    }
    if (cause == srsran::cause_protocol_t::unspecified) {
      return format_to(ctx.out(), "unspecified");
    }

    return format_to(ctx.out(), "unknown");
  }
};

template <>
struct formatter<srsran::cause_misc_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::cause_misc_t& cause, FormatContext& ctx) const
  {
    if (cause == srsran::cause_misc_t::ctrl_processing_overload) {
      return format_to(ctx.out(), "ctrl_processing_overload");
    }
    if (cause == srsran::cause_misc_t::not_enough_user_plane_processing_res) {
      return format_to(ctx.out(), "not_enough_user_plane_processing_res");
    }
    if (cause == srsran::cause_misc_t::hardware_fail) {
      return format_to(ctx.out(), "hardware_fail");
    }
    if (cause == srsran::cause_misc_t::om_intervention) {
      return format_to(ctx.out(), "om_intervention");
    }
    if (cause == srsran::cause_misc_t::unspecified) {
      return format_to(ctx.out(), "unspecified");
    }

    return format_to(ctx.out(), "unknown");
  }
};

template <>
struct formatter<srsran::establishment_cause_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(srsran::establishment_cause_t o, FormatContext& ctx) const
  {
    if (o == srsran::establishment_cause_t::emergency) {
      return format_to(ctx.out(), "emergency");
    }
    if (o == srsran::establishment_cause_t::high_prio_access) {
      return format_to(ctx.out(), "high_prio_access");
    }
    if (o == srsran::establishment_cause_t::mt_access) {
      return format_to(ctx.out(), "mt_access");
    }
    if (o == srsran::establishment_cause_t::mo_sig) {
      return format_to(ctx.out(), "mo_sig");
    }
    if (o == srsran::establishment_cause_t::mo_data) {
      return format_to(ctx.out(), "mo_data");
    }
    if (o == srsran::establishment_cause_t::mo_voice_call) {
      return format_to(ctx.out(), "mo_voice_call");
    }
    if (o == srsran::establishment_cause_t::mo_video_call) {
      return format_to(ctx.out(), "mo_video_call");
    }
    if (o == srsran::establishment_cause_t::mo_sms) {
      return format_to(ctx.out(), "mo_sms");
    }
    if (o == srsran::establishment_cause_t::mps_prio_access) {
      return format_to(ctx.out(), "mps_prio_access");
    }
    if (o == srsran::establishment_cause_t::mcs_prio_access) {
      return format_to(ctx.out(), "mcs_prio_access");
    }

    return format_to(ctx.out(), "unknown");
  }
};

} // namespace fmt
