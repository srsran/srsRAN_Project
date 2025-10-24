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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/csi_rs/csi_rs_id.h"
#include "srsran/ran/ssb/ssb_configuration.h"
#include <variant>

namespace srsran {

/// Defines which resource types to be used for the Radio Link Monitoring Configuration.
enum class rlm_resource_type : uint8_t { default_type = 0, ssb, csi_rs, ssb_and_csi_rs };

enum class rlm_res_id_t : uint8_t { MIN_RLM_RES_ID = 0, MAX_RLM_RES_ID = 9, MAX_NOF_RLF_RESOURCES = 10 };

constexpr rlm_res_id_t to_rlm_res_id(std::underlying_type_t<rlm_res_id_t> idx)
{
  return static_cast<rlm_res_id_t>(idx);
}

/// "RadioLinkMonitoringConfig" as per TS 38.331.
struct radio_link_monitoring_config {
  struct radio_link_monitoring_rs {
    enum class purpose { beam_failure = 0, rlf, both };

    rlm_res_id_t                                res_id;
    purpose                                     resource_purpose;
    std::variant<ssb_id_t, nzp_csi_rs_res_id_t> detection_resource;

    bool operator==(const radio_link_monitoring_rs& rhs) const
    {
      return resource_purpose == rhs.resource_purpose and detection_resource == rhs.detection_resource;
    }
  };

  static_vector<radio_link_monitoring_rs, static_cast<size_t>(rlm_res_id_t::MAX_NOF_RLF_RESOURCES)> rlm_resources;

  // TODO: to be expanded with Beam Failure Resources, when supported.

  bool operator==(const radio_link_monitoring_config& rhs) const { return rlm_resources == rhs.rlm_resources; }

  bool operator!=(const radio_link_monitoring_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
