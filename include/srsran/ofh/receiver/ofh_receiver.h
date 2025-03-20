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
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"

namespace srsran {
namespace ofh {

class receiver_metrics_collector;

/// Open Fronthaul receiver interface.
class receiver
{
public:
  virtual ~receiver() = default;

  /// Returns an OTA symbol boundary notifier of this Open Fronthaul receiver or nullptr if not present.
  virtual ota_symbol_boundary_notifier* get_ota_symbol_boundary_notifier() = 0;

  /// Returns the controller of this Open Fronthaul receiver.
  virtual operation_controller& get_operation_controller() = 0;

  /// Returns the metrics collector of this Open Fronthaul receiver or nullptr if metrics are disabled.
  virtual receiver_metrics_collector* get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace srsran
