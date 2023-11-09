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

#include "srsran/ofh/ofh_ota_symbol_boundary_notifier.h"
#include <vector>

namespace srsran {
namespace ofh {

/// \brief OTA symbol boundary notifier subscriber.
///
/// Interface to subscribe/unsubscribe OTA symbol boundary notifiers.
class ota_symbol_boundary_notifier_subscriber
{
public:
  /// Default destructor.
  virtual ~ota_symbol_boundary_notifier_subscriber() = default;

  /// Subscribes the given notifiers to listen to OTA symbol boundary events.
  virtual void subscribe(std::vector<ota_symbol_boundary_notifier*> notifiers) = 0;

  /// Unsubscribes all the registered notifiers.
  virtual void unsubscribe_all() = 0;
};

} // namespace ofh
} // namespace srsran
