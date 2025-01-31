/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
