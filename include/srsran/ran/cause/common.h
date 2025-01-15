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

#include <cstdint>

namespace srsran {

enum class cause_protocol_t : uint8_t {
  transfer_syntax_error = 0,
  abstract_syntax_error_reject,
  abstract_syntax_error_ignore_and_notify,
  msg_not_compatible_with_receiver_state,
  semantic_error,
  abstract_syntax_error_falsely_constructed_msg,
  unspecified
};

enum class cause_misc_t : uint8_t {
  ctrl_processing_overload = 0,
  not_enough_user_plane_processing_res,
  hardware_fail,
  om_intervention,
  unspecified
};

// Establishment cause

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
