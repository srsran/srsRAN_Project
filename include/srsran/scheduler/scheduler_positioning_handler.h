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

namespace srsran {

struct positioning_measurement_request {
  /// \brief This RNTI can correspond to either a real connected UE C-RNTI or an RNTI assigned just for positioning
  /// measurement.
  rnti_t pos_rnti;
  /// In case the positioning measurement is for a currently connected UE, we also define the UE index.
  std::optional<du_ue_index_t> ue_index;
  /// Cell at which the positioning is to be made.
  du_cell_index_t cell_index;
  /// SRS resources to measure.
  srs_config srs_to_measure;
};

/// Interfaces used to start and stop new positioning measurements in the scheduler
class scheduler_positioning_handler
{
public:
  virtual ~scheduler_positioning_handler() = default;

  /// Initiates a new positioning measurement.
  virtual void handle_positioning_measurement_request(const positioning_measurement_request& req) = 0;

  /// Shuts down an on-going positioning measurement
  virtual void handle_positioning_measurement_stop(du_cell_index_t cell_index, rnti_t pos_rnti) = 0;
};

} // namespace srsran
