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

#include "srsran/adt/optional.h"
#include <cstdint>
#include <limits>

namespace srsran {
namespace srs_cu_cp {

/// \brief AMF_UE_ID (non ASN1 type of AMF_UE_NGAP_ID) used to identify the UE in the AMF.
/// \remark See TS 38.413 Section 9.3.3.1: AMF_UE_NGAP_ID valid values: (0..2^40-1)
constexpr static uint64_t MAX_NOF_AMF_UES = ((uint64_t)1 << 40);
enum class amf_ue_id_t : uint64_t { min = 0, max = MAX_NOF_AMF_UES - 1, invalid = 0x1ffffffffff };

/// Convert AMF_UE_ID type to integer.
inline uint64_t amf_ue_id_to_uint(amf_ue_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to AMF_UE_ID type.
inline amf_ue_id_t uint_to_amf_ue_id(std::underlying_type_t<amf_ue_id_t> id)
{
  return static_cast<amf_ue_id_t>(id);
}

// Globally unique AMF identifier.
struct guami_t {
  std::string plmn;
  uint16_t    amf_set_id;
  uint8_t     amf_pointer;
  uint8_t     amf_region_id;
};

struct ngap_ue_aggr_max_bit_rate {
  uint64_t ue_aggr_max_bit_rate_dl;
  uint64_t ue_aggr_max_bit_rate_ul;
};

} // namespace srs_cu_cp
} // namespace srsran