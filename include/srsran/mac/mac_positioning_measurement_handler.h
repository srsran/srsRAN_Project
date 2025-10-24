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

#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/srs/srs_configuration.h"
#include "srsran/support/async/async_task.h"
#include <optional>

namespace srsran {

/// Request to perform a new positioning measurement.
struct mac_positioning_measurement_request {
  struct cell_info {
    du_cell_index_t cell_index;
    /// UE identifier in case the SRSConfig belongs to a UE connected to this DU. Otherwise, it is empty.
    std::optional<du_ue_index_t> ue_index;
    std::optional<rnti_t>        rnti;
    /// SRS resources to measure.
    srs_config srs_to_meas;
  };

  /// Cells for which to perform measurements.
  std::vector<cell_info> cells;
};

/// Result of a positioning measurement request.
struct mac_positioning_measurement_response {
  struct ul_rtoa_meas {
    phy_time_unit        ul_rtoa;
    std::optional<float> rsrp_dbfs;
  };
  struct cell_result {
    /// List of UL RToA measurements for the cell for a given request.
    std::vector<ul_rtoa_meas> ul_rtoa_meass;
  };

  /// Slot at which the measurement (SRS) was received at PHY layer.
  slot_point sl_rx;
  /// Results for the cells where positioning measurement occurred.
  std::vector<cell_result> cell_results;
};

/// Interface to request positioning measurements from MAC.
class mac_positioning_measurement_handler
{
public:
  virtual ~mac_positioning_measurement_handler() = default;

  /// Request a new positioning measurement.
  virtual async_task<mac_positioning_measurement_response>
  handle_positioning_measurement_request(const mac_positioning_measurement_request& req) = 0;
};

} // namespace srsran
