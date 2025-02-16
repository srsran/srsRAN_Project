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

#include "resource_usage_utils.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace resource_usage_utils {

/// RAII class that starts resource usage measurement on construction and writes measured resource usage at the
/// destruction.
class scoped_resource_usage
{
public:
  /// Constructor receives measurement struct that needs to be filled and measurement type.
  scoped_resource_usage(measurements& measurements_, rusage_measurement_type type_ = rusage_measurement_type::NONE) :
    meas(measurements_), type(type_)
  {
    start_tp = std::chrono::high_resolution_clock::now();

    if (type != rusage_measurement_type::NONE) {
      start_snapshot = cpu_usage_now(type);
      if (!start_snapshot) {
        srslog::fetch_basic_logger("METRICS").warning(
            "Scoped resource usage tracker failed to query current resource usage, errno={}", start_snapshot.error());
      }
    }
  }

  /// Destructor saves measurements in the stored object.
  ~scoped_resource_usage()
  {
    rusage_meas_time_point end_tp = std::chrono::high_resolution_clock::now();
    meas.duration                 = std::chrono::duration_cast<rusage_meas_duration>(end_tp - start_tp);

    if (type == rusage_measurement_type::NONE) {
      return;
    }

    if (!start_snapshot) {
      meas.reset();
      return;
    }
    auto current_snapshot = cpu_usage_now(type);
    if (!current_snapshot) {
      srslog::fetch_basic_logger("METRICS").warning(
          "Scoped resource usage tracker failed to query current resource usage, errno={}", current_snapshot.error());
      meas.reset();
      return;
    }
    meas.user_time   = (current_snapshot.value().user_time - start_snapshot.value().user_time);
    meas.system_time = (current_snapshot.value().system_time - start_snapshot.value().system_time);
    meas.max_rss     = current_snapshot.value().max_rss;
  }

private:
  measurements&               meas;
  rusage_measurement_type     type;
  rusage_meas_time_point      start_tp;
  expected<cpu_snapshot, int> start_snapshot;
};

} // namespace resource_usage_utils
} // namespace srsran
