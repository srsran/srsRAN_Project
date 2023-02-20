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

#include "srsgnb/ran/slot_point.h"

namespace srsran {

/// Radio Unit timing notifier interface.
class ru_timing_notifier
{
public:
  virtual ~ru_timing_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// Notifies that the processing of a new slot has started.
  ///
  /// \param[in] slot Current slot.
  virtual void on_tti_boundary(slot_point slot) = 0;
};

} // namespace srsran