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

#include "srsgnb/cu_cp/cu_cp_types.h"
#include <cstdint>
#include <limits>

namespace srsgnb {
namespace srs_cu_cp {

/// \brief UE_NGAP_ID (non ASN1 type of RAN_UE_NGAP_ID) used to identify the UE in the NGAP.
/// \remark See TS 38.413 Section 9.3.3.2: RAN_UE_NGAP_ID valid values: (0..2^32-1)
enum class ue_ngap_id_t : uint64_t { min = 0, max = std::numeric_limits<uint64_t>::max(), invalid = 0x1ffffffff };

/// Convert RAN-UE-NGAP-ID type to integer.
inline uint64_t ue_ngap_id_to_uint(ue_ngap_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to RAN-UE-NGAP-ID type.
inline ue_ngap_id_t uint_to_ue_ngap_id(std::underlying_type_t<ue_ngap_id_t> id)
{
  return static_cast<ue_ngap_id_t>(id);
}

inline ue_ngap_id_t get_ue_ngap_id(du_index_t du_index, ue_index_t ue_index)
{
  return uint_to_ue_ngap_id((du_index * MAX_NOF_UES) + ue_index);
}

inline ue_ngap_id_t get_ue_ngap_id(std::underlying_type_t<du_index_t> du_index,
                                   std::underlying_type_t<ue_index_t> ue_index)
{
  return uint_to_ue_ngap_id((du_index * MAX_NOF_UES) + ue_index);
}

} // namespace srs_cu_cp
} // namespace srsgnb