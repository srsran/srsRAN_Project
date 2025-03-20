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

#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/transmitter/ofh_downlink_handler.h"
#include "srsran/ofh/transmitter/ofh_uplink_request_handler.h"

namespace srsran {
namespace ofh {

class transmitter_metrics_collector;

/// Open Fronthaul transmitter interface.
class transmitter
{
public:
  /// Default destructor.
  virtual ~transmitter() = default;

  /// Returns the uplink request handler of this Open Fronthaul transmitter.
  virtual uplink_request_handler& get_uplink_request_handler() = 0;

  /// Returns the downlink handler of this Open Fronthaul transmitter.
  virtual downlink_handler& get_downlink_handler() = 0;

  /// Returns the OTA symbol boundary notifier of this Open Fronthaul transmitter.
  virtual ota_symbol_boundary_notifier& get_ota_symbol_boundary_notifier() = 0;

  /// Returns the metrics collector of this Open Fronthaul transmitter or nullptr if metrics are disabled.
  virtual transmitter_metrics_collector* get_metrics_collector() = 0;
};

} // namespace ofh
} // namespace srsran
