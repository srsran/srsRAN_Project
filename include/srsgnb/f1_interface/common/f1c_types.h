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

namespace srsgnb {

/// \brief GNB-DU-UE-F1AP-ID used to identify the UE in the F1AP-DU.
enum class gnb_du_ue_f1ap_id_t : uint64_t { min = 0, max = std::numeric_limits<uint64_t>::max() };

inline uint64_t gnb_du_ue_f1ap_id_to_uint(gnb_du_ue_f1ap_id_t id)
{
  return static_cast<uint64_t>(id);
}

} // namespace srsgnb
