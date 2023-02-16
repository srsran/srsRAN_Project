/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/phy/upper/channel_processors/prach_detector.h"

namespace srsgnb {

class prach_detector_spy : public prach_detector
{
  bool detect_method_been_called = false;

public:
  prach_detection_result detect(const prach_buffer& input, const configuration& config) override
  {
    detect_method_been_called = true;

    return {};
  }

  bool has_detect_method_been_called() const { return detect_method_been_called; }
};

} // namespace srsgnb
