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

#include "srsran/cu_cp/cu_cp_types.h"

namespace srsran {
namespace srs_cu_cp {

/// UE configuration passed to CU-CP
struct ue_configuration {
  std::chrono::seconds inactivity_timer{7200};
  /// Timeout for PDU session to be setup in seconds, before the UE is released.
  std::chrono::seconds pdu_session_setup_timeout = std::chrono::seconds{2};
};

} // namespace srs_cu_cp
} // namespace srsran
