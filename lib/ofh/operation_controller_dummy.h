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

#include "srsran/ofh/ofh_controller.h"

namespace srsran {
namespace ofh {

/// Operation controller dummy implementation.
class operation_controller_dummy : public operation_controller
{
public:
  // See interface for documentation.
  void start() override {}

  // See interface for documentation.
  void stop() override {}
};

} // namespace ofh
} // namespace srsran
