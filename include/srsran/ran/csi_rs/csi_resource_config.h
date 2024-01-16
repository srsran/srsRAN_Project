/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "csi_rs_constants.h"
#include "csi_rs_id.h"
#include "srsran/adt/static_vector.h"
#include "srsran/adt/variant.h"
#include "srsran/scheduler/config/bwp_configuration.h"

namespace srsran {

/// \brief CSI-ResourceConfigId is used to identify a CSI-ResourceConfig.
/// \remark See TS 38.331, \c CSI-ResourceConfigId.
enum csi_res_config_id_t : uint8_t {
  MIN_CSI_RESOURCE_CONFIG_ID   = 0,
  MAX_CSI_RESOURCE_CONFIG_ID   = 111,
  MAX_NOF_CSI_RESOURCE_CONFIGS = 112,
};

/// CSI-ResourceConfig defines a group of one or more NZP-CSI-RS-ResourceSet, CSI-IM-ResourceSet and/or
/// CSI-SSB-ResourceSet.
/// \remark See TS 38.331, \c CSI-ResourceConfig.
struct csi_resource_config {
  /// List of references to CSI-IM resources used for beam measurement and reporting in a CSI-RS resource set. Contains
  /// up to maxNrofCSI-IM-ResourceSetsPerConfig resource sets if resourceType is 'aperiodic' and 1 otherwise.
  using csi_im_resource_set_list =
      static_vector<csi_im_res_set_id_t, MAX_NOF_CSI_IM_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG>;

  struct nzp_csi_rs_ssb {
    /// List of references to NZP CSI-RS resources used for beam measurement and reporting in a CSI-RS resource set.
    /// Contains up to maxNrofNZP-CSI-RS-ResourceSetsPerConfig resource sets if resourceType is 'aperiodic' and 1
    /// otherwise.
    static_vector<nzp_csi_rs_res_set_id_t, MAX_NOF_NZP_CSI_RS_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG>
        nzp_csi_rs_res_set_list;
    /// List of references to SSB resources used for beam measurement and reporting in a CSI-RS resource set.
    static_vector<csi_ssb_res_set_id_t, MAX_NOF_CSI_SSB_RESOURCE_SETS_PER_CSI_RESOURCE_CONFIG> csi_ssb_res_set_list;

    bool operator==(const nzp_csi_rs_ssb& rhs) const
    {
      return nzp_csi_rs_res_set_list == rhs.nzp_csi_rs_res_set_list && csi_ssb_res_set_list == rhs.csi_ssb_res_set_list;
    }
    bool operator!=(const nzp_csi_rs_ssb& rhs) const { return !(rhs == *this); }
  };

  /// Time domain behavior of resource configuration. It does not apply to resources provided in the
  /// csi-SSB-ResourceSetList.
  /// \remark See TS 38.214, clause 5.2.1.2 and TS 38.331, \c CSI-ResourceConfig.
  enum class resource_type { aperiodic, semiPersistent, periodic };

  csi_res_config_id_t                               res_cfg_id;
  variant<nzp_csi_rs_ssb, csi_im_resource_set_list> csi_rs_res_set_list;
  /// The DL BWP which the CSI-RS associated with this CSI-ResourceConfig are located in.
  bwp_id_t      bwp_id;
  resource_type res_type;

  bool operator==(const csi_resource_config& rhs) const
  {
    return res_cfg_id == rhs.res_cfg_id && csi_rs_res_set_list == rhs.csi_rs_res_set_list && bwp_id == rhs.bwp_id &&
           res_type == rhs.res_type;
  }
  bool operator!=(const csi_resource_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
