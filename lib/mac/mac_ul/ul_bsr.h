/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/adt/to_array.h"
#include "srsran/mac/bsr_format.h"

namespace srsran {

/// UL BSR Report for a single Logical Channel Group
struct lcg_bsr_report {
  lcg_id_t lcg_id;
  uint8_t  buffer_size;
};

/// List of reported UL BSRs
using lcg_bsr_report_list = static_vector<lcg_bsr_report, MAX_NOF_LCGS>;

/// Decode Short BSR
inline lcg_bsr_report decode_sbsr(byte_buffer_view payload)
{
  srsran_sanity_check(not payload.empty(), "Trying to decode SBSR but payload is empty.");
  lcg_bsr_report sbsr = {};
  sbsr.lcg_id         = uint_to_lcg_id((payload[0] & 0xe0U) >> 5U);
  sbsr.buffer_size    = payload[0] & 0x1fU;
  return sbsr;
}

struct long_bsr_report {
  lcg_bsr_report_list list; /// one entry for each reported LCG
};

/// Decode Long BSR
expected<long_bsr_report> decode_lbsr(bsr_format format, byte_buffer_view payload);

/// Converts the buffer size field of a BSR (5 or 8-bit Buffer Size field) into Nof Bytes.
/// \param buff_size_index The buffer size field contained in the MAC PDU.
/// \param format The BSR format that determines the buffer size field length.
/// \return The actual buffer size level in Bytes.
uint32_t buff_size_field_to_bytes(size_t buff_size_index, bsr_format format);

} // namespace srsran
