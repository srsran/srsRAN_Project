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

#include <cstdint>
#include <limits>
#include <type_traits>

namespace srsgnb {

/// \brief GNB-CU-CP-UE-E1AP-ID used to identify the UE in the CU-CP E1AP.
/// \remark See TS 38.463 Section 9.3.1.4: GNB-CU-UE-E1AP-ID valid values: (0..2^32-1)
constexpr static uint64_t MAX_NOF_CU_CP_E1AP_UES = ((uint64_t)1 << 32);
enum class gnb_cu_cp_ue_e1ap_id_t : uint64_t { min = 0, max = MAX_NOF_CU_CP_E1AP_UES - 1, invalid = 0x1ffffffff };

constexpr inline uint64_t gnb_cu_cp_ue_e1ap_id_to_uint(gnb_cu_cp_ue_e1ap_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to GNB-CU-CP-UE-E1AP-ID type.
constexpr inline gnb_cu_cp_ue_e1ap_id_t int_to_gnb_cu_cp_ue_e1ap_id(uint64_t idx)
{
  return static_cast<gnb_cu_cp_ue_e1ap_id_t>(idx);
}

/// \brief GNB-CU-UP-UE-E1AP-ID used to identify the UE in the CU-UP E1AP.
/// \remark See TS 38.473 Section 9.3.1.5: GNB-CU-UP-UE-E1AP-ID valid values: (0..2^32-1)
constexpr static uint64_t MAX_NOF_CU_UP_E1AP_UES = ((uint64_t)1 << 32);
enum class gnb_cu_up_ue_e1ap_id_t : uint64_t { min = 0, max = MAX_NOF_CU_CP_E1AP_UES - 1, invalid = 0x1ffffffff };

constexpr inline uint64_t gnb_cu_up_ue_e1ap_id_to_uint(gnb_cu_up_ue_e1ap_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert integer to GNB-DU-UE-E1AP-ID type.
constexpr inline gnb_cu_up_ue_e1ap_id_t int_to_gnb_cu_up_ue_e1ap_id(uint64_t idx)
{
  return static_cast<gnb_cu_up_ue_e1ap_id_t>(idx);
}

} // namespace srsgnb
