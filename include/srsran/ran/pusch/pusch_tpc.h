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

#include "srsran/adt/optional.h"
#include "srsran/ran/du_types.h"
#include <cstdint>

namespace srsran {

/// \brief UE Transmit Power Control (TPC) command configuration for PUSCH.
///
/// The identifiers are inspired by TS38.331 Section as 6.3.2 (see field \e PUSCH-TPC-CommandConfig).
struct pusch_tpc_command_config {
  /// An index determining the position of the first bit of TPC command inside the DCI format 2-2 payload.
  /// Values {1..15}. Field is optionally present if this serving cell is configured with a supplementary uplink (SUL).
  /// It is mandatory present otherwise.
  optional<uint8_t> tpc_index;
  /// An index determining the position of the first bit of TPC command inside the DCI format 2-2 payload.
  /// Values {1..15}. Present only if serving cell is configured with a supplementary uplink (SUL).
  optional<uint8_t> tpc_index_sul;
  /// The serving cell to which the acquired power control commands are applicable. If the value is absent, the UE
  /// applies the TPC commands to the serving cell on which the command has been received.
  optional<du_cell_index_t> target_cell;

  bool operator==(const pusch_tpc_command_config& rhs) const
  {
    return std::tie(tpc_index, tpc_index_sul, target_cell) ==
           std::tie(rhs.tpc_index, rhs.tpc_index_sul, rhs.target_cell);
  }

  bool operator!=(const pusch_tpc_command_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
