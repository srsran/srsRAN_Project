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

/// eCPRI protocol revision.
constexpr unsigned ECPRI_PROTOCOL_REVISION = 1U;

/// Size of the eCPRI common header.
constexpr units::bytes ECPRI_COMMON_HEADER_SIZE{4U};

/// Size of the header fields in a eCPRI IQ data packet.
constexpr units::bytes ECPRI_IQ_DATA_PACKET_FIELDS_SIZE{4U};

/// Size of the header fields in a eCPRI Real-Time control packet.
constexpr units::bytes ECPRI_REALTIME_CONTROL_PACKET_FIELDS_SIZE{4U};

} // namespace ecpri
} // namespace srsran
