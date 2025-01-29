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
#include <cstdint>

namespace srsran {

using pos_meas_id_t = uint8_t;

struct positioning_measurement_request {
  pos_meas_id_t                pos_id;
  std::optional<du_ue_index_t> ue_to_measure;
  srs_config                   srs_to_measure;
};

/// Interfaces used to start and stop new positioning measurements in the scheduler
class scheduler_positioning_handler
{
public:
  virtual ~scheduler_positioning_handler() = default;

  /// Initiates a new positioning measurement.
  virtual void handle_positioning_measurement_request(const positioning_measurement_request& req) = 0;

  /// Shuts down an on-going positioning measurement
  virtual void handle_positioning_measurement_stop(pos_meas_id_t pos_id) = 0;
};

} // namespace srsran
