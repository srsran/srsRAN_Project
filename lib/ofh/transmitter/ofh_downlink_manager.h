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

#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/transmitter/ofh_downlink_handler.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul downlink manager.
class downlink_manager
{
public:
  /// Default destructor.
  virtual ~downlink_manager() = default;

  /// Returns the downlink handler of this downlink manager.
  virtual downlink_handler& get_downlink_handler() = 0;

  /// Returns the OTA symbol boundary notifier of this downlink manager.
  virtual ota_symbol_boundary_notifier& get_ota_symbol_boundary_notifier() = 0;
};

} // namespace ofh
} // namespace srsran
