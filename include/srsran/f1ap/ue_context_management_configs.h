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

#include "srsran/pdcp/pdcp_sn_size.h"
#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/ran/qos/qos_flow_id.h"
#include "srsran/ran/qos/qos_parameters.h"
#include "srsran/ran/rb_id.h"
#include "srsran/ran/s_nssai.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/rlc/rlc_mode.h"

namespace srsran {

/// Parameters of a SRB to setup in the DU UE context.
struct f1ap_srb_to_setup {
  srb_id_t srb_id = srb_id_t::nulltype;
};

/// Parameters of a failed SRB setup in the DU UE context.
struct f1ap_srb_failed_to_setup {
  srb_id_t srb_id = srb_id_t::nulltype;
  /// Reason for the failure.
  std::optional<f1ap_cause_t> cause;
};

/// Used to activate notification control for a given DRB.
enum class drb_notification_control { active = 0, not_active };

struct flow_mapped_to_drb {
  qos_flow_id_t                 qos_flow_id = qos_flow_id_t::invalid;
  qos_flow_level_qos_parameters qos_flow_level_qos_params;
};

struct f1ap_drb_info {
  qos_flow_level_qos_parameters drb_qos;
  s_nssai_t                     s_nssai;
  /// \brief Establishes whether notification control is active.
  ///
  /// [TS 38.473 8.3.1.2] If the Notification Control IE is included in the DRB to Be Setup List IE and it is set to
  /// active, the gNB-DU shall, if supported, monitor the QoS of the DRB and notify the gNB-CU if the QoS cannot be
  /// fulfilled any longer or if the QoS can be fulfilled again. The Notification Control IE can only be applied to GBR
  /// bearers.
  std::optional<drb_notification_control> notif_ctrl;
  std::vector<flow_mapped_to_drb>         flows_mapped_to_drb_list;
};

/// Parameters of a new DRB to be setup in the DU UE context.
struct f1ap_drb_to_setup {
  /// DRB-Id of the new DRB.
  drb_id_t drb_id = drb_id_t::invalid;
  /// RLC mode of the new DRB (e.g. AM, UM-BiDir, etc).
  rlc_mode mode;
  /// UL Transport layer info for the given DRB. See ulUPTNLInformation-ToBeSetup-List in TS 38.473.
  std::vector<up_transport_layer_info> uluptnl_info_list;
  /// \brief PDCP SN length of the DRB.
  /// \remark (Implementation-defined) We use the same length for DL and UL.
  pdcp_sn_size pdcp_sn_len = pdcp_sn_size::invalid;
  /// QoS Information of the DRB.
  f1ap_drb_info qos_info;
};

/// New parameters to set for an existing DRB in the DU UE context.
struct f1ap_drb_to_modify {
  /// DRB-Id of the existing DRB.
  drb_id_t drb_id = drb_id_t::invalid;
  /// UL Transport layer info for the given DRB. See ulUPTNLInformation-ToBeSetup-List in TS 38.473.
  std::vector<up_transport_layer_info> uluptnl_info_list;
};

/// Parameters of a setup/modified DRB in the DU UE context.
struct f1ap_drb_setupmod {
  /// DRB-Id of the setup/modified DRB.
  drb_id_t drb_id = drb_id_t::invalid;
  /// LCID assigned to the setup/modified DRB.
  std::optional<lcid_t> lcid;
  /// DL Transport layer info for the given DRB.
  std::vector<up_transport_layer_info> dluptnl_info_list;
  /// Indicates whether the RLC has been reestablished.
  bool rlc_reestablished = false;
};

/// Parameters of a failed DRB setup/modify in the DU UE context.
struct f1ap_drb_failed_to_setupmod {
  /// DRB-Id of the failed to setup/modify DRB.
  drb_id_t drb_id = drb_id_t::invalid;
  /// Reason for the failure.
  std::optional<f1ap_cause_t> cause;
};

} // namespace srsran
