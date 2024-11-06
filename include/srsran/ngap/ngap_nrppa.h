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

/// DL/UL UE Associated NRPPA Transport message.
struct ngap_ue_associated_nrppa_transport {
  ue_index_t  ue_index;
  byte_buffer routing_id;
  byte_buffer nrppa_pdu;
};

/// DL/UL Non UE Associated NRPPA Transport message.
struct ngap_non_ue_associated_nrppa_transport {
  byte_buffer routing_id;
  byte_buffer nrppa_pdu;
};

} // namespace srs_cu_cp
} // namespace srsran
