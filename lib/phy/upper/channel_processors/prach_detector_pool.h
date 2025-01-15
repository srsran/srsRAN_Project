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
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"

namespace srsran {

/// Concurrent PRACH detector pool.
class prach_detector_pool : public prach_detector
{
public:
  /// Creates a PRACH detector pool from a list of detectors. Ownership is transferred to the pool.
  explicit prach_detector_pool(std::vector<std::unique_ptr<prach_detector>> processors_) :
    processors(std::move(processors_))
  {
  }

  // See interface for documentation.
  prach_detection_result detect(const prach_buffer& input, const configuration& config) override
  {
    prach_detector& detector = processors.get();
    return detector.detect(input, config);
  }

private:
  concurrent_thread_local_object_pool<prach_detector> processors;
};

} // namespace srsran