/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/du_types.h"
#include <cstdint>

namespace srsgnb {

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

} // namespace srsgnb
