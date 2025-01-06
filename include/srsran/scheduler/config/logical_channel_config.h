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

#include "srsran/ran/logical_channel/lcid.h"
#include "srsran/ran/qos/five_qi_qos_mapping.h"
#include "srsran/ran/qos/qos_parameters.h"
#include "srsran/ran/rrm.h"
#include "srsran/ran/sr_configuration.h"
#include "srsran/scheduler/config/logical_channel_group.h"

namespace srsran {

/// \c LogicalChannelConfig, TS 38.331.
/// Information relative to the scheduling of a logical channel in the MAC scheduler.
struct logical_channel_config {
  /// QoS specific features associated with a logical channel (only used by DRBs).
  struct qos_info {
    /// QoS characteristics associated with the logical channel.
    standardized_qos_characteristics qos;
    /// QoS information present only for GBR QoS flows.
    std::optional<gbr_qos_flow_information> gbr_qos_info;

    bool operator==(const qos_info& rhs) const { return qos == rhs.qos && gbr_qos_info == rhs.gbr_qos_info; }
  };

  lcid_t   lcid;
  lcg_id_t lc_group;
  /// Slice associated with this Bearer.
  rrm_policy_member rrm_policy;
  /// QoS information associated with this logical channel.
  std::optional<qos_info> qos;
  // TODO: add remaining fields;
  std::optional<scheduling_request_id> sr_id;
  bool                                 lc_sr_mask;
  bool                                 lc_sr_delay_timer_applied;

  bool operator==(const logical_channel_config& rhs) const
  {
    return lcid == rhs.lcid and lc_group == rhs.lc_group and rrm_policy == rhs.rrm_policy and qos == rhs.qos and
           sr_id == rhs.sr_id and lc_sr_mask == rhs.lc_sr_mask and
           lc_sr_delay_timer_applied == rhs.lc_sr_delay_timer_applied;
  }
};

} // namespace srsran
