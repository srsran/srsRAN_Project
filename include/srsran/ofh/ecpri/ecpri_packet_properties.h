/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
