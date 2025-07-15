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

#include "srsran/phy/upper/channel_processors/prach_detector.h"
#include "srsran/support/memory_pool/bounded_object_pool.h"

namespace srsran {

/// Concurrent PRACH detector pool.
class prach_detector_pool : public prach_detector
{
public:
  using detector_pool = bounded_object_pool<prach_detector>;

  /// Creates a PRACH detector pool from a shared detector pool.
  explicit prach_detector_pool(std::shared_ptr<detector_pool> detectors_) : detectors(std::move(detectors_)) {}

  // See interface for documentation.
  prach_detection_result detect(const prach_buffer& input, const configuration& config) override
  {
    auto detector = detectors->get();
    report_fatal_error_if_not(detector, "Failed to retrieve PRACH detector.");
    return detector->detect(input, config);
  }

private:
  std::shared_ptr<detector_pool> detectors;
};

} // namespace srsran
