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

#include "srsran/pdcp/pdcp_sn_size.h"
#include "srsran/ran/cause/f1ap_cause.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/up_transport_layer_info.h"
#include "srsran/rlc/rlc_mode.h"

namespace srsran {

/// Parameters of a failed SRB setup in the DU UE context.
struct f1ap_srb_failed_to_setup {
  srb_id_t srb_id = srb_id_t::nulltype;
  /// Reason for the failure
  std::optional<f1ap_cause_t> cause;
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
};

/// Parameters of a failed DRB setup/modify in the DU UE context.
struct f1ap_drb_failed_to_setupmod {
  /// DRB-Id of the failed to setup/modify DRB.
  drb_id_t drb_id = drb_id_t::invalid;
  /// Reason for the failure
  std::optional<f1ap_cause_t> cause;
};

} // namespace srsran