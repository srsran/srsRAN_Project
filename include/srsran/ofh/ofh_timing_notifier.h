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

#include "srsran/ran/slot_point.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul interface to notify timing related events.
class timing_notifier
{
public:
  /// Default destructor.
  virtual ~timing_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// Notifies that the processing of a new slot has started. It indicates in \c slot the next slot available for
  /// transmission.
  ///
  /// \param[in] context Notification context.
  virtual void on_tti_boundary(slot_point slot) = 0;
};

} // namespace ofh
} // namespace srsran
