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
