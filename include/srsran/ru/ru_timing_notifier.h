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

#include "srsran/ran/slot_point.h"

namespace srsran {

/// Radio Unit interface to notify timing related events.
class ru_timing_notifier
{
public:
  /// Default destructor.
  virtual ~ru_timing_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// Notifies that the processing of a new slot has started.
  ///
  /// \param[in] slot Current slot.
  virtual void on_tti_boundary(slot_point slot) = 0;

  /// \brief Notifies that an uplink half slot has been received and processed by the Radio Unit.
  ///
  /// \param[in] slot Current slot.
  virtual void on_ul_half_slot_boundary(slot_point slot) = 0;

  /// \brief Notifies that an uplink full slot has been received and processed by the Radio Unit.
  ///
  /// \param[in] slot Current slot.
  virtual void on_ul_full_slot_boundary(slot_point slot) = 0;
};

} // namespace srsran
