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

/// \brief UE Transmit Power Control (TPC) command configuration for SRS.
///
/// The identifiers are inspired by TS38.331 Section as 6.3.2 (see field \e SRS-TPC-CommandConfig).
struct srs_tpc_command_config {
  /// The starting bit position of a block within the group DCI with SRS request fields (optional) and TPC commands. The
  /// value 1 of the field corresponds to the first/left most bit of format2-3. See TS 38.212, clause 7.3.1 and
  /// TS 38.213, clause 11.3. Values {1..31}.
  optional<uint8_t> starting_bit_of_format_2_3;
  /// Indicate whether or not an SRS Request will be included i.e. 0 means SRS request bits in the field is 0 bits
  /// and 1 means SRS request bits in the field is 2 bits. Values {0..1}.
  optional<uint8_t> field_type_format_2_3;
  /// The starting bit position of a block within the group DCI with SRS request fields (optional) and TPC commands for
  /// SUL. The value 1 of the field corresponds to the first/left most bit of format2-3. See TS 38.212, clause 7.3.1 and
  /// TS 38.213, clause 11.3. Values {1..31}.
  optional<uint8_t> starting_bit_of_format_2_3_sul;

  bool operator==(const srs_tpc_command_config& rhs) const
  {
    return std::tie(starting_bit_of_format_2_3, field_type_format_2_3, starting_bit_of_format_2_3_sul) ==
           std::tie(rhs.starting_bit_of_format_2_3, rhs.field_type_format_2_3, rhs.starting_bit_of_format_2_3_sul);
  }

  bool operator!=(const srs_tpc_command_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
