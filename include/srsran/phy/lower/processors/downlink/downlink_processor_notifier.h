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

struct lower_phy_baseband_metrics;
struct lower_phy_timing_context;
struct resource_grid_context;

/// \brief Lower physical layer downlink processor - Notifier interface.
///
/// Notifies time boundary events.
class downlink_processor_notifier
{
public:
  /// Default destructor.
  virtual ~downlink_processor_notifier() = default;

  /// \brief Notifies a new TTI boundary event.
  ///
  /// See \ref lower_phy_timing_notifier::on_tti_boundary for more information.
  /// \param[in] context Notification context.
  virtual void on_tti_boundary(const lower_phy_timing_context& context) = 0;

  /// \brief Notifies a new measurement of downlink baseband metrics.
  virtual void on_new_metrics(const lower_phy_baseband_metrics& metrics) = 0;
};

} // namespace srsran
