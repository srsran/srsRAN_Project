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

#include "srsran/cu_cp/cu_cp_types.h"
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

} // namespace srs_cu_cp
} // namespace srsran