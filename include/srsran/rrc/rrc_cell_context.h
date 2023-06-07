/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/optional.h"
#include "srsran/ran/nr_cgi.h"
#include <cstdint>

namespace srsran {

namespace srs_cu_cp {

/// TS 38.331 - MeasurementTimingConfiguration: MeasTiming::frequencyAndTiming
struct cu_cp_meas_timing_frequency_and_timing {
  uint32_t carrier_freq;
  // TODO: add remaining members with matching type
  // ssb_subcarrier_spacing;
  // ssb_measurement_timing_configuration;
  // optional<> ss_rssi_measurement;
};

/// TS 38.331 - MeasurementTimingConfiguration: MeasTiming
struct cu_cp_meas_timing {
  optional<cu_cp_meas_timing_frequency_and_timing> frequency_and_timing;
};

/// TS 38.331 - MeasurementTimingConfiguration
struct cu_cp_meas_timing_cfg {
  std::vector<cu_cp_meas_timing> meas_timing_list;
};

// Cell-related configuration used by the RRC.
struct rrc_cell_context {
  nr_cell_global_id_t cgi;
  uint32_t            tac;
  /// Contains the MeasurementTimingConfiguration inter-node message defined in TS 38.331
  cu_cp_meas_timing_cfg meas_time_cfg;
};

} // namespace srs_cu_cp

} // namespace srsran
