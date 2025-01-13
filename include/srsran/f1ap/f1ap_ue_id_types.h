/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include <cstdint>

namespace srsran {

/// \brief GNB-CU-UE-F1AP-ID used to identify the UE in the F1AP-CU.
/// \remark See TS 38.473 Section 9.3.1.4: GNB-CU-UE-F1AP-ID valid values: (0..2^32-1).
constexpr uint64_t MAX_NOF_CU_F1AP_UES = uint64_t(1) << 32;
enum class gnb_cu_ue_f1ap_id_t : uint64_t { min = 0, max = MAX_NOF_CU_F1AP_UES - 1, invalid = 0x1ffffffff };

/// Converts a GNB-CU-UE-F1AP-ID to an integer.
constexpr uint64_t gnb_cu_ue_f1ap_id_to_uint(gnb_cu_ue_f1ap_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Converts an integer to a GNB-CU-UE-F1AP-ID type.
constexpr gnb_cu_ue_f1ap_id_t int_to_gnb_cu_ue_f1ap_id(uint64_t idx)
{
  return static_cast<gnb_cu_ue_f1ap_id_t>(idx);
}

/// \brief GNB-DU-UE-F1AP-ID used to identify the UE in the F1AP-DU.
/// \remark See TS 38.473 Section 9.3.1.5: GNB-DU-UE-F1AP-ID valid values: (0..2^32-1).
constexpr uint64_t MAX_NOF_DU_F1AP_UES = uint64_t(1) << 32;
enum class gnb_du_ue_f1ap_id_t : uint64_t { min = 0, max = MAX_NOF_DU_F1AP_UES - 1, invalid = 0x1ffffffff };

/// Converts a GNB-DU-UE-F1AP-ID to an integer.
constexpr uint64_t gnb_du_ue_f1ap_id_to_uint(gnb_du_ue_f1ap_id_t id)
{
  return static_cast<uint64_t>(id);
}

/// Convert an integer to a GNB-DU-UE-F1AP-ID type.
constexpr gnb_du_ue_f1ap_id_t int_to_gnb_du_ue_f1ap_id(uint64_t idx)
{
  return static_cast<gnb_du_ue_f1ap_id_t>(idx);
}

} // namespace srsran
