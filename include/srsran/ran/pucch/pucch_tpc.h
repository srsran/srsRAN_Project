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

/// \brief UE Transmit Power Control (TPC) command configuration for PUCCH.
///
/// The identifiers are inspired by TS38.331 Section as 6.3.2 (see field \e PUCCH-TPC-CommandConfig).
struct pucch_tpc_command_config {
  /// An index determining the position of the first bit of TPC command (applicable to the SpCell) inside the DCI format
  /// 2-2 payload. Values {1..15}.
  /// The field is mandatory present if the PUCCH-TPC-CommandConfig is provided in the PDCCH-Config for the SpCell.
  /// Otherwise, the field is absent.
  optional<uint8_t> tpc_index_pcell;
  /// An index determining the position of the first bit of TPC command (applicable to the PUCCH SCell) inside the DCI
  /// format 2-2 payload. Values {1..15}.
  /// The field is mandatory present if the PUCCH-TPC-CommandConfig is provided in the PDCCH-Config for the PUCCH-SCell.
  /// The field is optionally present, need R, if the UE is configured with a PUCCH SCell in this cell group and if the
  /// PUCCH-TPC-CommandConfig is provided in the PDCCH-Config for the SpCell.
  optional<uint8_t> tpc_index_pucch_scell;

  bool operator==(const pucch_tpc_command_config& rhs) const
  {
    return std::tie(tpc_index_pcell, tpc_index_pucch_scell) == std::tie(rhs.tpc_index_pcell, rhs.tpc_index_pucch_scell);
  }

  bool operator!=(const pucch_tpc_command_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
