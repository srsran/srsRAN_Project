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
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier_manager.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul timing manager.
class timing_manager
{
public:
  /// Default destructor.
  virtual ~timing_manager() = default;

  /// Returns the controller of this timing manager.
  virtual operation_controller& get_controller() = 0;

  /// Returns the OTA symbol boundary notifier manager of this timing manager.
  virtual ota_symbol_boundary_notifier_manager& get_ota_symbol_boundary_notifier_manager() = 0;
};

} // namespace ofh
} // namespace srsran
