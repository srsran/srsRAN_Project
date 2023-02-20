/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
