/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1u/cu_up/f1u_config.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/ran/cu_types.h"
#include "srsran/ran/lcid.h"
#include <cstdint>
#include <type_traits>
#include <vector>

namespace srsran {
namespace srs_cu_up {

/// Maximum number of UEs supported by CU-UP (implementation-defined).
enum ue_index_t : uint16_t {
  MIN_UE_INDEX     = 0,
  MAX_UE_INDEX     = 1023,
  MAX_NOF_UES      = 1024,
  INVALID_UE_INDEX = MAX_NOF_UES
};

/// Convert integer to CU UE index type.
constexpr inline ue_index_t int_to_ue_index(std::underlying_type_t<ue_index_t> idx)
{
  return static_cast<ue_index_t>(idx);
}

constexpr inline bool is_ue_index_valid(ue_index_t ue_idx)
{
  return ue_idx < MAX_NOF_UES;
}

struct e1ap_bearer_context_inactivity_notification {
  ue_index_t                    ue_index    = ue_index_t::INVALID_UE_INDEX;
  bool                          ue_inactive = false;
  std::vector<drb_id_t>         inactive_drbs;
  std::vector<pdu_session_id_t> inactive_pdu_sessions;
};

/// QoS Configuration, i.e. 5QI and the associated SDAP, PDCP and F1-U configuration for DRBs
struct cu_up_qos_config {
  pdcp_custom_config pdcp_custom_cfg; ///< Implementation-specific parameters for PDCP
  f1u_config         f1u_cfg;         ///< Implementation-specific parameters for F1-U
};

} // namespace srs_cu_up
} // namespace srsran
