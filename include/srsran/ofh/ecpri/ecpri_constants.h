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
