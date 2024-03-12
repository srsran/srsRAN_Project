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

#include "srsran/ran/cu_types.h"
#include "srsran/sdap/sdap.h"
#include "srsran/support/timers.h"
#include <memory>

/// This factory header file depends on the SDAP interfaces (see above include list). It is kept separate as clients of
/// the SDAP interfaces dont need to call factory methods.

namespace srsran {

namespace srs_cu_up {

struct sdap_entity_creation_message {
  uint32_t              ue_index;
  pdu_session_id_t      pdu_session_id;
  sdap_rx_sdu_notifier* rx_sdu_notifier;
};

/// Creates an instance of a SDAP interface.
std::unique_ptr<sdap_entity> create_sdap(sdap_entity_creation_message& msg);

} // namespace srs_cu_up

} // namespace srsran
