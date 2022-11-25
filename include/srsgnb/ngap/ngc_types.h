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

/// \brief AMF_UE_ID (non ASN1 type of AMF_UE_NGAP_ID) used to identify the UE in the AMF.
/// \remark See TS 38.413 Section 9.3.3.1: AMF_UE_NGAP_ID valid values: (0..2^40-1)
enum class amf_ue_id_t : uint64_t { min = 0, max = std::numeric_limits<uint64_t>::max(), invalid = 0x1ffffffffff };

/// Convert AMF_UE_NGAP_ID type to integer.
inline uint64_t amf_ue_id_to_uint(amf_ue_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to AMF_UE_NGAP_ID type.
inline amf_ue_id_t uint_to_amf_ue_id(std::underlying_type_t<amf_ue_id_t> id)
{
  return static_cast<amf_ue_id_t>(id);
}

/// \brief RAN_UE_ID (non ASN1 type of RAN_UE_NGAP_ID).
/// \remark See TS 38.413 Section 9.3.3.2: RAN_UE_NGAP_ID valid values: (0..2^32-1)
enum class ran_ue_id_t : uint64_t { min = 0, max = std::numeric_limits<uint64_t>::max(), invalid = 0x1ffffffff };

/// Convert RAN_UE_NGAP_ID type to integer.
inline uint64_t ran_ue_id_to_uint(ran_ue_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to RAN_UE_NGAP_ID type.
inline ran_ue_id_t uint_to_ran_ue_id(std::underlying_type_t<ran_ue_id_t> id)
{
  return static_cast<ran_ue_id_t>(id);
}

/// \brief UE_NGAP_ID internally used to identify the UE in the NGAP.
/// \remark The UE_NGAP_ID is derived from the DU index the UE is connected to an the UE index
/// by (DU_INDEX * MAX_NOF_UES) + UE_INDEX
enum class ue_ngap_id_t : uint64_t {
  min     = 0,
  max     = (MAX_NOF_DUS * MAX_NOF_UES) + MAX_NOF_UES,
  invalid = (MAX_NOF_DUS * MAX_NOF_UES) + MAX_NOF_UES + 1
};

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

inline ue_index_t get_ue_index_from_ue_ngap_id(ue_ngap_id_t ngap_id)
{
  return int_to_ue_index(ue_ngap_id_to_uint(ngap_id) % MAX_NOF_UES);
}

inline du_index_t get_du_index_from_ue_ngap_id(ue_ngap_id_t ngap_id)
{
  std::underlying_type_t<ue_index_t> ue_idx = get_ue_index_from_ue_ngap_id(ngap_id);
  return int_to_du_index((ue_ngap_id_to_uint(ngap_id) - ue_idx) / MAX_NOF_UES);
}

} // namespace srs_cu_cp
} // namespace srsgnb