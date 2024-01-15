/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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