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

#include "srsran/support/units.h"

namespace srsran {
namespace ecpri {

/// eCPRI message types.
enum class message_type : uint8_t { iq_data = 0x00, rt_control_data = 0x02 };

/// eCPRI common header.
struct common_header {
  /// Protocol revision.
  unsigned revision;
  /// Last packet flag.
  bool is_last_packet;
  /// Message type.
  message_type msg_type;
  /// Payload size.
  units::bytes payload_size;
};

/// eCPRI Real-Time control parameters.
struct realtime_control_parameters {
  /// Identifier of a series of Real-Time Control Data messages, as per eCPRI v2 Section 3.2.4.3.
  uint16_t rtc_id;
  /// Identifier of each message in a series of Real-Time Control Data messages, as per eCPRI v2 Section 3.2.4.3.
  uint16_t seq_id;
};

/// eCPRI IQ data parameters.
struct iq_data_parameters {
  /// Identifier of a series of IQ Data Transfer messages, as per eCPRI v2 Section 3.2.4.1.
  uint16_t pc_id;
  /// Identifier of each message in a series of IQ Data Transfer messages, as per eCPRI v2 Section 3.2.4.1.
  uint16_t seq_id;
};

} // namespace ecpri
} // namespace srsran
