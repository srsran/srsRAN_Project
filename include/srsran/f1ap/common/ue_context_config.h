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
  /// \brief PDCP SN length of the DRB.
  /// \remark (Implementation-defined) We use the same length for DL and UL.
  std::optional<pdcp_sn_size> pdcp_sn_len;
};

/// Parameters of a setup/modified DRB in the DU UE context.
struct f1ap_drb_setupmod {
  drb_id_t                             drb_id = drb_id_t::invalid;
  std::optional<lcid_t>                lcid;
  std::vector<up_transport_layer_info> dluptnl_info_list;
};

/// Parameters of a failed DRB setup/modify in the DU UE context.
struct f1ap_drb_failed_to_setupmod {
  drb_id_t                    drb_id = drb_id_t::invalid;
  std::optional<f1ap_cause_t> cause;
};

} // namespace srsran