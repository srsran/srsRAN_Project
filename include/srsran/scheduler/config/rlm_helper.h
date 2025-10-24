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

#include "srsran/adt/span.h"
#include "srsran/ran/csi_rs/csi_meas_config.h"
#include "srsran/ran/radio_link_monitoring.h"

namespace srsran {
namespace rlm_helper {

struct rlm_builder_params {
  /// Parameters that are needed only when the RLM uses SSB as detection resources.
  struct rlm_ssb_params {
    /// SSB bitmap and beam IDs, used to define which SSB ID should be used for RLM.
    uint64_t                       ssb_bitmap;
    std::array<uint8_t, NOF_BEAMS> ssb_beam_ids;
  };

  rlm_builder_params() = default;
  rlm_builder_params(rlm_resource_type resource_type_, uint8_t L_max_) : resource_type(resource_type_), L_max(L_max_)
  {
    // Meant for rlm_resource_type::csi_rs only.
  }
  rlm_builder_params(rlm_resource_type                     resource_type_,
                     uint8_t                               L_max_,
                     uint64_t                              ssb_bitmap,
                     const std::array<uint8_t, NOF_BEAMS>& ssb_beam_ids) :
    resource_type(resource_type_), L_max(L_max_), ssb_params(rlm_ssb_params{ssb_bitmap, ssb_beam_ids})
  {
  }

  /// Defines which resources (e.g, default, SSB, CSI-RS, SSB and CSI-RS) the UE should use for RLM.
  rlm_resource_type resource_type = rlm_resource_type::default_type;
  /// Max number of SSB occasions per SSB period. Possible values are {4, 8, 64}.
  uint8_t                       L_max;
  std::optional<rlm_ssb_params> ssb_params;
};

/// Builds the Radio Link Monitoring configuration.
/// \param[in] params Parameters set by the user and other parameters that depend on the cell configuration.
/// \param [in] csi_rs_resources List of CSI-RS resources that can be used for Radio Link Monitoring.
radio_link_monitoring_config make_radio_link_monitoring_config(const rlm_builder_params&       params,
                                                               span<const nzp_csi_rs_resource> csi_rs_resources);
} // namespace rlm_helper
} // namespace srsran
